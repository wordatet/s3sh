#ifndef _COMPAT_H
#define _COMPAT_H

#ifdef __PCC__
#ifndef __attribute__
#define __attribute__(x)
#endif
#ifndef __cold__
#define __cold__
#endif
#ifndef __leaf__
#define __leaf__
#endif
#ifndef __throw
#define __throw
#endif
#ifndef __THROW
#define __THROW
#endif
#ifndef __nonnull
#define __nonnull(x)
#endif
#ifndef __wur
#define __wur
#endif
#ifndef __warn_unused_result__
#define __warn_unused_result__
#endif
#ifndef MB_LEN_MAX
#define MB_LEN_MAX 16
#endif
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif
#endif

/* Linux / 64-bit headers */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <stdint.h>
#include <limits.h>

#endif /* _COMPAT_H */
