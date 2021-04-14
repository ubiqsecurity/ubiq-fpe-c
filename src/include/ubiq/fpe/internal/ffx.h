#ifndef UBIQ_FPE_INTERNAL_FFX_H
#define UBIQ_FPE_INTERNAL_FFX_H

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

int ffx_str(char * const str, const size_t len,
            const unsigned int m, const unsigned int r, const bigint_t * n);

__END_DECLS

#endif
