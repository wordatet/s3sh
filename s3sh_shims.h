#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef S3SH_SHIMS_H
#define S3SH_SHIMS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/times.h>

/* PDP-11 / V7/S3 era types often need mapping */
#define LOCAL static
#define PROC extern

/* 64-bit safe pointer punning */
#undef Lcheat
#undef Rcheat
#define Lcheat(a) (*(intptr_t *)&(a))
#define Rcheat(a) ((intptr_t)(a))

/* sbrk removal */
#define sbrk(x) s3sh_sbrk_shim(x)
void *s3sh_sbrk_shim(intptr_t incr);
void *s3sh_get_heap_base(void);
#define S3SH_HEAP_BASE s3sh_get_heap_base()
void s3sh_init(void);

#endif /* S3SH_SHIMS_H */
