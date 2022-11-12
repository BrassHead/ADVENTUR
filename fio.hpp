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

#include <string.h>
#include <stdio.h>

#define	MAXLINE		134
#define CMEMSIZ		10000

#define NULLFILE	(FILE *)0
#define MAXPB		60

#define FILSEP 		'/'
#define	STDIN	    0
#define	STDOUT	    1

#if defined(_MSDOS) || defined(_WIN32)

	#define fileno 		_fileno
	#undef FILSEP
	#define FILSEP 		'\\'

#endif


#define PERCENT		'%'
#define BLANK		' '
#define TAB		    '\t'
#define COMMA		','
#define SLASH		'/'
#define NEWLINE		'\n'
#define ATSIGN		'@'
#define PLUS		'+'
#define MINUS		'-'

#define sep(c)		( (c) == BLANK || (c) == TAB || (c) == COMMA )
#define com(c)		( (c) == '*' || (c) == '{' )


// Forward declarations
int pop ();
int push ( int value );


namespace fio {

struct files
{
	const char *fnam ;
	int cnt ;
	FILE *fd ;
} ;

const int EOS = 0;
int inunit = STDIN ;

static char cmem[CMEMSIZ] ;
static int pcmem = 0 ;
char list = 0 ;

static struct files fx[15] =
{
	{ "(stdin)", 0, stdin },
	{ "(stdout)", 0, stdout },
	{ "(stderr)", 0, stderr },
	{ "", 0, NULLFILE },
	{ "", 0, NULLFILE },
	{ "", 0, NULLFILE },
	{ "", 0, NULLFILE },
	{ "", 0, NULLFILE },
	{ "", 0, NULLFILE },
	{ "", 0, NULLFILE },
	{ "", 0, NULLFILE },
	{ "", 0, NULLFILE },
	{ "", 0, NULLFILE },
	{ "", 0, NULLFILE },
	{ "", 0, NULLFILE }
} ;

static char pbuf[MAXPB] ;
static short int pbp = 0 ;

static char inbuf[MAXLINE] ;
static short int ibp = MAXLINE ;

template<typename... Args>
    void error( const char *rnam, const char *fmt, Args... args )
{
    if ( inunit >= 0 )
        fprintf (stderr,"%s(%d)[%s]: ",fx[inunit].fnam,
            fx[inunit].cnt,rnam) ;
    else
        fprintf (stderr,"(EOF) %s: ",rnam) ;

    // Might get a warning on the next line, but it's OK
    fprintf ( stderr, fmt, args... ) ;
    fprintf ( stderr, "\n" ) ;
    exit (1) ;
}


template<typename... Args> void errout( const char *fmt, Args... args )
{
    fprintf ( stderr, fmt, args... ) ;
    return ;
}


void prlist ( char *line )
{
	printf ("%4d %-8s %s",fx[inunit].cnt,fx[inunit].fnam,line) ;
	return ;
}


char *strsav ( const char *s )
{
    char *p ;
    int n ;

    n = strlen(s) + 1 ;
    if ( (pcmem+n) >= CMEMSIZ )
        error ("Strsave","Out of symbol space for `%-10.10s'\n",s) ;

    p = &(cmem[pcmem]) ;
    pcmem += n ;
    (void) strcpy (p,s) ;
    return (p) ;
}


int openf ( char *name )
{
	FILE *fd ;
	int unit ;
	char *s ;

	if ( ( fd = fopen (name,"r") ) == NULLFILE )
		return (-1) ;
	unit = fileno (fd) ;
	if ( ( s = strrchr (name,FILSEP) ) == (char *)0 )
		fx[unit].fnam = strsav (name) ;
	else
		fx[unit].fnam = strsav (&(s[1])) ;
	fx[unit].cnt = 0 ;
	fx[unit].fd = fd ;
	return (unit) ;
}

void closef ( int unit )
{
	if ( fx[unit].fd != NULLFILE )
	{
		fx[unit].cnt = 0 ;
		fclose (fx[unit].fd) ;
		fx[unit].fd = NULLFILE ;
	}
	return ;
}

int Getc ()
{
	int c ;
	static int Eof = 0 ;

	if ( pbp > 0 )
	{
		c = pbuf[pbp--] ;
	}
	else
	{
		if ( ibp >= MAXLINE || inbuf[ibp] == EOS )
		{
			if ( Eof )
				return (-2) ;
			for (;;)
			{
				if ( fgets (inbuf,MAXLINE,fx[inunit].fd) != 0 )
					break ;
				closef (inunit) ;
				if ( ( inunit = pop () ) == ERROR )
				{
					ibp = MAXLINE ;
					Eof = 1 ;
					return (-2) ;
				}
			}
			ibp = 0 ;
			fx[inunit].cnt++ ;
			if (list)
				prlist (inbuf) ;
		}
		c = inbuf[ibp++] ;
	}

	return (c) ;
}

void Ungetc ( int c )
{
	if ( ++pbp >= MAXPB )
		error ("Ungetc","too many characters (%d) put back!",MAXPB) ;
	pbuf[pbp] = c ;
	return ;
}

void pbstr ( const char *s )
{
	int i ;

	for ( i = strlen(s)-1 ; i >= 0 ; i-- )
		Ungetc (s[i]) ;
	return ;
}


void flushline ()
{
    int c ;

    while ( ( c = Getc () ) != '\n' )
        if ( c == EOF )
            break ;
    return ;
}


int skip ()
{
    int c ;

    for ( c = Getc () ; sep(c) ; c = Getc () )
        ;

    if ( !com(c) )
        return (c) ;

    flushline () ;
    return (NEWLINE) ;
}


} // namespace fio
