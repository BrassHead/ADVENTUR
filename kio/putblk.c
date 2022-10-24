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

#ifdef _MSDOS
#define creat _create
#define open _open
#define lseek _lseek
#define write _write
#define read _read
#endif

int PutBlk (b)
  int b ;
{
	long int loc ;
	extern long int MapBlk() ;

	loc = MapBlk (b) ;
	if ( loc == ERROR )
		return (ERROR) ;

	(void) lseek (kfd,loc,0) ;
	if ( write (kfd,&Iblk,sizeof(Iblk)) != sizeof(Iblk) )
		return (ERROR) ;
	return (b) ;
}
