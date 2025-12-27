#
/*
 *	UNIX shell
 *
 *	S. R. Bourne
 *	Bell Telephone Laboratories
 *
 */

#include	"defs.h"


/*
 *	storage allocator
 *	(circular first fit strategy)
 */

#define BUSY 01
#define busy(x)	(Rcheat((x)->word)&BUSY)

extern char *s3sh_end_ptr;
#define end s3sh_end_ptr

POS		brkincr=BRKINCR;
BLKPTR		blokp;			/*current search pointer*/
BLKPTR		bloktop;		/*top of arena (last blok) initialized later */



ADDRESS	sh_alloc(nbytes)
	POS		nbytes;
{
	REG POS		rbytes = round(nbytes+BYTESPERWORD,BYTESPERWORD);
    fprintf(stderr, "[Alloc] alloc(%ld) -> rbytes=%ld\n", (long)nbytes, (long)rbytes);

	LOOP	INT		c=0;
		REG BLKPTR	p = blokp;
		REG BLKPTR	q;
		REP	
            fprintf(stderr, "  [Alloc] check p=%p, word=%p, busy=%d\n", p, p->word, busy(p));
            IF !busy(p)
			THEN	WHILE !busy(q = p->word) DO 
                        fprintf(stderr, "    [Alloc] merge p=%p with q=%p\n", p, q);
                        p->word = q->word;
                    OD
				IF ADR(q)-ADR(p) >= rbytes
				THEN	blokp = BLK(ADR(p)+rbytes);
					IF q > blokp
					THEN	blokp->word = p->word;
					FI
					p->word=BLK(Rcheat(blokp)|BUSY);
                    fprintf(stderr, "  [Alloc] success: return %p, new blokp=%p\n", ADR(p+1), blokp);
					return(ADR(p+1));
				FI
			FI
			q = p; p = BLK(Rcheat(p->word)&~BUSY);
		PER	p>q ORF (c++)==0 DONE
        fprintf(stderr, "  [Alloc] failed to find space, calling addblok\n");
		addblok(rbytes);
	POOL
}

VOID	addblok(reqd)
	POS		reqd;
{
    fprintf(stderr, "[Blok] addblok: reqd=%ld, bloktop=%p, staktop=%p\n", (long)reqd, bloktop, staktop);
	IF stakbas!=staktop
	THEN	REG STKPTR	rndstak;
		REG BLKPTR	blokstak;

		pushstak(0);
		rndstak=(STKPTR) round(staktop,BYTESPERWORD);
		blokstak=BLK(stakbas)-1;
		blokstak->word=stakbsy; stakbsy=blokstak;
		bloktop->word=BLK(Rcheat(rndstak)|BUSY);
		bloktop=BLK(rndstak);
	FI
	reqd += brkincr; reqd &= ~(brkincr-1);
	blokp=bloktop;
	bloktop=bloktop->word=BLK(Rcheat(bloktop)+reqd);
	bloktop->word=BLK(ADR(end)+1);
	BEGIN
	   REG STKPTR stakadr=STK(bloktop+2);
	   staktop=movstr(stakbot,stakadr);
	   stakbas=stakbot=stakadr;
	END
}

VOID	sh_free(ap)
	BLKPTR		ap;
{
	REG BLKPTR	p;

	IF (p=ap) ANDF p<bloktop
	THEN	Lcheat((--p)->word) &= ~BUSY;
	FI
}

#ifdef DEBUG
chkbptr(ptr)
	BLKPTR	ptr;
{
	INT		exf=0;
	REG BLKPTR	p = end;
	REG BLKPTR	q;
	INT		us=0, un=0;

	LOOP
	   q = Rcheat(p->word)&~BUSY;
	   IF p==ptr THEN exf++ FI
	   IF q<end ORF q>bloktop THEN abort(3) FI
	   IF p==bloktop THEN break FI
	   IF busy(p)
	   THEN us += q-p;
	   ELSE un += q-p;
	   FI
	   IF p>=q THEN abort(4) FI
	   p=q;
	POOL
	IF exf==0 THEN abort(1) FI
	prn(un); prc(SP); prn(us); prc(NL);
}
#endif
