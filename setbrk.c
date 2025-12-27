#
/*
 *	UNIX shell
 *
 *	S. R. Bourne
 *	Bell Telephone Laboratories
 *
 */

#include	"defs.h"

ADDRESS setbrk(incr)
	INT		incr;
{
	REG BYTPTR	a=(BYTPTR)(sbrk(incr));
	brkend=a+incr;
	return((ADDRESS)(a));
}
