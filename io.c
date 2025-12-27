#
/*
 * UNIX shell
 *
 * S. R. Bourne
 * Bell Telephone Laboratories
 *
 */

#include	"defs.h"
#include	"dup.h"


/* ========	input output and file copying ======== */

VOID initf(fd)
	UFD		fd;
{
	REG S3_FILE	f=standin;

	f->fdes=fd; f->fsiz=((flags&oneflg)==0 ? BUFSIZ : 1);
	f->fnxt=f->fend=f->fbuf; f->feval=0; f->flin=1;
	f->feof=FALSE;
}

INT estabf(s)
	REG STRING	s;
{
	REG S3_FILE	f;

	(f=standin)->fdes = -1;
	f->fend=length(s)+(f->fnxt=s);
	f->flin=1;
	return(f->feof=(s==0));
}

VOID push(af)
	S3_FILE		af;
{
	REG S3_FILE	f;

	(f=af)->fstak=standin;
	f->feof=0; f->feval=0;
	standin=f;
}

INT pop()
{
	REG S3_FILE	f;

	IF (f=standin)->fstak
	THEN	IF f->fdes>=0 THEN close(f->fdes) FI
		standin=f->fstak;
		return(TRUE);
	ELSE	return(FALSE);
	FI
}

VOID chkpipe(pv)
	INT		*pv;
{
	IF pipe(pv)<0 ORF pv[INPIPE]<0 ORF pv[OTPIPE]<0
	THEN	error(piperr);
	FI
}

INT chkopen(idf)
	STRING		idf;
{
	REG INT		rc;

	IF (rc=open(idf,0))<0
	THEN	failed(idf,badopen);
	ELSE	return(rc);
	FI
}

VOID sh_rename(f1,f2)
	REG INT		f1, f2;
{
#ifdef RES	/*	research has different sys calls from TS	*/
	IF f1!=f2
	THEN	dup(f1|DUPFLG, f2);
		close(f1);
		IF f2==0 THEN ioset|=1 FI
	FI
#else
	INT	fs;
	IF	f1!=f2
	THEN 	fs = fcntl(f2,1,0);
		close(f2);
		fcntl(f1,0,f2);
		close(f1);
		IF fs==1 THEN fcntl(f2,2,1) FI
		IF f2==0 THEN ioset|=1 FI
	FI
#endif
}

INT create(s)
	STRING		s;
{
	REG INT		rc;

	IF (rc=creat(s,0666))<0
	THEN	failed(s,badcreate);
	ELSE	return(rc);
	FI
}

INT tmpfil()
{
	itos(serial++); movstr(numbuf,sh_tmpnam);
	return(create(tmpout));
}

/* set by trim */
BOOL		nosubst;

VOID copy(ioparg)
	IOPTR		ioparg;
{
	CHAR		c, *ends;
	REG CHAR	*cline, *clinep;
	INT		fd;
	REG IOPTR	iop;

	IF iop=ioparg
	THEN	copy(iop->iolst);
		ends=mactrim(iop->ioname); IF nosubst THEN iop->iofile &= ~IODOC FI
		fd=tmpfil();
		iop->ioname=cpystak(tmpout);
		iop->iolst=iotemp; iotemp=iop;
		cline=locstak();

		IF stripflg
		THEN	WHILE *ends=='\t' DO ends++ OD
		FI
		LOOP	clinep=cline; chkpr(NL);
			IF stripflg
			THEN
				WHILE (c=(nosubst ? readc() : nextc(*ends)), !eolchar(c))
				&& cline == clinep && c == '\t' DONE
				WHILE (!eolchar(c))
				DO
					*clinep++=c;
					c=(nosubst ? readc() : nextc(*ends));
				OD
			ELSE
			WHILE (c = (nosubst ? readc() :  nextc(*ends)),  !eolchar(c)) DO *clinep++ = c   OD
			FI
			*clinep=0;
			IF eof ORF eq(cline,ends) THEN break FI
			*clinep++=NL;
			if (write(fd,cline,clinep-cline) == -1) { /* ignore */ }
		POOL
		IF stripflg THEN stripflg-- FI
		close(fd);
	FI
}
