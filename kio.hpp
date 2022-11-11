/*
**		    Copyright (c) 1985	Ken Wellsch
**
**     Permission is hereby granted to all users to possess, use, copy,
**     distribute, and modify the programs and files in this package
**     provided it is not for direct commercial benefit and secondly,
**     that this notice and all modification information be kept and
**     maintained in the package.
**
*/

// keyed-IO routines

#include <fcntl.h>
#include <stdarg.h>

#if defined(_MSDOS) || defined(_WIN32)
	#include <io.h>
	#define creat _create
	#define open _open
	#define lseek _lseek
	#define write _write
	#define read _read
	#define close _close
	#define READONLY	_O_RDWR
	#define READWRITE	_O_RDWR
	#define O_RAW		_O_BINARY
#elif defined(__linux__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__APPLE__)
	#include <unistd.h>
	#include <sys/stat.h>
	//#include <sys/file.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	//#include <errno.h>
	#define READONLY	O_RDONLY
	#define READWRITE	O_RDWR
	#define O_RAW		0
#endif


#define MAXENTRIES	16
#define MAXIBLK		1024
#define EMPTY		-1
#define ERROR		-1

#define EXISTING	0
#define NEW		1

struct mkey
{
	int b ;
	int e ;
} ;

struct iblk
{
	long int i_loc[MAXENTRIES] ;
	short int i_siz[MAXENTRIES] ;
} ;

extern struct iblk Iblk ;
extern short int Sblk[MAXIBLK] ;
extern char knam[20], rnam[20] ;
extern int Modified ;
extern int CurBlk ;
extern long int RecLoc ;
extern short int IndLoc ;
extern int kfd ;
extern int rfd ;


/* Globals */
struct iblk Iblk ;

short int Sblk[MAXIBLK] ;

char knam[20], rnam[20] ;

int Modified = 0 ;
int CurBlk = -1 ;
long int RecLoc = 0L ;
short int IndLoc = 0 ;
int kfd = -1 ;
int rfd = -1 ;

//////////////////////////////////////////////

template<typename... Args> void error(const char *rnam, const char *fmt, Args... args)
{
	printf ("%s: ",rnam) ;
    // Might get a warning on the next line, but it's OK
	printf (fmt, args...) ;
	exit (1) ;
}


void MakNam (char *name)
{
	(void) strncpy (knam,name,15) ;
	(void) strncpy (rnam,name,15) ;
	(void) strncat (knam,".key",20) ;
	(void) strncat (rnam,".rec",20) ;

	return ;
}


void MapKey ( int key, struct mkey *k )
{
	k->b = key / MAXENTRIES ;
	k->e = key % MAXENTRIES ;
	return ;
}


int MakBlk (int b)
{
	int i ;

	if ( Modified )
		return (ERROR) ;

	for ( i = 0 ; i < MAXENTRIES ; i++ )
	{
		Iblk.i_loc[i] = 0 ;
		Iblk.i_siz[i] = 0 ;
	}

	Sblk[b] = IndLoc++ ;
	CurBlk = b ;
	return (b) ;
}


long int MapBlk ( int b )
{
	long int loc ;

	if ( Sblk[b] == EMPTY )
		return (ERROR) ;
	loc = ((long) Sblk[b]) * sizeof(Iblk) + sizeof(Sblk) ;
	return (loc) ;
}


int PutBlk (int b)
{
	long int loc ;

	loc = MapBlk (b) ;
	if ( loc == ERROR )
		return (ERROR) ;

	(void) lseek (kfd,loc,0) ;
	if ( write (kfd,&Iblk,sizeof(Iblk)) != sizeof(Iblk) )
		return (ERROR) ;
	return (b) ;
}


int GetBlk (int b)
{
	long int loc ;

	if ( b == CurBlk )
		return (b) ;
	
	if ( Modified )
	{
		if ( PutBlk (CurBlk) != CurBlk )
			return (ERROR) ;
		Modified = 0 ;
	}

	loc = MapBlk (b) ;
	if ( loc == ERROR )
		return (ERROR) ;

	(void) lseek (kfd,loc,0) ;
	if ( read (kfd,&Iblk,sizeof(Iblk)) != sizeof(Iblk) )
		return (ERROR) ;
	CurBlk = b ;
	return (b) ;
}


int closek (int unit)
{
	(void) close (rfd) ;

	if ( unit == NEW )
	{
		(void) lseek (kfd,0L,0) ;
		if ( write (kfd,Sblk,sizeof(Sblk)) != sizeof(Sblk) )
			error ("Closef","error on updating super block!") ;
	}

	if ( Modified )
		if ( PutBlk(CurBlk) != CurBlk )
			error ("Closef","error updating modified block!") ;

	(void) close (kfd) ;

	return (0) ;
}


int creatk (char * name)
{
	int i ;

	(void) MakNam (name) ;

	if ( ( kfd = creat (knam,0600|O_RAW) ) < 0 ||
	     ( rfd = creat (rnam,0600|O_RAW) ) < 0 )
		error ("Creatk","unable to create files %s!",name) ;

	for ( i = 0 ; i < MAXIBLK ; i++ )
		Sblk[i] = EMPTY ;

	(void) lseek (kfd,0L,0) ;
	if ( write (kfd,Sblk,sizeof(Sblk)) != sizeof(Sblk) )
		error ("Creatk","%s -- error on initializing!",knam) ;

	(void) close (kfd) ;
	(void) close (rfd) ;

	if ( ( kfd = open (knam,READWRITE|O_RAW) ) < 0 ||
	     ( rfd = open (rnam,READWRITE|O_RAW) ) < 0 )
		error ("Creatk","%s -- error reopening!",name) ;

	return (NEW) ;
}

int dupk ( int unit, int oldkey, int newkey)
{
	struct mkey k ;
	long int loc ;
	int siz ;

	if ( unit == EXISTING )
		return (ERROR) ;

	MapKey (oldkey,&k) ;

	if ( GetBlk (k.b) != k.b || Iblk.i_siz[k.e] <= 0 )
		return (ERROR) ;
	
	loc = Iblk.i_loc[k.e] ;
	siz = Iblk.i_siz[k.e] ;

	MapKey (newkey,&k) ;

	if ( GetBlk (k.b) != k.b )
		if ( MakBlk (k.b) != k.b )
			error ("Dupk","unable to create key %d!",newkey) ;

	if ( Iblk.i_siz[k.e] > 0 )
		error ("Dupk","key %d already exists!",newkey) ;

	Iblk.i_loc[k.e] = loc ;
	Iblk.i_siz[k.e] = siz ;
	Modified++ ;

	return (siz) ;
}


int GetRec (
  long int loc,
  char *buf,
  short int size
) {
	(void) lseek (rfd,loc,0) ;
	if ( read (rfd,buf,size) != size )
		return (ERROR) ;
	buf[size] = 0 ;
	return (size) ;
}


int openk (char *name)
{
	(void) MakNam (name) ;

	if ( ( kfd = open (knam,READONLY|O_RAW) ) < 0 ||
	     ( rfd = open (rnam,READONLY|O_RAW) ) < 0 )
		return (ERROR) ;

	if ( read (kfd,Sblk,sizeof(Sblk)) != sizeof(Sblk) )
		error ("Openk","unable to read super block!") ;

	return (EXISTING) ;
}


long int PutRec (char *buf, int size)
{
	long int loc ;

	loc = RecLoc ;
	(void) lseek (rfd,loc,0) ;
	if ( write (rfd,buf,size) != size )
		return (ERROR) ;
	RecLoc += ((long) size) ;
	return (loc) ;
}


int readk (int unit, int key, char *buffer, int size)
{
	struct mkey k ;

	MapKey (key,&k) ;

	if ( GetBlk (k.b) != k.b || Iblk.i_siz[k.e] == 0 )
		return (ERROR) ;

	if ( Iblk.i_siz[k.e] > size )
		error ("Readk","buffer too small (%d<%d)!",size,Iblk.i_siz[k.e]) ;

	return (GetRec(Iblk.i_loc[k.e],buffer,Iblk.i_siz[k.e])) ;
}


int writek ( int unit, int key, char *buffer, int size )
{
	struct mkey k ;

	if ( size < 1 || key < 0 || unit == EXISTING )
		return (ERROR) ;

	MapKey (key,&k) ;

	if ( GetBlk (k.b) != k.b )
		if ( MakBlk (k.b) != k.b )
			error ("Writek","unable to create key %d!",key) ;

	if ( Iblk.i_siz[k.e] > 0 )
		error ("Writek","key %d already exists!",key) ;

	Iblk.i_loc[k.e] = PutRec (buffer,size) ;
	Iblk.i_siz[k.e] = size ;
	Modified++ ;
	return (size) ;
}
