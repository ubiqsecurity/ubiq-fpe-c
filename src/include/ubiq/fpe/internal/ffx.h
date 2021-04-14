#ifndef UBIQFPE_INTERNAL_FFX_H
#define UBIQFPE_INTERNAL_FFX_H

#include <sys/cdefs.h>

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <ubiq/fpe/internal/bn.h>

__BEGIN_DECLS

uint8_t * ffx_revb(uint8_t * const str, const size_t len);
static inline
char * ffx_revs(char * const str)
{
    return (char *)ffx_revb((uint8_t *)str, strlen(str));
}

static inline
int ffx_nums(uint8_t ** const n, size_t * const nb,
             const char * const str, const unsigned int radix)
{
    bigint_t x;
    int res;
    bigint_init(&x);
    res = bigint_set_str(&x, str, radix);
    if (res == 0) {
        *n = (uint8_t *)bigint_export(&x, nb);
    }
    bigint_deinit(&x);
    return res;
}


int ffx_str(char * const str, const unsigned int m, const unsigned int r,
            const bigint_t * n);

__END_DECLS

#endif
