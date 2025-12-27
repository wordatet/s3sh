#
/*
 * UNIX shell
 *
 * S. R. Bourne
 * Bell Telephone Laboratories
 *
 */

#include	"defs.h"


STRING		trapcom[MAXTRAP];
BOOL		trapflg[MAXTRAP]= {
					0,
					0,	/* hangup */
					SIGCAUGHT,	/* interrupt */
					SIGCAUGHT,	/* quit */
					0,	/* illegal instr */
					0,	/* trace trap */
					0,	/* IOT */
					0,	/* EMT */
					0,	/* float pt. exp */
					0,	/* kill */
					0, 	/* bus error */
					0,	/* memory faults */
					0,	/* bad sys call */
					0,	/* bad pipe call */
					SIGCAUGHT,	/* alarm */
					SIGCAUGHT, 	/* software termination */
					0,	/* unassigned */
					};

/* ========	fault handling routines	   ======== */


VOID	fault(sig)
	REG INT		sig;
{
	REG INT		flag;

	signal(sig, (void (*)(int))fault);
	IF sig==MEMF
	THEN	error(nospace);
	ELIF sig==ALARM
	THEN	IF flags&waiting
		THEN	done();
		FI
	ELSE	flag = (trapcom[sig] ? TRAPSET : SIGSET);
		trapnote |= flag;
		trapflg[sig] |= flag;
		IF sig == INTR THEN wasintr++ FI
	FI
}

stdsigs()
{
	signal(MEMF, (void (*)(int))fault);
	ignsig(QUIT);
	getsig(INTR);
	getsig(ALARM);
}

ignsig(n)
{
	REG INT		s, i;

	IF (i=n)==MEMF
	THEN	clrsig(i);
		failed(badtrap, "cannot trap 11");
	ELIF (s=(intptr_t)signal(i, SIG_IGN)&01)==0
	THEN	trapflg[i] |= SIGMOD;
	FI
	return(s);
}

getsig(n)
{
	REG INT		i;

	IF trapflg[i=n]&SIGMOD ORF ignsig(i)==0
	THEN	signal(i, (void (*)(int))fault);
	FI
}

oldsigs()
{
	REG INT		i;
	REG STRING	t;

	i=MAXTRAP;
	WHILE i--
	DO  t=trapcom[i];
		    IF t==0 ORF *t
		    THEN clrsig(i);
		    FI

		    trapflg[i]=0;
	OD
	trapnote=0;
}

clrsig(i)
	INT		i;
{
	free(trapcom[i]); trapcom[i]=0;
	IF trapflg[i]&SIGMOD
	THEN	IF trapflg[i]&SIGCAUGHT
		THEN signal(i, (void (*)(int))fault);
		ELSE	signal(i, SIG_DFL);
		FI
		trapflg[i] &= ~SIGMOD;
	FI
}

chktrap()
{
	/* check for traps */
	REG INT		i=MAXTRAP;
	REG STRING	t;

	trapnote &= ~TRAPSET;
	WHILE --i
	DO IF trapflg[i]&TRAPSET
	   THEN trapflg[i] &= ~TRAPSET;
		IF t=trapcom[i]
		THEN	INT	savxit=exitval;
			execexp(t,0);
			exitval=savxit;
			 exitset();
		FI
	   FI
	OD
}
