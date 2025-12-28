
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "defs.h"

/* Manual prototype since it's often missing or non-standard */
extern int profil(unsigned short *buf, size_t bufsiz, size_t offset, unsigned int scale);

VOID monitor(lowpc, highpc, buf, bufsiz, cntsiz)
    char *lowpc, *highpc;
    int *buf, bufsiz, cntsiz;
{
    register int o;
    static int *sbuf, ssiz;

    if (lowpc == 0) {
#if defined(__GNUC__) && !defined(__PCC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull"
#endif
        profil((unsigned short *)0, 0, 0, 0);
#if defined(__GNUC__) && !defined(__PCC__)
#pragma GCC diagnostic pop
#endif
        char template[] = "profXXXXXX";
        o = mkstemp(template);
        if (write(o, sbuf, ssiz<<1) == -1) { /* ignore */ }
        close(o);
        return;
    }
    ssiz = bufsiz;
    buf[0] = (int)(intptr_t)lowpc;
    buf[1] = (int)(intptr_t)highpc;
    buf[2] = cntsiz;
    sbuf = buf;
    buf += 3*(cntsiz+1);
    bufsiz -= 3*(cntsiz+1);
    if (bufsiz<=0)
        return;
    o = ((highpc - lowpc)>>1) & 077777;
    if(bufsiz < o)
        o = ((long)bufsiz<<15) / o;
    else
        o = 077777;
    profil((unsigned short *)buf, bufsiz<<1, (size_t)(intptr_t)lowpc, o<<1);
}
