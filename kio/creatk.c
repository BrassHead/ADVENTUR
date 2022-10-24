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

#include "kio.h"

#define READWRITE	_O_RDWR
#define O_RAW		_O_BINARY

#include <fcntl.h>
#include <io.h>

#ifdef _MSDOS
#define creat _creat
#define open _open
#define lseek _lseek
#define write _write
#define read _read
#define close _close
#endif


int creatk (name)
  char *name ;
{
	register int i ;

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
