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

#include "adefs.h"

#ifdef TOS
#include <fcntl.h>
#define random() rand()
#define srandom(x) srand(x)
#endif

#ifdef _MSDOS
#define random() rand()
#define srandom(x) srand(x)
#endif

int executive (a,b)
  int a, b ;
{
	static int saveval ;

	switch (a)
	{
		case 1:
			if ( savgm() == ERROR )
				setval (b,1) ;
			else
				setval (b,0) ;
			break ;

		case 2:
			if ( resgm() == ERROR )
				setval (b,1) ;
			else
				setval (b,0) ;
			break ;

		case 3:
			if ( delgm() == ERROR )
				setval (b,1) ;
			else
				setval (b,0) ;
			break ;

		case 4:
#ifdef CACHE
			ClrCache () ;
#endif CACHE
			setval (b,0) ;
			break ;

		case 5:
			setval (b,prime()) ;
			break ;

		case 6:
			setval (b,0) ;
			break ;
		
		case 7:
			saveval = eval (b) ;
			break ;
		
		case 8:
			setval (b,saveval) ;
			break ;

		default:
			error ("Execute","bad mode %d!",a) ;
			break ;
	}
	return ;
}

int ssw (a)
  int a ;
{
#ifdef TOS

	int wiz;
	if ( (wiz = open("gandalf.wiz",O_RDONLY)) == -1 )
		return (0) ;
	close (wiz) ;
	return (1) ;

#else

	register char *p ;
	extern char *getenv() ;

	if ( ( p = getenv("WIZARD") ) == 0 )
		return (0) ;
	
	if ( strcmp (p,"Gandalf") == 0 )
		return (1) ;
	return (0) ;

#endif

}

#include <time.h>

int svar (a,b)
  int a, b ;
{
	register int c ;

#ifndef TOS
	struct tm *tp, *localtime() ;
/*	long int time(), clock ;			BrianBr 92/05/16  */
	long int clock ;

	clock = time (0) ;
	tp = localtime (&clock) ;
#else
	struct tm *tp;
	long clock;
	clock = Gettime();
	tp = &clock;
#endif

	switch (a)
	{
		case 0:
		case 1:
		case 2:
			c = 0 ;
			break ;
		case 3:
#ifdef TOS
			c = 0;
#else
			c = tp->tm_wday ;
#endif
			break ;
		case 4:
			c = tp->tm_hour ;
			break ;
		case 5:
			c = tp->tm_min ;
			break ;
		case 6:
#ifdef TOS
			c = tp->tm_sec2 * 2;
#else
			c = tp->tm_sec ;
#endif
			break ;
		case 7:
			c = 0 ;
			break ;
		case 8:
			c = ssw (b) ;
			break ;
		default:
			c = 0 ;
			break ;
	}
	return (c) ;
}

int prime ()
{
	return (0) ;
}

int rnd (n)
  int n ;
{
	return ((random())%n) ;
}
