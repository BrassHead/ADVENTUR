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

#include <time.h>

#include "kio.hpp"
using kio::openk ;
using kio::closek ;
using kio::readk ;
using kio::error ;

#include "fio.hpp"


#if defined(_MSDOS) || defined(_WIN32)

    #define random() rand()
    #define srandom(x) srand(x)

    #define getpid _getpid
    int getuid() { return 1;}

#endif

#define ADV			"adv"
#define FREEZER		"freezer"
#define	INITIAL		0
#define	REPEAT		500
#define	OBJECTS		120
#define	PLACES		307
#define	VARS		50
#define	SYMTABREC	13000
#define MAXKEY		14000
#define	MAXOPS		58
#define	BUFSIZE		500
#define	TBUFSIZE	3000
#define CACHESIZE	100
#define MAXCACHE	4096
#define	TABSIZ		128
#define	LEXSIZ		20
#define	MAXLINE		134
#define LINESIZE	134

#define	YES		    1
#define	NO		    0
#define	EOS		    0
#define NIL		    ((struct symstr *)0)
#define	OK		    1
#define MATCH	    6

#define ADD		     0
#define AND		     1
#define ANYOF	     2
#define APPORT	     3
#define AT		     4
#define BIC		     5
#define BIS		     6
#define BIT		     7
#define CALL	     8
#define CHANCE	     9
#define DEFAULT	    10
#define DEPOSIT	    11
#define DIVIDE	    12
#define DROP	    13
#define ELSE	    14
#define EOFTOK	    15
#define EOI		    16
#define EOR		    17
#define EVAL	    18
#define EXEC	    19
#define FIN		    20
#define GET		    21
#define GOTO	    22
#define HAVE	    23
#define IFAT	    24
#define IFEQ	    25
#define IFGE	    26
#define IFGT	    27
#define IFHAVE	    28
#define IFKEY	    29
#define IFLE	    30
#define IFLOC	    31
#define IFLT	    32
#define IFNEAR	    33
#define INPUT	    34
#define ITLIST	    35
#define ITOBJ	    36
#define ITPLACE	    37
#define KEYWORD	    38
#define LDA		    39
#define LOCATE	    40
#define MOVE	    41
#define MULT	    42
#define NAME	    43
#define NEAR	    44
#define NOT		    45
#define OR		    46
#define PROCEED	    47
#define QUERY	    48
#define QUIT	    49
#define RANDOM	    50
#define SAY		    51
#define SET		    52
#define SMOVE	    53
#define STOP	    54
#define SUB		    55
#define SVAR	    56
#define VALUE	    57

#define	LINELEN		2
#define LEXLEN		5
#define	MAXCALLS	10
#define	MAXDOS		2

#define OBJKEY		8000
#define MAXOBJECTS	120
#define MAXOTEXT	20
#define MAXOCODE	4
#define PLACEKEY	4000
#define MAXPLACES	350
#define MAXPCODE	8
#define VERBKEY		2000
#define MAXVERBS	160
#define MAXVCODE	12

#define	MOVED		001
#define	BRIEF		002
#define	FAST		004
#define	LOOKING		010
#define	BEEN		002
#define	DUAL		010

#define	XOBJECT		0100000
#define	XVERB		0040000
#define	XPLACE		0020000
#define	BADWORD		0010000

#define	INIT		0
#define	LABEL		1
#define	VERB		2
#define	PLACE		3
#define	TEXT		4
#define	OBJECT		5
#define	VARIABLE	6
#define	NULLCLASS   7

#define	INHAND		(-1)
#define	REPLACE		'#'
#define	NULLTXT		7000
#define	BLANKTXT	7001
#define	OKTXT		7002

#define SMEMSIZ		450

#define forever		for(;;)

#define class(key)	(clss[(key)/1000])
#define indx(key)	((key)%1000)
#define logical(instr)	(ltab[(instr)])
#define opnum(opcode)	(opn[(opcode)])
#define where(key)	(objloc[indx(key)])

#define CACHE

struct saveb
{
    int s_uid ;
    short int s_ov[OBJECTS], s_ob[OBJECTS], s_ol[OBJECTS] ;
    short int s_pb[PLACES] ;
    short int s_vv[VARS], s_vb[VARS] ;
} ;
struct saveb sb ;

struct symstr
{
    char		    *s_nam ;
    struct symstr	*s_nxt ;
    short int	    s_val ;
} ;

static int powof2[16] =
{
    000001, 000002, 000004, 000010,
    000020, 000040, 000100, 000200,
    000400, 001000, 002000, 004000,
    010000, 020000, 040000, 0100000
} ;

char opn [MAXOPS] =
{
    2, 0, 1, 2, 1, 2, 2, 2, 1, 1,
    1, 2, 2, 1, 0, 0, 0, 0, 2, 2,
    0, 1, 1, 1, 1, 2, 2, 2, 1, 1,
    2, 2, 2, 1, 0, 1, 1, 1, 1, 2,
    2, 2, 2, 2, 1, 0, 0, 0, 1, 0,
    2, 1, 2, 3, 0, 2, 2, 2	
} ;

char ltab[MAXOPS] =
{
    0,0,0,0,0,0,0,1,0,1,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,1,1,1,1,
    1,1,1,1,1,1,0,0,0,0,0,0,0,0,
    0,0,0,1,0,0,1,0,0,0,0,0,0,0,
    0,0
} ;

struct symstr *symtab [TABSIZ] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
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

#ifdef SHOWOP
    int CurKey = -1 ;
#endif // SHOWOP

char token [MAXLINE] ;

int linlen = 0 ;
int linewd [LINELEN] ;

char lex[LEXLEN][LEXSIZ] ;

short int objval[OBJECTS] ;
short int objbit[OBJECTS] ;
short int objloc[OBJECTS] ;

short int plcbit[PLACES] ;

short int varval[VARS] ;
short int varbit[VARS] ;

short int codebuf[BUFSIZE] ;
char textbuf[TBUFSIZE] ;

int nrep, ninit, nvars, nobj, nplace ;
int here, there, status, argwd[LINELEN] ;

int dbunit = -1 ;

static struct symstr smem[SMEMSIZ] ;
static int psmem = SMEMSIZ ;

static char cmem[CMEMSIZ] ;
static int  pcmem = 0 ;


#ifdef CACHE

    struct ctabstr
    {
        short int  c_key ;
        short int  c_siz ;
        short int *c_cod ;
    } ;
    
    static int full = NO ;
    static struct ctabstr ctab[CACHESIZE] ;
    static short int cache[MAXCACHE] ;
    static int cachesize = 0 ;

#endif // CACHE

// extern char ltab[] ;

// extern char token [] ;

// extern int linlen ;
// extern int linewd [] ;

// extern char lex[LEXLEN][LEXSIZ] ;

// extern short int objval[], objbit[], objloc[] ;

// extern short int plcbit[] ;

// extern short int varval[], varbit[] ;

// extern short int codebuf[] ;
// extern char textbuf[] ;

// extern int nrep, ninit, nvars, nobj, nplace ;
// extern int here, there, status, argwd[] ;

// extern int dbunit ;
// extern char clss[] ;


////////////////////////////////////////////////////////////////////////


int prime ()
{
    return (0) ;
}


int rnd ( int n )
{
    return ((random())%n) ;
}


int ref ( int key )
{
    if ( class(key) == VARIABLE )
        return (varval[indx(key)]) ;
    return (key) ;
}


int findnam ( int val, char *s )
{
    struct symstr *p ;
    int i ;

    for ( i = 0 ; i < TABSIZ; i++ )
        for ( p = symtab[i] ; p != NIL ; p = p->s_nxt )
        {
            if ( p->s_val == val )
            {
                (void) strcpy (s,p->s_nam) ;
                return (val) ;
            }
        }
    return (ERROR) ;
}


void up2low ( char *word )
{
    for ( ; *word ; word++ )
        if ( (*word) >= 'A' && (*word) <= 'Z' )
            *word += ( 'a' - 'A' ) ;
    return ;
}


int hash ( const char *s )
{
    int h, i ;

    for ( i = h = 0 ; *s != EOS && i < MATCH ; i++ )
        h += *s++ ;
    
    return ( ( h % TABSIZ ) ) ;
}


int find ( const char *name )
{
    struct symstr *p ;

    for ( p = symtab[hash(name)] ; p != NIL ; p = p->s_nxt )
    {
        if ( strncmp (name,p->s_nam,MATCH) == 0 )
        {
            return (p->s_val) ;
        }
    }
    return (ERROR) ;
}


char *strsav ( char *s )
{
    char *p ;
    int n ;

    n = strlen(s) + 1 ;
    if ( (pcmem+n) >= CMEMSIZ )
        error ("Strsave","Out of symbol space on `%-10.10s'\n",s) ;

    p = &(cmem[pcmem]) ;
    pcmem += n ;
    (void) strcpy (p,s) ;
    return (p) ;
}


int bitval ( int key )
{
    int val ;

    switch (class(key))
    {
        case INIT:
            if ( key < 0 || key > 15 )
                error ("Bitval","bad key %d!",key) ;
            val = powof2[key] ;
            break ;

        case OBJECT:
            val = objbit[indx(key)] ;
            break ;

        case VARIABLE:
            val = varval[indx(key)] ; /* single indirection */
            switch (class(val))
            {
                case OBJECT:
                    val = objbit[indx(val)] ;
                    break ;

                case PLACE:
                    val = plcbit[indx(val)] ;
                    break ;
                
                default:
                    val = varbit[indx(key)] ;
                    break ;
            }
            break ;
        
        case PLACE:
            val = plcbit[indx(key)] ;
            break ;

        default:
            error ("Bitval","bad key %d!",key) ;
            break ;
    }
    return (val) ;
}


void setbit ( int key, int bits )
{
    int val ;

    switch (class(key))
    {
        case OBJECT:
            objbit[indx(key)] = bits ;
            break ;

        case VARIABLE:
            varbit[indx(key)] = bits ;
            val = varval[indx(key)] ;
            switch (class(val))
            {
                case OBJECT:
                    objbit[indx(val)] = bits ;
                    break ;

                case PLACE:
                    plcbit[indx(val)] = bits ;
                    break ;
            }
            break ;
        
        case PLACE:
            plcbit[indx(key)] = bits ;
            break ;

        default:
            error ("SetBit","bad key %d!",key) ;
            break ;
    }
    return ;
}


void setval ( int key, int val )
{
    switch (class(key))
    {
        case OBJECT:
            objval[indx(key)] = val ;
            break ;

        case VARIABLE:
            varval[indx(key)] = val ;
            break ;

        default:
            error ("Setval","bad key %d!",key) ;
            break ;
    }
    return ;
}


int eval ( int key )
{
    int val ;

    switch (class(key))
    {
        case INIT:
            val = key ;
            break ;

        case OBJECT:
            val = objval[indx(key)] ;
            break ;

        case VARIABLE:
            val = varval[indx(key)] ;
            break ;

        default:
            error ("Eval","bad key %d!",key) ;
            break ;
    }
    return (val) ;
}


int saykey ( int key )
{
    int rec, b ;

    switch (class(key))
    {
        case OBJECT:
            if ( where (key) != INHAND )
            {
                if ( eval (key) < 0 )
                    return (ERROR) ;
                rec = key + ( (eval(key) + 1) * MAXOBJECTS ) ;
            }
            else
                rec = key ;
            break ;

        case PLACE:
            b = bitval (status) ;
            if ( (b&LOOKING) )
                rec = key + MAXPLACES ;
            else
                if ( (b&BRIEF) && (bitval(key)&BEEN) )
                    rec = key ;
                else
                    if ( (b&FAST) )
                        rec = key ;
                    else
                        rec = key + MAXPLACES ;
            break ;

        case TEXT:
            rec = key ;
            break ;

        default:
            error ("Saykey","bad key %d!",key) ;
            break ;
    }
    return (rec) ;
}


#ifdef CACHE

void wcopy (short *a, short *b, int n)
{
    while ( n-- )
        *b++ = *a++ ;
    return ;
}


/*
**	ok2sav -- determine whether the code for the given key
**		  should be saved (if possible).
**		  Note: there is no need to save the INIT code,
**		  	but certainly saving the REPEAT code is
**			well worthwhile.
*/

static int ok2sav ( int key )
{
    int val ;

    if ( key >= REPEAT && key < 1000 )	/* save `REPEAT' code */
        return (YES) ;

    switch ( class (key) )
    {

        case VERB :
        case OBJECT :
        case LABEL :
        case PLACE :
                val = YES ;
                break ;
        default :
                val = NO ;
                break ;
    }
    return (val) ;
}


static int GetCache ( int key )
{
    int i, size ;

    for ( i = 0 ; i < CACHESIZE ; i++ )
    {
        if ( ctab[i].c_key == key )
            break ;
        if ( ctab[i].c_key == -1 )
            return (ERROR) ;
    }

    size = ctab[i].c_siz ;
    if ( size <= 0 )
        return (ERROR) ;
    wcopy (ctab[i].c_cod,codebuf,size) ;
    return (size) ;
}


void ClrCache ()
{
    int i ;

    full = NO ;
    cachesize = 0 ;
    for ( i = 0 ; i < CACHESIZE ; i++ )
        ctab[i].c_key = -1 ;

    return ;
}


static void PutCache ( int key, int size)
{
    int i ;

    if ( size <= 0 )
        return ;

    for ( i = 0 ; i < (CACHESIZE-1) ; i++ )
    {
        if ( ctab[i].c_key == key )
            return ;
        if ( ctab[i].c_key == -1 )
        {
            ctab[i+1].c_key = -1 ;
            break ;
        }
    }

    if ( i == (CACHESIZE-1) )	/* cache address table is full */
    {
        full = YES ;
        return ;
    }

    if ( (cachesize+size) >= MAXCACHE ) /* cache memory is (almost) full */
    {
        if ( (MAXCACHE-cachesize) < 50 ) /* less than 50 bytes left */
            full = YES ;
        return ;
    }

    ctab[i].c_key = key ;
    ctab[i].c_siz = size ;
    ctab[i].c_cod = &(cache[cachesize]) ;

    cachesize += size ;

    wcopy (codebuf,ctab[i].c_cod,size) ;

    return ;
}

#ifdef STATUS

int pcstat ()
{
    int i ;

    printf ("Cache Status: (%s) -- (%d) of (%d) available\n\n",
        (full?"full":"active"),(MAXCACHE-cachesize),MAXCACHE) ;
    
    for ( i = 0 ; i < (CACHESIZE-1) ; )
    {
        if ( ctab[i].c_key == -1 )
            break ;
        printf ("\t[%d] key(%d) size(%d)\n",
            i,ctab[i].c_key,ctab[i].c_siz) ;
    }

    printf ("\n") ;
    return ;
}

#endif // STATUS

#endif // CACHE


#ifdef SHOWOP

void showop ( int active, int op, short args[3])
{
    int n, i ;

    if ( active )
        printf ("%5d: ",CurKey) ;
    else
        printf ("<skip> ") ;

    if ( op < 0 || op >= 58 )
    {
        printf ("bad opcode (%d)\n",op) ;
        return ;
    }

    printf ("%-7.7s",ops[op]) ;

    n = opnum (op) ;

    for ( i = 0 ; i < n ; i++ )
        printf (" %5d",args[i]) ;
    
    printf ("\n") ;
    return ;
}

#endif // SHOWOP


void setup ()
{
    int i ;

    for ( i = 0 ; i < OBJECTS ; i++ )
    {
        objval[i] = 0 ;
        objbit[i] = XOBJECT ;
        objloc[i] = 0 ;
    }
    
    for ( i = 0 ; i < PLACES ; i++ )
    {
        plcbit[i] = XPLACE ;
    }

    for ( i = 0 ; i < VARS ; i++ )
    {
        varval[i] = 0 ;
        varbit[i] = XVERB ;
    }

#ifdef CACHE
    ClrCache () ;
#endif // CACHE

    return ;
}


void opendb( const char *name )
{
    dbunit = openk (name) ;

    if ( dbunit == ERROR )
        error ("Opendb","unable to open %s!",name) ;
    return ;
}


/*
**	rdcode -- fetch the code addressed by key.
**
**	Note: an open (chain) hashing strategy was tried
**	      on the cache table, with no significant speed
**	      increase! This seems puzzling, but since the
**	      hashing takes more memory, why bother with it?
**
*/

int rdcode ( int key )
{
    int endb  ;

#ifdef CACHE
    if ( ( endb = GetCache (key) ) > 0 )	/* already in memory? */
        return (endb) ;
#endif // CACHE

    if ( ( endb = readk ( dbunit, key, (char *)codebuf, BUFSIZE*2 ) ) < 1 )
        return (ERROR) ;

    endb = (endb+1)/2 ;

#ifdef CACHE
    if ( full )
        return (endb) ;

    if ( ok2sav (key) == NO )
        return (endb) ;

    PutCache (key,endb) ;		/* lets keep a copy around */
#endif // CACHE

    return (endb) ;
}

void low2up (char *word)
{
    for ( ; *word ; word++ )
        if ( (*word) >= 'a' && (*word) <= 'z' )
            *word += ( 'A' - 'a' ) ;
    return ;
}

int readln ( char words[LEXLEN][LEXSIZ] )
{
    int cnt, i ;
    char *b ;
    char buffer[LINESIZE] ;

    printf ("? ") ;

    if ( fgets (buffer,LINESIZE,stdin) == 0 )
        exit (1) ;

#ifdef STATUS
    if ( strcmp (buffer,"~status\n") == 0 )
    {
        pcstat () ;
        (void) strcpy (buffer,"LOOK\n") ;
    }
#endif // STATUS

    cnt = 0 ;
    for ( cnt = 0, b = buffer ; *b != '\n' && *b != '\0' ; )
    {
        while ( *b == ' ' || *b == '\t' )
            b++ ;
    
        for ( i = 0 ; *b != ' ' && *b != '\t' && *b != '\n' && *b != '\0' ; b++ )
            if ( i < (LEXSIZ-1) )
                words[cnt][i++] = *b ;
    
        words[cnt][i] = EOS ;
        if ( i > 0 )
            if ( ++cnt >= LEXLEN )
                break ;
    }
    return (cnt) ;
}


void command ()
{
    int i, k ;
    int cnt, val ;

    linlen = 0 ;
    for ( i = 0 ; i < LINELEN ; i++ )
        linewd[i] = -1 ;
    
    cnt = readln (lex) ;

    for ( i = 0 ; i < cnt ; i++ )
    {
        low2up (lex[i]) ;
        val = find(lex[i]) ;
        if ( class(val) == NULLCLASS )
            continue ;
        linewd[linlen++] = val ;
    }

    for ( i = 0 ; i < LINELEN ; i++ )
    {
        switch (class(linewd[i]))
        {
            case OBJECT:
            case PLACE:
                k = bitval(linewd[i]) ;
                break ;

            case VERB:
                k = XVERB ;
                break ;

            default:
                k = 0 ;
                break ;
        }
        if ( linewd[i] < 0 )
            k = BADWORD ;
        setval (argwd[i],linewd[i]) ;
        setbit (argwd[i],k) ;
    }

    setval (status,linlen) ;
    if ( linlen <= 0 )
        return ;
    
    switch ( class(linewd[0]) )
    {
        case VERB:
            setbit(status,(bitval(status)|XVERB)) ;
            break ;

        case OBJECT:
            setbit(status,(bitval(status)|XOBJECT)) ;
            break ;

        case PLACE:
            setbit(status,(bitval(status)|XPLACE)) ;
            break ;
    }

    return ;
}


int flushc ( int bp, short cbuf[], int len)
{
    int instr, level ;
    int isNew, n ;

    for ( level = 1, isNew = 1 ; level > 0 ; )
    {
        if ( bp >= len )
            error ("Flushc","nonterminating condition!") ;

        instr = cbuf[bp++] ;

#ifdef SHOWOP
        showop (NO,instr,&(cbuf[bp])) ;
#endif // SHOWOP

        if ( instr == NOT )
            continue ;

        bp += opnum (instr) ;

        if ( logical (instr) )
        {
            if ( isNew )
                level++ ;
            n = cbuf[bp] ;
            if ( n == OR || n == AND || n == EOR )
            {
                isNew = 0 ;
                bp++ ;
            }
            else
                isNew = 1 ;
            continue ;
        }

        if ( instr == ELSE && level <= 1 )
            break ;
        if ( instr == FIN )
            level-- ;
        else
            if ( instr == EOFTOK )
                break ;
    }
    return (bp) ;
}


int ssw ( int a )
{
    char *p ;

    if ( ( p = getenv("WIZARD") ) == 0 )
        return (0) ;
    
    if ( strcmp (p,"Gandalf") == 0 )
        return (1) ;
    return (0) ;
}


int svar ( int a, int b)
{
    int c ;

	struct tm *tp ;
	time_t clock ;

	clock = time (0) ;
	tp = localtime (&clock) ;

    switch (a)
    {
        case 0:
        case 1:
        case 2:
            c = 0 ;
            break ;
        case 3:
            c = tp->tm_wday ;
            break ;
        case 4:
            c = tp->tm_hour ;
            break ;
        case 5:
            c = tp->tm_min ;
            break ;
        case 6:
            c = tp->tm_sec ;
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


int nextrec ( int key )
{
    int n ;

    switch (class(key))
    {
        case INITIAL:
            if ( key < REPEAT )
            {
                if ( ++key >= ninit )
                    key = ERROR ;
            }
            else
            {
                if ( ++key >= nrep+REPEAT )
                    key = ERROR ;
            }
            break ;

        case LABEL:
            key = ERROR ;
            break ;

        case VERB:
            key += MAXVERBS ;
            n = key - VERBKEY ;
            n /= MAXVERBS ;
            if ( n >= MAXVCODE )
                key = ERROR ;
            break ;

        case PLACE:
            key += MAXPLACES ;
            n = key - PLACEKEY ;
            n /= MAXPLACES ;
            n -= 2 ;
            if ( n >= MAXPCODE )
                key = ERROR ;
            break ;
        
        case OBJECT:
            key += MAXOBJECTS ;
            n = key - OBJKEY ;
            n /= MAXOBJECTS ;
            n -= MAXOTEXT ;
            if ( n >= MAXOCODE )
                key = ERROR ;
            break ;
        
        default:
            error ("NextRey","bad key %d!",key) ;
            break ;
    }
    return (key) ;
}


void fini ()
{
    closek (dbunit) ;
    exit (0) ;
}


int savgm ()
{
    int i ;
    int uid ;
    int fd ;

    if ( ( fd = open (FREEZER,READWRITE) ) < 0 )
    {
        if ( ( fd = creat (FREEZER,0600) ) < 0 )
            return (ERROR) ;
        close (fd) ;
        if ( ( fd = open (FREEZER,READWRITE) ) < 0 )
            return (ERROR) ;
    }

    uid = getuid() + 1 ;

    forever
    {
        if ( read(fd,(char *)&sb,sizeof(sb)) != sizeof(sb) )
            break ;
        if ( sb.s_uid == uid || sb.s_uid == 0 )
        {
            (void) lseek (fd,-(sizeof(sb)),1) ;
            break ;
        }
    }

    sb.s_uid = uid ;

    for ( i = 0 ; i < OBJECTS ; i++ )
    {
        sb.s_ov[i] = objval[i] ;
        sb.s_ob[i] = objbit[i] ;
        sb.s_ol[i] = objloc[i] ;
    }

    for ( i = 0 ; i < PLACES ; i++ )
    {
        sb.s_pb[i] = plcbit[i] ;
    }

    for ( i = 0 ; i < VARS ; i++ )
    {
        sb.s_vv[i] = varval[i] ;
        sb.s_vb[i] = varbit[i] ;
    }

    if ( write (fd,(char *)&sb,sizeof(sb)) != sizeof(sb) )
    {
        (void) close(fd) ;
        return (ERROR) ;
    }
    (void) close(fd) ;
    return (OK) ;
}


int resgm ()
{
    int i ;
    int uid ;
    int fd ;

    if ( ( fd = open (FREEZER,READONLY) ) < 0 )
        return (ERROR) ;

#ifdef TOS
    uid = 1;
#else
    uid = getuid() + 1 ;
#endif

    forever
    {
        if ( read(fd,(char *)&sb,sizeof(sb)) != sizeof(sb) )
        {
            (void) close (fd) ;
            return (ERROR) ;
        }
        if ( sb.s_uid == uid )
            break ;
    }

    for ( i = 0 ; i < OBJECTS ; i++ )
    {
        objval[i] = sb.s_ov[i] ;
        objbit[i] = sb.s_ob[i] ;
        objloc[i] = sb.s_ol[i] ;
    }

    for ( i = 0 ; i < PLACES ; i++ )
    {
        plcbit[i] = sb.s_pb[i] ;
    }

    for ( i = 0 ; i < VARS ; i++ )
    {
        varval[i] = sb.s_vv[i] ;
        varbit[i] = sb.s_vb[i] ;
    }

    (void) close(fd) ;
    return (OK) ;
}


int delgm ()
{
    int uid ;
    int fd ;

    if ( ( fd = open (FREEZER,READWRITE) ) < 1 )
        return (ERROR) ;

    uid = getuid() + 1 ;

    forever
    {
        if ( read(fd,(char *)&sb,sizeof(sb)) != sizeof(sb) )
        {
            (void) close (fd) ;
            return (ERROR) ;
        }
        if ( sb.s_uid == uid )
            break ;
    }

    sb.s_uid = 0 ;
    (void) lseek (fd,-(sizeof(sb)),1) ;
    if ( write (fd,(char *)&sb,sizeof(sb)) != sizeof(sb) )
    {
        (void) close(fd) ;
        return (ERROR) ;
    }
    (void) close(fd) ;
    return (OK) ;
}


void say ( int key )
{
    int rec ;

    if ( key == NULLTXT )
        return ;
    if ( key == BLANKTXT )
    {
        printf ("\n") ;
        return ;
    }
    if ( key == OKTXT )
    {
        printf ("Ok.\n") ;
        return ;
    }

    if ( ( rec = saykey (key) ) < 0 )
        return ;

    if ( readk (dbunit,rec,textbuf,TBUFSIZE) < 1 )
        return ;
    
    puts (textbuf) ;
    return ;
}


void sayval ( int key, int val )
{
    int rec, i, endb ;

    rec = saykey (key) ;

    if ( ( endb = readk (dbunit,rec,textbuf,TBUFSIZE) ) < 1 )
        error ("Sayval","bad text key %d!",key) ;
    
    for ( i = 0 ; i < endb ; i++ )
    {
        if ( textbuf[i] == REPLACE )
        {
            printf ("%d",val) ;
            continue ;
        }
        putchar(textbuf[i]) ;
    }
    return ;
}


void saynam ( int key, int op)
{
    int i, j ;
    int rec, endb ;
    char word[20] ;

    for ( j = 0 ; j < LINELEN ; j++ )
        if ( op == argwd[j] || op == ref(argwd[j]) )
            break ;
    if ( j < LINELEN )
        (void) strncpy (word,lex[j],20) ;
    else
        if ( findnam (op,word) != op )
            error ("Saynam","bad name %d!",op) ;
    
    rec = saykey (key) ;
    if ( ( endb = readk (dbunit,rec,textbuf,TBUFSIZE) ) < 1 )
        error ("Saynam","bad key %d!",rec) ;

    up2low (word) ;

    for ( i = 0 ; i < endb ; i++ )
    {
        if ( textbuf[i] == REPLACE )
        {
            printf ("%s",word) ;
            continue ;
        }
        putchar (textbuf[i]) ;
    }
    return ;
}


void define ( char *nam, int val )
{
    struct symstr *p ;
    int h ;

    /* assume the symbol does not exist */

    if ( psmem <= 0 )
        error ("Symbol Define","symbol table full on `%s'",nam) ;

    p = &(smem[--psmem]) ;

    p->s_nam = strsav (nam) ;
    p->s_val = val ;

    h = hash (nam) ;
    p->s_nxt = symtab[h] ;
    symtab[h] = p ;

    return ;
}

#ifdef PSTAB

void pstab ()
{
    int i, j ;
    struct symstr *p ;

    for ( i = j = 0 ; i < TABSIZ ; i++ )
    {
        for ( p = symtab[i] ; p != NIL ; p = p->s_nxt )
        {
            printf ("   %-6.6s  %5d",p->s_nam,p->s_val) ;
            if ( (++j)%4 == 0 )
                printf ("\n") ;
        }
    }
    printf ("\n\n") ;

    printf ("Total number of symbols in symbol table is %d\n",j) ; 
    printf ("Total symbol structures still available is %d\n",psmem) ;
    printf ("Total character storage space used is %d\n",pcmem) ;

    return ;
}

#endif // PSTAB

#ifndef class

    int class ( int key )
    {
        if ( key < 0 || key > MAXKEY )
            return (clss[0]) ;
        return (clss[key/1000]) ;
    }

#endif // class


#ifndef indx

    int indx ( int key )
    {
        int i, m ;

        i = key % 1000 ;

        switch (class(key))
        {
            case OBJECT:
                m = OBJECTS ;
                break ;

            case PLACE:
                m = PLACES ;
                break ;

            case VARIABLE:
                m = VARS ;
                break ;

            default:
                error ("Indx","bad key %d!",key) ;
                break ;
        }
        if ( i < 0 || i >= m )
            error ("Indx","bad index %d for key %d!",i,key) ;
        return (i) ;
    }

#endif // indx

#ifndef logical

    int logical ( short instr )
    {
        if ( instr < 0 || instr >= MAXOPS )
            error ("Logical","bad opcode %d!",instr) ;
        return (ltab[instr]) ;
    }

#endif // logical


void movobj ( int key, int loc)
{
    if ( class(key) != OBJECT )
        error ("Movobj","not an object %d!",key) ;
    objloc[indx(key)] = loc ;
    return ;
}


int near ( int key )
{
    int w, b, h ;

    w = where (key) ;
    b = bitval (key) ;
    h = eval(here) ;

    if ( w == h || ( w == (h-1) && (b&DUAL) ) )
        return (YES) ;
    return (NO) ;
}


#ifndef opnum

    int opnum ( int opcode )
    {
        if ( opcode < 0 || opcode >= MAXOPS )
            error ("Opnum","bad opcode %d",opcode) ;

        return (opn[opcode]) ;
    }

#endif // opnum


#ifndef where

    int where ( int key )
    {
        if ( class(key) != OBJECT )
            error ("Where","not object key %d!",key) ;
        return (objloc[indx(key)]) ;
    }

#endif // where


void webster ()
{
    int key, i, j ;
    int totsym, size, val ;

    key = SYMTABREC ;
    if ( ( size = readk (dbunit,key,(char *)codebuf,BUFSIZE*2) ) == ERROR )
        error ("Webster","unable to access symbol table (%d)!",key) ;
    if ( size != sizeof(short int) )
        error ("Webster","bad symbol table size record (%d)!",size) ;
    
    for ( totsym = codebuf[0] ; totsym > 0 ; )
    {
        key++ ;
        if ( ( size = readk (dbunit,key,(char *)codebuf,BUFSIZE*2) ) < 1 )
            error ("Webster","bad sym table count (%d)!",totsym) ;
        size /= sizeof(short int) ;

        for ( i = 0 ; i < size ; )
        {
            for ( j = 0 ; codebuf[i] != EOS ; j++, i++ )
                token[j] = codebuf[i] ;
            token[j] = token[6] = EOS ;
            val = codebuf[i+1] ;
            i += 2 ;
            define (token,val) ;
            totsym-- ;
        }
    }

    nrep = find ("<NREP>") ;
    ninit = find ("<NINIT>") ;
    nvars = find ("<NVARS>") ;
    nobj = find ("<NOBJ>") ;
    nplace = find ("<NPLACE>") ;
    here = find ("HERE") ;
    there = find ("THERE") ;
    status = find ("STATUS") ;
    argwd[0] = find ("ARG1") ;
    argwd[1] = find ("ARG2") ;

    if ( nrep < 1 )
        error ("Webster","missing repeat code (%d)!",nrep) ;
    if ( nobj > OBJECTS )
        error ("Webster","too many objects (%d>%d)!",nobj,OBJECTS) ;
    if ( nplace > PLACES )
        error ("Webster","too many places (%d>%d)!",nplace,PLACES) ;
    if ( nvars > VARS )
        error ("Webster","too many variables (%d>%d)!",nvars,VARS) ;
    if ( class(here) != VARIABLE || class(there) != VARIABLE ||
         class(argwd[0]) != VARIABLE || class(argwd[1]) != VARIABLE ||
         class(status) != VARIABLE )
        error ("Webster","missing special variable!") ;

    return ;
}


int query ( short op )
{
    int c, d ;

    forever
    {
        say (op) ;
        printf ("? ") ;
        c = getchar () ;
        while ( (d=getchar()) != '\n' )
            if ( d == EOF )
                break ;
        if ( c == 'y' || c == 'Y' )
            return (YES) ;
        if ( c == 'n' || c == 'N' )
            return (NO) ;
    }
}


void executive ( int a, int b)
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
#endif // CACHE
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


int condition ( int bp, short cbuf[], int len )
{
    int i, cond, instr ;
    short op[3], lmode ;
    int n, negate, passon ;

    negate = 0 ;
    passon = 0 ;
    lmode  = 0 ;

    forever
    {
        instr = cbuf [bp++] ;
        n = opnum (instr) ;
        for ( i = 0 ; i < n ; i++, bp++ )
            op[i] = cbuf [bp] ;

        if ( bp > len )
            error ("Condition","bad opcode %d!",instr) ;

#ifdef SHOWOP
        showop (YES,instr,op) ;
#endif // SHOWOP

        if ( instr == NOT )
        {
            negate =  !negate ;
            continue ;
        }

        switch (instr)
        {
            case BIT:
                cond = ( (bitval(op[0]) & bitval(op[1])) != 0 );
                break ;

            case CHANCE:
                cond = ( eval (op[0]) > rnd (100) ) ;
                break ;

            case IFAT:
                cond = ( ref (op[0]) == eval (here) ) ;
                break ;

            case IFEQ:
                cond = ( eval (op[0]) == eval (op[1]) ) ;
                break ;

            case IFGE:
                cond = ( eval (op[0]) >= eval (op[1]) ) ;
                break ;

            case IFGT:
                cond = ( eval (op[0]) > eval (op[1]) ) ;
                break ;

            case IFHAVE:
                cond = ( where ( ref (op[0]) )  == INHAND ) ;
                break ;

            case IFKEY:
                cond = 0 ;
                for ( i = 0 ; i < linlen ; i++ )
                    if ( (cond = (op[0] == linewd[i])) )
                        break ;
                break ;

            case IFLE:
                cond = ( eval (op[0]) <= eval (op[1]) ) ;
                break ;

            case IFLOC:
                cond = ( where ( ref(op[0]) ) == ref (op[1]) ) ;
                break ;

            case IFLT:
                cond = ( eval (op[0]) < eval (op[1]) ) ;
                break ;

            case IFNEAR:
                cond = ( where ( ref (op[0]) ) == INHAND ||
                     near ( ref (op[0]) ) == YES ) ;
                break ;

            case QUERY:
                cond = (query ( ref (op[0]) ) == YES) ;
                break ;

            default:
                error ("Condition","bad opcode %d!",instr) ;
                break ;
        }

        cond = ((cond||negate)&&(!(cond&&negate))) ;

        switch ( lmode )
        {
            case OR :
                cond = ( cond || passon ) ;
                break ;

            case AND :
                cond = ( cond && passon ) ;
                break ;

            case EOR :
                cond = ((cond||passon)&&(!(cond&&passon))) ;
                break ;
        }

        switch ( cbuf[bp] )
        {
            case OR :
                lmode = OR ;
                break ;

            case AND :
                lmode = AND ;
                break ;

            case EOR :
                lmode = EOR ;
                break ;

            default :
                if ( cond )
                    return (bp) ;
                return (flushc (bp,cbuf,len)) ;
        }

        bp++ ;
        passon = cond ;
        negate = 0 ;
    }
}


void process ( int key )
{
    int rec, calls, dos, endb ;
    int srec[MAXCALLS], sbp[MAXCALLS], sdos[MAXCALLS] ;
    int dovar[MAXDOS], dopnt[MAXDOS], domod[MAXDOS] ;
    short int instr, op[3] ;
    int bp, i, j ;
    int n ;

    rec = key ;
    calls = 0 ;
    dos = 0 ;

    forever
    {
        bp = 0 ;
        while ( rec == ERROR || ( endb = rdcode (rec) ) == ERROR )
        {
            if ( calls <= 0 )
                return ;
            rec = srec [calls] ;
            bp = sbp [calls] ;
            dos = sdos [calls] ;
            calls-- ;
        }

#ifdef SHOWOP
        CurKey = rec ;
#endif // SHOWOP
        while ( bp < endb )
        {
            instr = codebuf [bp] ;
            if ( logical (instr) )
            {
                bp = condition (bp,codebuf,endb) ;
                continue ;
            }
            bp++ ;
            n = opnum (instr) ;
            for ( i = 0 ; i < n ; i++, bp++ )
                op [i] = codebuf [bp] ;

#ifdef SHOWOP
            showop (YES,instr,op) ;
#endif // SHOWOP
            switch (instr)
            {
              case ADD:

                setval (op[0],(eval(op[0])+eval(op[1]))) ;
                continue ;

              case ANYOF:

                bp -= 2 ;
                for ( j = 0 ; codebuf [bp] == ANYOF ; bp+=2 )
                    for ( i = 0 ; i < LINELEN ; i++ )
                        if ( codebuf [bp+1] == linewd [i] )
                            j++ ;
                if ( j > 0 )
                    continue ;
                break ;

              case APPORT:

                movobj ( ref (op[0]), ref (op[1]) ) ;
                continue ;

              case AT:

                bp -= 2 ;
                i = eval (here) ;
                for ( j = 0 ; codebuf [bp] == AT ; bp += 2 )
                    if ( codebuf [bp+1] == i )
                        j++ ;
                if ( j > 0 )
                    continue ;
                break ;

              case BIC:

                setbit (op[0],bitval(op[0])&(~bitval(op[1]))) ;
                continue ;

              case BIS:

                setbit (op[0],bitval(op[0])|bitval(op[1])) ;
                continue ;

              case CALL:

                if ( (++calls) >= MAXCALLS )
                   error ("Process","call stack overflow (%d)!",
                       calls) ;
                srec [calls] = rec ;
                sbp  [calls] = bp ;
                sdos [calls] = dos ;
                rec = ref ( op[0] ) ;
                j = class ( rec ) ;
                if ( j == OBJECT )
                    rec += (MAXOTEXT * MAXOBJECTS) ;
                else
                    if ( j == PLACE )
                        rec += ( 2 * MAXPLACES ) ;
                break ;

              case DEFAULT:

                if ( linlen != 1 )
                    continue ;
                j = 0 ;
                for ( i = OBJKEY ; indx(i) < nobj ; i++ )
                {
                    if ( near (i) == NO ||
                         (bitval(i)&bitval(op[0])) == 0 )
                        continue ;
                    if ( j != 0 )
                    {
                        j = 0 ;
                        break ;
                    }
                    j = i ;
                }
                if ( j == 0 )
                    continue ;
                setval (argwd[1],j) ;
                setbit (argwd[1],bitval(j)) ;
                linewd[1] = j ;
                setval (status,2) ;
                linlen = 2 ;
                continue ;

              case DEPOSIT:

                setval (ref(op[0]),eval(op[1])) ;
                continue ;

              case DIVIDE:

                setval (op[0],(eval(op[0])/eval(op[1]))) ;
                continue ;

              case DROP:

                movobj ( ref (op[0]), eval (here) ) ;
                continue ;

              case ELSE:

                bp = flushc (bp,codebuf,endb) ;
                continue ;

              case EOFTOK:

                continue ;

              case EOI:

                i = eval ( dovar[dos] ) ;
                switch ( domod[dos] )
                {
                    /* only iterate with nearby objects */
                    case ITLIST:

                        do
                        {
                            if ( indx(++i) >= nobj )
                            {
                                dos-- ;
                                break ;
                            }
                        }
                        while ( near (i) != YES ) ;
                        if ( indx(i) >= nobj )
                            continue ;
                        break ;

                    case ITOBJ :

                        if ( indx(++i) >= nobj )
                        {
                            dos-- ;
                            continue ;
                        }
                        break ;

                    case ITPLACE :

                        if ( indx(++i) >= nplace )
                        {
                            dos-- ;
                            continue ;
                        }
                        break ;

                    default :
                        error ("Process",
                            "unknown IT* type!") ;
                        break ;

                }
                setval (dovar[dos],i) ;
                bp = dopnt[dos] ;
                continue ;

              case EVAL:

                setval (op[0],eval(ref(op[1]))) ;
                continue ;

              case EXEC:

                executive (op[0],op[1]) ;
                continue ;

              case FIN:

                continue ;

              case GET:

                j = ref ( op[0] ) ;
                movobj (j,INHAND) ;
                i = eval (j) ;
                if ( i < 0 )
                    setval (j,-1-i) ;
                continue ;

              case GOTO:

                /* must do this so "back" will work */
                i = eval (here) ;
                j = bitval (here) ;

                setval (here,ref(op[0])) ;
                setbit (here,bitval(ref(op[0]))) ;
                setval (there,i) ;
                setbit (there,j) ;
                setbit (status,(bitval(status)|MOVED)) ;
                continue ;

              case HAVE:

                if ( where ( ref (op[0]) ) == INHAND )
                    continue ;
                break ;

              case INPUT:

                command () ;
                continue ;

              case ITLIST:

                dos++ ;
                dovar[dos] = op[0] ;
                /* do first object and all others nearby */
                setval (op[0],OBJKEY) ;
                dopnt[dos] = bp ;
                domod[dos] = ITLIST ;
                continue ;

              case ITOBJ:

                dos++ ;
                dovar[dos] = op[0] ;
                setval (op[0],OBJKEY) ;
                dopnt[dos] = bp ;
                domod[dos] = ITOBJ ;
                continue ;

              case ITPLACE:

                dos++ ;
                dovar[dos] = op[0] ;
                setval (op[0],PLACEKEY) ;
                dopnt[dos] = bp ;
                domod[dos] = ITPLACE ;
                continue ;

              case KEYWORD:

                for ( i = 0 ; i < LINELEN ; i++ )
                    if ( op[0] == linewd[i] )
                        break ;
                if ( i < LINELEN )
                    continue ;
                break ;

              case LDA:

                setval (op[0],op[1]) ;
                continue ;

              case LOCATE:

                setval (op[0],where(ref(op[1]))) ;
                continue ;

              case MOVE:

                if ( op[0] != linewd[0] )
                {
                    if ( linlen < 2 )
                        continue ;
                    if ( op[0] != linewd[1] )
                        continue ;
                }
                i = eval (here) ;
                j = bitval (here) ;
                setval (here,ref(op[1])) ;
                setbit (here,bitval(ref(op[1]))) ;
                setval (there,i) ;
                setbit (there,j) ;
                setbit (status,(bitval(status)|MOVED)) ;
                return ;

              case MULT:

                setval (op[0],(eval(op[0])*eval(op[1]))) ;
                continue ;

              case NAME:

                for ( i = 0 ; i < LINELEN ; i++ )
                    if ( op[1] == argwd[i] )
                    {
                        saynam (ref(op[0]),op[1]) ;
                        break ;
                    }
                if ( i >= LINELEN )
                    saynam (ref(op[0]),ref(op[1])) ;
                continue ;

              case NEAR:

                i = ref ( op[0] ) ;
                if ( where (i) == INHAND || near (i) == YES )
                    continue ;
                break ;

              case PROCEED:

                break ;

              case QUIT:

                return ;

              case RANDOM:

                setval (op[0],rnd(eval(op[1]))) ;
                continue ;

              case SAY:

                say ( ref (op[0]) ) ;
                continue ;

              case SET:

                setval (op[0],eval(op[1])) ;
                continue ;

              case SMOVE:

                if ( op[0] != linewd[0] )
                {
                    if ( linlen < 2 )
                        continue ;
                    if ( op[0] != linewd[1] )
                        continue ;
                }
                i = eval (here) ;
                j = bitval (here) ;
                setval (here,ref(op[1])) ;
                setbit (here,bitval(ref(op[1]))) ;
                setval (there,i) ;
                setbit (there,j) ;
                setbit (status,(bitval(status)|MOVED)) ;
                say (op[2]) ;
                return ;

              case STOP:

                fini () ;

              case SUB:

                setval (op[0],(eval(op[0])-eval(op[1]))) ;
                continue ;

              case SVAR:

                i = eval (op[1]) ;
                i = svar (eval(op[0]),i) ;
                setval (op[1],i) ;
                continue ;

              case VALUE:

                sayval (ref(op[0]),ref(op[1])) ;
                continue ;

              default:

                error ("Process","unrecognized opcode %d!",
                    instr) ;
                break ;

            }
            break ;
        }

        if ( instr != CALL )
            rec = nextrec (rec) ;
    }
}


int main ()
{
    srandom ( time(0)+getpid() ) ;

    setup () ;
    opendb (ADV) ;
    webster () ;

#ifdef PSTAB
    pstab () ;
#endif // PSTAB

    process (INITIAL) ;

    forever
        process (REPEAT) ;
}
