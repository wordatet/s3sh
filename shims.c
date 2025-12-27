#include <stdlib.h>
#include "defs.h"

/* Minimal shims for the new malloc-only architecture */

void s3sh_init(void) {
    /* No-op: handled by each component independently now */
}

void *s3sh_sbrk_shim(intptr_t incr) {
    /* Retired: should not be called anymore. */
    abort();
    return NULL;
}

/* 
 * Fake 'end' symbol for compatibility if anything still expects it.
 * But we've removed blok.c so it shouldn't be needed.
 */
char *s3sh_end_ptr;
