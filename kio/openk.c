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

#include <fcntl.h>
#include <io.h>

#define READONLY	_O_RDWR
#define O_RAW		_O_BINARY


#ifdef _MSDOS
#define creat _create
#define open _open
#define lseek _lseek
#define write _write
#define read _read
#endif

int openk (name)
  char *name ;
{
	(void) MakNam (name) ;

	if ( ( kfd = open (knam,READONLY|O_RAW) ) < 0 ||
	     ( rfd = open (rnam,READONLY|O_RAW) ) < 0 )
		return (ERROR) ;

	if ( read (kfd,Sblk,sizeof(Sblk)) != sizeof(Sblk) )
		error ("Openk","unable to read super block!") ;

	return (EXISTING) ;
}
