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

#include "kio.hpp"
using kio::writek ;
using kio::creatk ;
using kio::closek ;
using kio::dupk ;

#include "fio.hpp"
using namespace fio ;

#define NEXT	    4
#define	OK		    1
#define	MAJOR	    0
#define	EOFFLAG		(-2)
#define NIL		    ((struct symstr *)0)
#define	YES		    1
#define	NO		    0
#define	KEEP	    1
#define	RELEASE	    0

#define NULLOBJECT	">$<"
#define LINKPLACE	">*<"
#define SHORTDESC	1
#define LONGDESC	2

#define STATES		25
#define	MAXCOM		16
#define MATCHCOM	3
#define	MAXOPS		58
#define MATCHOPS	4

#define	ACTION		0
#define	AT		    1
#define	DEFINE		2
#define	INCLUDE		3
#define	INITIAL		4
#define	LABEL		5
#define	LIST		6
#define	NOLIST		7
#define	NULLCLASS	8
#define	OBJECT		9
#define	PLACE		10
#define	REPEAT		11
#define	SYNON		12
#define	TEXT		13
#define	VARIABLE	14
#define	VERB		15

#define	MAXOBJECTS	120
#define MAXOTEXT	20
#define MAXOCODE	4
#define	MAXPLACES	350
#define MAXPCODE	8
#define	MAXVERBS	160
#define MAXVCODE	12

#define MAXKEY		14000
#define	NULLWORD	12000
#define	SYMTABREC	13000
#define	SYMPERREC	20

#define MAXCBUF		420
#define MAXTBUF		3000

#define MEMSIZ		1100
#define TABSIZ		256


#define status(key)	( state[(key)/1000] )

#define isalnum(c)	(((c)>='a'&&(c)<='z')|| \
             ((c)>='A'&&(c)<='Z')|| \
             ((c)>='0'&&(c)<='9'))
#define isdigit(c)	((c)>='0'&&(c)<='9')
#define isalpha(c)	(((c)>='a'&&(c)<='z')||((c)>='A'&&(c)<='Z'))

// Forward references
int type ( int key ) ;
void compile ( int key ) ;
int chkmaj () ;
void plink ( char *line, int which ) ;
int getline ( char line[], int msize ) ;
void exec ( int cmd ) ;

struct symstr
{
    char 		    *s_nam ;
    struct symstr	*s_nxt ;
    short int   	 s_val ;
    short int   	 s_aux ;
    short int	     s_mod ;
} ;


// Global variables

static char tbuf[MAXTBUF] ;
static short int tbp = 0 ;

static short int cbuf[MAXCBUF] ;
static short int cbp = 0 ;

static const char *ops [MAXOPS] =
{
    "ADD",		"AND",		"ANYOF",	"APPORT",	
    "AT",		"BIC",		"BIS",		"BIT",	
    "CALL",		"CHANCE",	"DEFAULT",	"DEPOSIT",	
    "DIVIDE",	"DROP",		"ELSE",		"EOF",	
    "EOI",		"EOR",		"EVAL",		"EXEC",	
    "FIN",		"GET",		"GOTO",		"HAVE",	
    "IFAT",		"IFEQ",		"IFGE",		"IFGT",	
    "IFHAVE",	"IFKEY",	"IFLE",		"IFLOC",	
    "IFLT",		"IFNEAR",	"INPUT",	"ITLIST",	
    "ITOBJ",	"ITPLACE",	"KEYWORD",	"LDA",	
    "LOCATE",	"MOVE",		"MULT",		"NAME",	
    "NEAR",		"NOT",		"OR",		"PROCEED",	
    "QUERY",	"QUIT",		"RANDOM",	"SAY",	
    "SET",		"SMOVE",	"STOP",		"SUB",	
    "SVAR",		"VALUE"
} ;

static struct symstr mem[MEMSIZ] ;
static int pmem = MEMSIZ ;

static struct symstr *symtab [TABSIZ] =
{
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
} ;

char clss [] =
{
    INITIAL,
    LABEL,
    VERB,
    VERB,
    PLACE,
    PLACE,
    PLACE,
    TEXT,
    OBJECT,
    OBJECT,
    OBJECT,
    VARIABLE,
    NULLCLASS,
    0
} ;

short int ninit	 = 0 ;
short int nrep	 = 0 ;
short int nvars	 = 0 ;
short int nobj	 = 0 ;
short int nplace = 0 ;

char state [STATES] =
{
    RELEASE,
    RELEASE,
    KEEP,
    KEEP,
    RELEASE,
    RELEASE,
    RELEASE,
    RELEASE,
    KEEP,
    KEEP,
    KEEP,
    RELEASE,
    KEEP,
    RELEASE,
    RELEASE,
    RELEASE,
    RELEASE,
    RELEASE,
    RELEASE,
    RELEASE,
    RELEASE,
    RELEASE,
    RELEASE,
    RELEASE,
    RELEASE
} ;

short int tclass [MAXOPS] =
{
    2, 0, -1,  2, -1, 2, 2, 2,  1, 1,
    1, 2,  2,  1,  0, 0, 0, 0,  2, 2,
    0, 1,  1, -1,  1, 2, 2, 2,  1, 1,
    2, 2,  2,  1,  0, 1, 1, 1, -1, 2,
    2, 2,  2,  2, -1, 0, 0, 0,  1, 0,
    2, 1,  2,  3,  0, 2, 2, 2
} ;

char token [MAXLINE] ;
int dbunit = -1 ;

static short int nxtint =     0 ;
static short int nxtrep =   500 ;
static short int nxtlab =  1000 ;
static short int nxtvrb =  2000 ;
static short int nxtplc =  4000 ;
static short int nxttxt =  7000 ;
static short int nxtobj =  8000 ;
static short int nxtvar = 11000 ;

struct cm
{
    const char *c_nam ;
    char c_num ;
} ;

struct cm commnd [MAXCOM] =
{
    "ACTION",	ACTION,
    "AT",		AT,
    "DEFINE",	DEFINE,
    "INCLUDE",	INCLUDE,
    "INITIAL",	INITIAL,
    "LABEL",	LABEL,
    "LIST",		LIST,
    "NOLIST",	NOLIST,
    "NULL",		NULLCLASS,
    "OBJECT",	OBJECT,
    "PLACE",	PLACE,
    "REPEAT",	REPEAT,
    "SYNONYM",	SYNON,
    "TEXT",		TEXT,
    "VARIABLE",	VARIABLE,
    "VERB",		VERB
} ;

////////////////////////////////////////////////////////////////


template<typename... Args>
    void synerr( const char *rnam, const char *fmt, Args... args )
{
    if ( inunit >= 0 )
        fprintf (stderr,"%s(%d): ",fx[inunit].fnam,fx[inunit].cnt) ;
    else
        fprintf (stderr,"(EOF) %s: ",rnam) ;

    fprintf ( stderr, fmt, args... ) ;
    fprintf ( stderr, "\n" ) ;
    flushline () ;
    return ;
}


int ctoi ( char *s, int *v )
{
    int val ;
    int sign ;

    sign = 1 ;
    if ( *s == PLUS )
        s++ ;
    else
        if ( *s == MINUS )
        {
            s++ ;
            sign = -1 ;
        }
    for ( *v = val = 0 ; *s != EOS ; s++ )
    {
        if ( isdigit(*s) )
            val = val * 10 + ( *s - '0' ) ;
        else
            return (ERROR) ;
    }
    *v = sign * val ;
    return (OK) ;
}


int gettok ( char tok[], int msize )
{
    int c, i ;

    tok[0] = EOS ;
    if ( ( c = skip () ) == NEWLINE || c == EOFFLAG )
        return (c) ;

    for ( i = 0 ; i < msize ; i++ )
    {
        tok[i] = c ;
        if ( c == NEWLINE || c == EOFFLAG || sep(c) )
        {
            Ungetc (c) ;
            tok[i] = EOS ;
            return (OK) ;
        }
        c = Getc () ;
    }

    Ungetc (BLANK) ;
    tok[i] = EOS ;
    synerr ("Gettok","token is too long (%d)!",msize) ;
    return (OK) ;
}


void clrcode ()
{
    cbp = 0 ;
    return ;
}


void appcode ( short code)
{
    if ( cbp >= MAXCBUF )
        error ("Appcode","code buffer overflow (%d)!",MAXCBUF) ;
    cbuf[cbp++] = code ;
    return ;
}


void outcode ( int key )
{
    if ( cbp < 1 )
        error ("Outcode","null code buffer!") ;
    if ( writek(dbunit,key,(char *)cbuf,cbp*sizeof(short int)) == ERROR )
        error ("Outcode","write error on key %d",key) ;
    return ;
}


void clrtext ()
{
    tbp = 0 ;
    return ;
}


void apptext ( char line[] )
{
    int i ;

    for ( i = 0 ; tbp < MAXTBUF ; i++ )
    {
        if ( line[i] == EOS )
            break ;
        tbuf[tbp++] = line[i] ;
    }
    if ( tbp >= MAXTBUF )
        error ("Apptext","text buffer overflow (%d)!",MAXTBUF) ;

    return ;
}


void outtext ( int key )
{
    if ( tbp < 1 )
        return ;
    if ( writek(dbunit,key,tbuf,tbp) == ERROR )
        error ("Outtext","write error on key %d",key) ;
    return ;
}


int findop ( char *tok, int *opc, int *cls)
{
    int i, high, low ;
    int m ;

    *opc = -1 ;
    *cls = 0 ;
    low = 0 ;
    high = MAXOPS-1 ;

    for ( i = (high+low)/2 ; low <= high ; i = (high+low)/2 )
    {
        if ( ( m = strncmp (tok,ops[i],MATCHOPS) ) == 0 )
        {
            *opc = i ;
            *cls = tclass [i] ;
            return (OK) ;
        }

        if ( m > 0 )
            low = i+1 ;
        else
            high = i-1 ;
    }
    return (ERROR) ;
}


int hash ( const char *s )
{
    int h ;

    for ( h = 0 ; *s != EOS ; )
        h += *s++ ;
    
    return ( ( h % TABSIZ ) ) ;
}


struct symstr *lookup ( const char *name )
{
    struct symstr *p ;

    for ( p = symtab[hash(name)] ; p != NIL ; p = p->s_nxt )
    {
        if ( strcmp (name,p->s_nam) == 0 ) {
#ifdef DEBUG
            printf("Debug: \"%s\" looked up.\n",name);
#endif
            return (p) ;
            }
    }
#ifdef DEBUG
    printf("Debug: \"%s\" not found.\n",name);
#endif
    return (NIL) ;
}


int seval ( char *s, int *val, int aflag )
{
    struct symstr *p ;
    int value ;

    if ( ( p = lookup (s) ) != NIL )
    {
        if ( aflag )
            *val = p->s_aux ;
        else
            *val = p->s_val ;
        return (OK) ;
    }

    if ( ctoi (s,&value) == OK )
    {
        *val = value ;
        return (OK) ;
    }

    return (ERROR) ;
}


int eval ( char *s )
{
    int val ;
    char *t ;
    int aflag, sign ;
    char *os, nc ;
    int value ;

    if ( isalnum(*s) && seval (s,&value,0) == OK )
        return (value) ;

    aflag = 0 ;
    sign = 0 ;
    os = s ;

    for ( val = 0 ; *s != EOS ; val += value )
    {
        switch (*s)
        {
            case ATSIGN:
                aflag ++ ;
                s++ ;
                break ;
            case PLUS:
                sign = 1 ;
                s++ ;
                break ;
            case MINUS:
                sign = -1 ;
                s++ ;
                break ;
        }

        for ( t = s ; *s != EOS && *s != PLUS && *s != MINUS ; s++ )
            ;

        nc = *s ;
        *s = EOS ;

        if ( seval (t,&value,aflag) != OK )
        {
            Ungetc (NEWLINE) ;
            synerr ("Eval","Unrecognized expression `%s'.",os) ;
            return (ERROR) ;
        }

        *s = nc ;
        if ( sign < 0 )
            value = -value ;
    }
    return (val) ;
}


struct symstr *define ( const char *nam, int val, int mod)
{
    struct symstr *p ;
    int h ;

    if ( lookup(nam) != NIL )
        return (NIL) ;
#ifdef DEBUG
    printf("Debug: Defining symbol \"%s\"\n",nam);
#endif
    
    if ( pmem <= 0 )
        error ("Symbol define","symbol table full on `%s'\n",nam) ;
    p = &(mem[--pmem]) ;

    p->s_nam = strsav (nam) ;
    p->s_val = val ;
    p->s_aux = 0 ;
    p->s_mod = mod ;

    h = hash (nam) ;
    p->s_nxt = symtab[h] ;
    symtab[h] = p ;

    return (p) ;
}


void act ()
{
    struct symstr *p ;
    int key ;
    int interval, limit ;

    if ( gettok (token,MAXLINE) != OK )
    {
        synerr ("Action","Missing Verb name.") ;
        return ;
    }

    if ( ( p = lookup (token) ) == NIL )
    {
        synerr ("Action","%s -- Undefined Verb.",token) ;
        return ;
    }

    if ( type (p->s_val) == OBJECT )
    {
        if ( p->s_aux < MAXOTEXT )
            p->s_aux = MAXOTEXT ;
        interval = MAXOBJECTS ;
        limit = MAXOTEXT + MAXOCODE ;
    }
    else
    {
        interval = MAXVERBS ;
        limit = MAXVCODE ;
    }

    key = (p->s_val) + ( (p->s_aux) * interval ) ;

    if ( type (key) != VERB && type (key) != OBJECT )
    {
        synerr ("Action","%s -- that's no action!",token) ;
        return ;
    }

    if ( (++(p->s_aux)) >= limit )
        error ("Action","too many code definitions for %s (%d)!",
            token,MAXOCODE) ;

    if ( gettok (token,MAXLINE) == OK )
    {
        pbstr (token) ;
        pbstr (" KEYWORD ") ;
    }

    compile (key) ;
    return ;
}

void at ()
{
    struct symstr *p ;
    int key ;

    if ( gettok (token,MAXLINE) != OK )
    {
        synerr ("At","Missing Place name.") ;
        return ;
    }

    if ( ( p = lookup (token) ) == NIL )
    {
        synerr ("At","%s -- Undefined Place.",token) ;
        return ;
    }

    key = (p->s_val) + ((p->s_aux) * MAXPLACES) ;

    if ( (++(p->s_aux)) >= MAXPCODE )
        error ("At","too many code definitions for %s (%d)!",
            token,MAXPCODE) ;

    if ( type (key) != PLACE )
    {
        synerr ("At","%s -- that's no Place!",token) ;
        return ;
    }

    flushline () ;
    compile (key) ;
    return ;
}

void def ()
{
    struct symstr *p ;

    while ( gettok (token,MAXLINE) == OK )
    {
        if ( ( p = lookup (token) ) == NIL )
        {
            synerr ("Define","%s -- Undefined symbol.",token) ;
            return ;
        }
        p->s_mod = KEEP ;
    }
    return ;
}

void inc ()
{
    char *s ;

    if ( gettok (token,MAXLINE) != OK )
    {
        synerr ("Include","Missing file name.") ;
        return ;
    }

    if ( push (inunit) == ERROR )
    {
        synerr ("Include","include's nested too deeply!") ;
        return ;
    }

    if ( ( inunit = openf (token) ) == ERROR )
    {
        synerr ("Include","%s -- unable to open file.",token) ;
        inunit = pop () ;
        return ;
    }

    if ( ( s = strrchr (token,FILSEP) ) == (char *)0 )
        errout ("%s:\n",token) ;
    else
        errout ("%s:\n",&(s[1])) ;

    flushline () ;
    return ;
}

void init ()
{
    flushline () ;
    compile (nxtint) ;
    nxtint ++ ;
    ninit ++ ;
    return ;
}

void lab ()
{
    if ( gettok (token,MAXLINE) != OK )
    {
        synerr ("Label","Missing Label name.") ;
        return ;
    }

    if ( define (token,nxtlab,status(nxtlab)) == NIL )
    {
        synerr ("Label","%s -- duplicate label name.",token) ;
        return ;
    }

    flushline () ;
    compile (nxtlab) ;
    nxtlab++ ;
    return ;
}


void null ()
{
    while ( gettok (token,MAXLINE) == OK )
    {
        if ( define (token,NULLWORD,KEEP) == NIL )
        {
            synerr ("Null","%s -- duplicate Null symbol",token) ;
            return ;
        }
    }
    return ;
}

void obj ()
{
    struct symstr *p ;
    int m, n ;

    if ( gettok (token,MAXLINE) != OK )
    {
        synerr ("Object","Missing Object name.") ;
        return ;
    }

    if ( ( p = define (token,nxtobj,status(nxtobj)) ) == NIL )
    {
        synerr ("Object","%s -- duplicate Obect name.",token) ;
        return ;
    }

    flushline () ;
    n = 0 ;
    m = getline (token,MAXLINE) ;
    if ( m == NEXT )
    {
        n++ ;
        m = getline (token,MAXLINE) ;
    }
    if ( m == OK && token[0] == NEWLINE )
    {
        while ( getline (token,MAXLINE) == OK )
            ;
        m = getline (token,MAXLINE) ;
        n++ ;
    }
    for ( ; m != MAJOR && m != EOFFLAG ; m = getline (token,MAXLINE) )
    {
        clrtext () ;
        do
        {
            if ( strncmp (token,NULLOBJECT,3) == 0 )
            {
                clrtext () ;
                break ;
            }
            apptext (token) ;
        }
        while ( getline (token,MAXLINE) == OK ) ;

        outtext (nxtobj+((n++)*MAXOBJECTS)) ;
    }

    if ( n > 0 )
        n-- ;

    p->s_aux = n ;

    nxtobj++ ;
    nobj ++ ;

    return ;
}

void place ()
{
    struct symstr *p ;

    if ( gettok (token,MAXLINE) != OK )
    {
        synerr ("Place","Missing Place name.") ;
        return ;
    }

    if ( ( p = define (token,nxtplc,status(nxtplc)) ) == NIL )
    {
        synerr ("Place","%s -- duplicate Place name.",token) ;
        return ;
    }
    flushline () ;

    if ( getline (token,MAXLINE) == OK )	/* short description */
    {
        if ( strncmp (token,LINKPLACE,3) == 0 )
        {
            plink (token,SHORTDESC) ;
            while ( getline (token,MAXLINE) == OK )
                ;
        }
        else
        {
            clrtext () ;
            do
            {
                apptext (token) ;
            }
            while ( getline (token,MAXLINE) == OK ) ;
            outtext (nxtplc) ;
        }
    }

    if ( getline (token,MAXLINE) == OK )	/* long description */
    {
        if ( strncmp (token,LINKPLACE,3) == 0 )
            plink (token,LONGDESC) ;
        else
        {
            clrtext () ;
            do
            {
                apptext (token) ;
            }
            while ( getline (token,MAXLINE) == OK ) ;
            outtext (nxtplc+MAXPLACES) ;
        }
    }

    p->s_aux = 2 ;
    nxtplc ++ ;
    nplace ++ ;

    return ;
}


void rep ()
{
    flushline () ;
    compile (nxtrep) ;
    nxtrep ++ ;
    nrep ++ ;

    return ;
}

void syn ()
{
    int val ;

    if ( gettok (token,MAXLINE) != OK )
    {
        synerr ("Synonym","Missing symbol.") ;
        return ;
    }

    val = eval (token) ;

    while ( gettok (token,MAXLINE) == OK )
    {
        if ( define (token,val,status(val)) == NIL )
        {
            synerr ("Synonym","%s -- duplicate name.",token) ;
            return ;
        }
    }
    return ;
}

void mtext ()
{

#ifdef DEBUG
    printf("Debug: Enterring text\n");
#endif
    if ( gettok (token,MAXLINE) == OK )
    {
#ifdef DEBUG
        printf("Debug: text token is \"%s\"\n",token);
#endif
        if ( define (token,nxttxt,status(nxttxt)) == NIL )
        {
            synerr ("Text","%s -- duplicate Text name.") ;
            return ;
        }
        flushline () ;
    }

#ifdef DEBUG
    printf("Debug: No token found after TEXT\n");
#endif
    clrtext () ;

    while ( getline (token,MAXLINE) == OK )
        apptext (token) ;

    outtext (nxttxt) ;

    nxttxt ++ ;

    return ;
}

void var ()
{
    while ( gettok (token,MAXLINE) == OK )
    {
        if ( define (token,nxtvar,status(nxtvar)) == NIL )
        {
            synerr ("Variable","%s -- duplicate symbol.",token) ;
            return ;
        }
        nxtvar++ ;
        nvars ++ ;
    }
    return ;
}

void verb ()
{
    if ( gettok (token,MAXLINE) != OK )
    {
        synerr ("Verb","Missing symbol.") ;
        return ;
    }

    if ( define (token,nxtvrb,status(nxtvrb)) == NIL )
    {
        synerr ("Verb","%s -- duplicate symbol.",token) ;
        return ;
    }

    while ( gettok (token,MAXLINE) == OK )
    {
        if ( define (token,nxtvrb,status(nxtvrb)) == NIL )
        {
            synerr ("Verb","%s -- duplicate synonym.",token) ;
            return ;
        }
    }
    nxtvrb++ ;
    return ;
}


int makedb ( const char *dbname )
{
    int unit ;

    unit = creatk (dbname) ;
    if ( unit == ERROR )
        error ("Makedb","unable to create database %s",dbname) ;
    return (unit) ;
}


void iodone ()
{
    closek (dbunit) ;
    return ;
}


int major ( char *tok )
{
    int i ;
    struct cm c ;

    for ( i = 0 ; i < MAXCOM ; i++ )
    {
        if ( strncmp (commnd[i].c_nam,tok,MATCHCOM) == 0 )
        {
            if ( i )
            {
                c = commnd[i] ;
                commnd[i] = commnd[0] ;
                commnd[0] = c ;
            }
            return (commnd[0].c_num) ;
        }
    }
    return (ERROR) ;
}


void parse ()
{
    int m ;

    m = getline (token,MAXLINE) ;
    for ( ; m != EOFFLAG ; m = getline (token,MAXLINE) )
    {
        if ( m == MAJOR && gettok (token,MAXLINE) == OK )
            exec (major(token)) ;
    }
    return ;
}

int getline ( char line[], int msize )
{
    int c, i ;

    line[0] = EOS ;

    if ( chkmaj () == MAJOR )
        return (MAJOR) ;

    c = skip () ;
    if ( c == PERCENT )
    {
        Ungetc (BLANK) ;
        return (NEXT) ;
    }

    if ( c == SLASH )
        c = Getc () ;
    
    for ( i = 0 ; i < msize ; i++ )
    {
        line[i] = c ;

        if ( c == NEWLINE )
        {
            line[i+1] = EOS ;
            return (OK) ;
        }
        if ( c == EOFFLAG )
            return (EOFFLAG) ;
        c = Getc () ;
    }
    Ungetc (BLANK) ;
    line[i] = EOS ;
    synerr ("Getline","line is too long (%d)!",msize) ;

    return (OK) ;
}


void exec ( int cmd )
{

    switch (cmd)
    {
        case ACTION:
            act () ;
            break ;
        case AT:
            at () ;
            break ;
        case DEFINE:
            def () ;
            break ;
        case INCLUDE:
            inc () ;
            break ;
        case INITIAL:
            init () ;
            break ;
        case LABEL:
            lab () ;
            break ;
        case LIST:
            list = YES ;
            break ;
        case NOLIST:
            list = NO ;
            break ;
        case NULLCLASS:
            null () ;
            break ;
        case OBJECT:
            obj () ;
            break ;
        case PLACE:
            place () ;
            break ;
        case REPEAT:
            rep () ;
            break ;
        case SYNON:
            syn () ;
            break ;
        case TEXT:
#ifdef DEBUG
            printf("Debug: Calling text()");
#endif
            mtext () ;
            break ;
        case VARIABLE:
            var () ;
            break ;
        case VERB:
            verb () ;
            break ;
        default:
            error ("Exec","%d -- Unrecognised Command.",cmd) ;
            break ;
    }
    return ;
}


void plink ( char *line, int which )
{
    char tok1[32] ;
    char tok2[32] ;
    struct symstr *p ;
    int oldkey, newkey ;

    pbstr (line) ;

    if ( gettok (tok1,32) != OK || gettok (tok1,32) != OK )
    {
        synerr ("Plink","invalid link(%s)!",LINKPLACE) ;
        return ;
    }

    if ( tok1[0] == 'd' || tok1[0] == 'D' )
    {
        if ( which == SHORTDESC )
        {
          synerr ("Plink","impossible to `ditto' short description!") ;
          return ;
        }
        
        if ( dupk (dbunit,nxtplc,(nxtplc+MAXPLACES)) == ERROR )
        {
            synerr ("Plink","unable to link to %d!",nxtplc) ;
            return ;
        }
        flushline () ;
        return ;
    }

    if ( gettok (tok2,32) != OK )
    {
        synerr ("Plink","missing place name!") ;
        return ;
    }
    flushline () ;

    if ( ( p = lookup (tok2) ) == NIL )
        synerr ("Plink","%s -- undefined place name!",tok2) ;
    
    if ( tok1[0] == 's' || tok1[0] == 'S' )
        oldkey = p->s_val ;
    else
        oldkey = ( (p->s_val) + MAXPLACES ) ;
    
    if ( which == SHORTDESC )
        newkey = nxtplc ;
    else
        newkey = nxtplc + MAXPLACES ;

    if ( dupk (dbunit,oldkey,newkey) == ERROR )
    {
        synerr ("Plink","unable to link %d to %d!",oldkey,newkey) ;
        return ;
    }

    return ;
}


int chkmaj ()
{
    int c ;

    for ( c = Getc() ; com(c) ; c = Getc() )	/* flush comments */
        while ( (c=Getc()) != NEWLINE )
            if ( c == EOFFLAG )
                return (OK) ;
    Ungetc (c) ;
    if ( isalpha(c) )
        return (MAJOR) ;
    return (OK) ;
}


#ifdef PSTAB

void pstab ()
{
    int i ;
    struct symstr *p ;
    int noverb = 0 ;
    int nov = 0 ;
    int noplac = 0 ;
    int nop = 0 ;
    int noobjs = 0 ;
    int noo = 0 ;

    for ( i = 0 ; i < TABSIZ ; i++ )
    {
        for ( p = symtab[i] ; p != NIL ; p = p->s_nxt )
        {
            switch (type(p->s_val))
            {
              case VERB:
                  printf ("%-12.12s",p->s_nam) ;
                  printf ("\t%5d",p->s_val) ;
                  printf ("\t(verb %d)\n",p->s_aux) ;
                  noverb += p->s_aux ;
                  nov++ ;
                  break ;
            
              case PLACE:
                  printf ("%-12.12s",p->s_nam) ;
                  printf ("\t%5d",p->s_val) ;
                  if ( p->s_aux > 0 )
                  {
                      printf ("\t(place %d)\n",(p->s_aux)-1) ;
                      noplac += (p->s_aux)-1 ;
                  }
                  else
                      printf ("\t(place %d)\n",(p->s_aux)) ;
                  nop++ ;
                  break ;

              case OBJECT:
                  printf ("%-12.12s",p->s_nam) ;
                  printf ("\t%5d",p->s_val) ;
                  if ( p->s_aux >= MAXOTEXT )
                      printf ("\t(code %d)\n",(p->s_aux)-MAXOTEXT) ;
                  else
                  {
                      printf ("\t(text %d)\n",p->s_aux) ;
                    noobjs += p->s_aux ;
                  }
                  noo++ ;
                  break ;
            
              default:
                  if ( p->s_mod == KEEP )
                  {
                      printf ("%-12.12s",p->s_nam) ;
                      printf ("\t%5d\n",p->s_val) ;
                  }
                  break ;
            }
        }
    }

    printf ("\n\n") ;

    if ( nov )
    {
        printf ("Number of verbs           -- %d\n",nov) ;
        printf ("Total number of code keys -- %d\n",noverb) ;
        printf ("Ave. number of code keys  -- %f\n",(float)noverb/nov) ;
        printf ("\n") ;
    }

    if ( nop )
    {
        printf ("Number of places          -- %d\n",nop) ;
        printf ("Total number of code keys -- %d\n",noplac) ;
        printf ("Ave. number of code keys  -- %f\n",(float)noplac/nop) ;
        printf ("\n") ;
    }

    if ( noo )
    {
        printf ("Number of objects         -- %d\n",noo) ;
        printf ("Total number of text keys -- %d\n",noobjs) ;
        printf ("Ave. number of text keys  -- %f\n",(float)noobjs/noo) ;
        printf ("\n") ;
    }
    return ;
}

#endif // PSTAB


void wstab ()
{
    int i ;
    struct symstr *p ;
    char *s ;
    int v, cnt, nxtsym ;

    (void) define ("<NREP>",nrep,KEEP) ;
    (void) define ("<NINIT>",ninit,KEEP) ;
    (void) define ("<NVARS>",nvars,KEEP) ;
    (void) define ("<NOBJ>",nobj,KEEP) ;
    (void) define ("<NPLACE>",nplace,KEEP) ;

    cnt = 0 ;
    clrcode () ;
    nxtsym = SYMTABREC +1 ;

    for ( i = 0 ; i < TABSIZ ; i++ )
    {
        for ( p = symtab[i] ; p != NIL ; p = p->s_nxt )
        {
            if ( p->s_mod == KEEP )
            {
                cnt++ ;
                v = p->s_val ;
                for ( s = p->s_nam ; *s ; )
                    appcode (*s++) ;
                appcode (EOS) ;
                appcode (v) ;
                if ( ( cnt % SYMPERREC ) == 0 )
                {
                    outcode (nxtsym) ;
                    nxtsym++ ;
                    clrcode () ;
                }
            }
        }
    }

    if ( ( cnt % SYMPERREC ) != 0 )
        outcode (nxtsym) ;
    
    clrcode () ;
    appcode (cnt) ;
    outcode (SYMTABREC) ;
    return ;
}

int type ( int key )
{
    if ( key < 0 || key > MAXKEY )
        return (0) ;
    return (clss[key/1000]) ;
}


void compile ( int key )
{
    int where, i ;
    int opcode, opnum ;

    clrcode () ;
    while ( chkmaj () == OK )
    {
        if ( gettok (token,MAXLINE) != OK )
            break ;
        if ( findop (token,&opcode,&opnum) == ERROR )
        {
            synerr ("Compile","%s -- unrecognized opcode.",token) ;
            return ;
        }
        if ( opnum < 0 )	/* infinite operand opcode */
        {
            while ( gettok (token,MAXLINE) == OK )
            {
                where = eval(token) ;
                appcode (opcode) ;
                appcode (where) ;
            }
        }
        else	/* zero, one, two, or three operand opcode */
        {
            appcode (opcode) ;
            for ( i = 1 ; i <= opnum ; i++ )
            {
                if ( gettok (token,MAXLINE) != OK )
                {
                    synerr("Compile","missing operand(s)") ;
                    return ;
                }
                where = eval(token) ;
                appcode (where) ;
            }
            if ( gettok (token,MAXLINE) != NEWLINE )
            {
                synerr ("Compile","%s ignored.",token) ;
                return ;
            }
        }
    }
    outcode (key) ;
    return ;
}


int main ( int argc, char *argv[] )
{
    if ( argc != 2 )
    {
        printf ("Usage: %s [commandfile]\n",argv[0]) ;
        exit (1) ;
    }
    if ( argc == 2 )
    {
        if ( ( inunit = openf (argv[1]) ) < 0 )
        {
            printf ("Cannot open: %s\n",argv[1]) ;
            exit (1) ;
        }
    }
    dbunit = makedb ("adv") ;
    parse () ;
    wstab () ;
    iodone () ;
    exit (0) ;
}
