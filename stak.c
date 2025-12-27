#
/*
 * UNIX shell
 *
 * S. R. Bourne
 * Bell Telephone Laboratories
 *
 */

#include	"defs.h"
#include    <stdio.h>

STKPTR		stakbot;
STKPTR      staktop;
STKPTR      stakbas;
STKPTR      brkend;

LOCAL STKPTR stak_buffer;
LOCAL POS    stak_size;

/* ========	storage allocation	======== */

VOID	initstak()
{
    stak_size = BRKINCR * 8;
    stak_buffer = (STKPTR)malloc(stak_size);
    IF !stak_buffer THEN abort() FI
	stakbot = staktop = stakbas = stak_buffer;
    brkend = stak_buffer + stak_size;
}

LOCAL VOID expand_stak(needed)
    POS needed;
{
    POS bot_off = stakbot - stak_buffer;
    POS top_off = staktop - stak_buffer;
    POS bas_off = stakbas - stak_buffer;
    
    POS new_size = stak_size + MAX(needed, BRKINCR * 4);
    stak_buffer = (STKPTR)realloc(stak_buffer, new_size);
    IF !stak_buffer THEN abort() FI
    
    stak_size = new_size;
    stakbot = stak_buffer + bot_off;
    staktop = stak_buffer + top_off;
    stakbas = stak_buffer + bas_off;
    brkend = stak_buffer + stak_size;
}

STKPTR	getstak(asize)
	INT		asize;
{	/* allocate requested stack */
	REG POS		size = round(asize, BYTESPERWORD);
    IF stakbot + size >= brkend
    THEN expand_stak(size)
    FI
	REG STKPTR	oldstak = stakbot;
	staktop = stakbot += size;
	return(oldstak);
}

STKPTR	locstak()
{	/* set up stack for local use
	 * should be followed by `endstak'
	 */
	IF brkend-staktop < BRKINCR
	THEN expand_stak(BRKINCR)
	FI
	return(stakbot);
}

STKPTR	savstak()
{
	return(stakbot);
}

STKPTR	endstak(argp)
	REG STRING	argp;
{	/* tidy up after `locstak' */
	REG STKPTR	oldstak;
	*argp++=0;
	oldstak=stakbot; 
    stakbot=staktop=(STKPTR) round(argp,BYTESPERWORD);
	return(oldstak);
}

VOID	tdystak(x)
	REG STKPTR 	x;
{
	/* try to bring stack back to x */
	staktop=stakbot=MAX(ADR(x),ADR(stakbas));
	rmtemp(x);
}

stakchk()
{
    /* No-op: realloc handles growth, and we don't need to shrink for now */
}

STKPTR	cpystak(x)
	STKPTR		x;
{
	return(endstak(movstr(x,locstak())));
}
