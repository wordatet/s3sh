#
/*
 *	UNIX shell
 */

#include "s3sh_shims.h"


/* error exits from various parts of shell */
#define ERROR	1
#define SYNBAD	2
#define SIGFAIL 3
#define SIGFLG	0200

/* command tree */
#define FPRS	020
#define FINT	040
#define FAMP	0100
#define FPIN	0400
#define FPOU	01000
#define FPCL	02000
#define FCMD	04000
#define COMMSK	017

#define TCOM	0
#define TPAR	1
#define TFIL	2
#define TLST	3
#define TIF	4
#define TWH	5
#define TUN	6
#define TSW	7
#define TAND	8
#define TORF	9
#define TFORK	10
#define TFOR	11

/* execute table */
#define SYSSET	1
#define SYSCD	2
#define SYSEXEC	3
#ifdef RES	/*	include login code	*/
#define SYSLOGIN	4
#else
#define SYSNEWGRP 4
#endif
#define SYSTRAP	5
#define SYSEXIT	6
#define SYSSHFT 7
#define SYSWAIT	8
#define SYSCONT 9
#define SYSBREAK 10
#define SYSEVAL 11
#define SYSDOT	12
#define SYSRDONLY 13
#define SYSTIMES 14
#define SYSXPORT 15
#define SYSNULL 16
#define SYSREAD 17
#define	SYSTST	18
#ifndef RES	/*	exclude umask code	*/
#define SYSUMASK 20
#endif

/*	builtin table	*/

#define TEST	127
/* used for input and output of shell */
#define INIO 18
#define OTIO 19

/*io nodes*/
#define USERIO	10
#define IOUFD	15
#define IODOC	16
#define IOPUT	32
#define IOAPP	64
#define IOMOV	128
#define IORDW	256
#define INPIPE	0
#define OTPIPE	1

/* arg list terminator */
#define ENDARGS	0

#include	"mac.h"
#include	"mode.h"
#include	"name.h"


/*	error catching */
INT errno;

/* result type declarations */
#include <stdlib.h>
#define alloc malloc
#define free free
STRING		make();
STRING		movstr();
TREPTR		cmd();
TREPTR		makefork();
NAMPTR		lookup();
INT		setname();
VOID		setargs();
DOLPTR		useargs();
DOLPTR		freeargs();
REAL		expr();
STRING		catpath();
STRING		getpath();
STRING		*scan();
STRING		mactrim();
STRING		macro();
STRING		execs();
STRING		simple();
STRING		nxtarg();
VOID		await();
VOID		post();
VOID		exname();
STRING		staknam();
VOID		printnam();
VOID		sh_rename();
#define rename sh_rename
VOID		printflg();
VOID		prs();
VOID		prc();
INT		sh_getenv();
STRING		*sh_setenv();

#define 	setflg	010
#define attrib(n,f)	(n->namflg |= f)
#define round(a,b)	(((intptr_t)((ADR(a)+b)-1))&~((b)-1))
#define closepipe(x)	(close(x[INPIPE]), close(x[OTPIPE]))
INT		cf();
INT		any();
#define eq(a,b)		(cf(a,b)==0)
#define max(a,b)	((a)>(b)?(a):(b))
#define assert(x)	;

/* Added prototypes for 64-bit safety and warning cleanup */
VOID    newline();
VOID    blank();
VOID    prp();
VOID    prt();
VOID    prn();
VOID    itos();
INT     stoi();

VOID    exitset();
VOID    sigchk();
VOID    failed();
VOID    error();
VOID    exitsh();
VOID    done();
VOID    rmtemp();

VOID    initf();
INT     estabf();
VOID    push();
INT     pop();
VOID    chkpipe();
INT     chkopen();
INT     create();
INT     tmpfil();
VOID    copy();

VOID    stdsigs();
INT     ignsig();
INT     getsig();
VOID    oldsigs();
VOID    clrsig();
VOID    chktrap();

INT execute();
INT syslook();
VOID setlist();
VOID replace();
VOID dfault();
VOID assign();
VOID chkpr();
INT gmatch();
INT nextc();
INT readc();
VOID makearg();
VOID execa();
INT getarg();
VOID trim();
VOID subst();
VOID clearup();
INT expand();
VOID bfailed();
INT pathopen();
INT length();
INT word();
VOID chgquot();
INT test();
INT     execexp();
VOID    builtin();
VOID    initio(); 
INT		options();
VOID	assnum();
INT		readvar();
VOID	namscan();
VOID	postclr();
VOID	settmp();
VOID	Ldup();
VOID	tdystak();
VOID	stakchk();
INT		profil();

/* temp files and io */
UFD		output;
INT		ioset;
IOPTR		iotemp;		/* files to be deleted sometime */
IOPTR		iopend;		/* documents waiting to be read at NL */
INT	stripflg;

/* substitution */
INT		dolc;
STRING		*dolv;
STRING		cmdadr;
DOLPTR		argfor;
ARGPTR		gchain;

/* stack */
#define		BLK(x)	((BLKPTR)(x))
#define		BYT(x)	((BYTPTR)(x))
#define		STK(x)	((STKPTR)(x))
#define		ADR(x)	((char*)(x))

/* stak stuff */
#include	"stak.h"
VOID		initstak();

/* string constants */
extern MSG		atline;
extern MSG		readmsg;
extern MSG		colon;
extern MSG		minus;
extern MSG		nullstr;
extern MSG		sptbnl;
extern MSG		unexpected;
extern MSG		endoffile;
extern MSG		synmsg;

/* name tree and words */
extern SYSTAB reserved;
INT		wdval;
INT		wdnum;
ARGPTR		wdarg;
INT		wdset;
BOOL		reserv;

/* prompting */
extern MSG		stdprompt;
extern MSG		supprompt;
extern MSG		profile;
extern MSG		sysprofile;

/* built in names */
NAMNOD		fngnod;
NAMNOD		ifsnod;
NAMNOD		homenod;
NAMNOD		mailnod;
NAMNOD		pathnod;
NAMNOD		ps1nod;
NAMNOD		ps2nod;

/* special names */
extern CHAR	flagadr[];
STRING		exitadr;
STRING		dolladr;
STRING		pcsadr;
STRING		pidadr;

extern MSG		defpath;

/* names always present */
extern MSG		mailname;
extern MSG		homename;
extern MSG		pathname;
extern MSG		fngname;
extern MSG		ifsname;
extern MSG		ps1name;
extern MSG		ps2name;

/* transput */
extern CHAR tmpout[];
STRING		sh_tmpnam;
INT		serial;
#define		TMPNAM 7
S3_FILE		standin;
#define input	(standin->fdes)
#define eof	(standin->feof)
INT		peekc;
STRING		comdiv;
extern MSG		devnull;

/* flags */
#define		noexec	01
#define	sysflg	01
#define		intflg	02
#define		prompt	04
#define		errflg	020
#define		ttyflg	040
#define		forked	0100
#define		oneflg	0200
#define		rshflg	0400
#define		waiting	01000
#define		stdflg	02000
#define		STDFLG	's'
#define		STDFLGLOC	4
#define		execpr	04000
#define		readpr	010000
#define		keyflg	020000
INT		flags;
INT		rwait;	/* flags read waiting */

/* error exits from various parts of shell */
#include	<setjmp.h>
jmp_buf		subshell;
jmp_buf		errshell;

/* fault handling */
#include	"brkincr.h"
POS	brkincr;
#define MINTRAP	0
#define MAXTRAP	17

#define INTR	2
#define QUIT	3
#define MEMF	11
#define ALARM	14
#define KILL	15
#define TRAPSET	2
#define SIGSET	4
#define SIGMOD	8
#define SIGCAUGHT	16

VOID		fault();
BOOL		trapnote;
extern STRING trapcom[];
extern BOOL trapflg[];

/* name tree and words */
STRING		*environ;
extern CHAR numbuf[];
extern MSG		export;
extern MSG	duperr;
extern MSG		readonly;

/* execflgs */
INT		exitval;
BOOL		execbrk;
INT		loopcnt;
INT		breakcnt;

/* messages */
extern MSG		mailmsg;
extern MSG		coredump;
extern MSG		badopt;
extern MSG		badparam;
extern MSG		unset;
extern MSG		badsub;
extern MSG		nospace;
extern MSG		notfound;
extern MSG		badtrap;
extern MSG		baddir;
extern MSG		badshift;
extern MSG		illegal;
extern MSG		restricted;
extern MSG		execpmsg;
extern MSG		notid;
extern MSG		wtfailed;
extern MSG		badcreate;
extern MSG		nofork;
extern MSG		noswap;
extern MSG		piperr;
extern MSG		badopen;
extern MSG		badnum;
extern MSG		arglist;
extern MSG		txtbsy;
extern MSG		toobig;
extern MSG		badexec;
extern MSG		badfile;

/*	'builtin' error messages	*/

extern MSG		btest;
extern MSG		badop;

/*	fork constant	*/
#define FORKLIM 32
/*	comment delimeter 	*/

#define	COMCHAR	'#'
extern address end[];

#include	"ctype.h"

INT		wasintr;	/* used to tell if break or delete is hit
				   while executing a wait		 */
