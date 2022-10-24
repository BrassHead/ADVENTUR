/*

This file is created to provide rough compatibility with the
UNIX (tm. Bell Labs) ctime routines.

*/

#include <osbind.h>

    struct tm {
	unsigned tm_sec2 : 5;
	unsigned tm_min : 6;
	unsigned tm_hour : 5;
	unsigned tm_mday : 5;
	unsigned tm_mon : 4;
	unsigned tm_year80 : 7;
    };



