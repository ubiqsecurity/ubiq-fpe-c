#ifndef UBIQ_FPE_INTERNAL_FFX_H
#define UBIQ_FPE_INTERNAL_FFX_H

#include <sys/cdefs.h>

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <ubiq/fpe/internal/bn.h>

__BEGIN_DECLS

uint8_t * ffx_revb(uint8_t * const dst,
                   const uint8_t * const src, const size_t len);
static inline
char * ffx_revs(char * const dst, const char * const src)
{
    const size_t len = strlen(src);
    char * const res =
        (char *)ffx_revb((uint8_t *)dst, (const uint8_t *)src, len);
    dst[len] = '\0';
    return res;
}

int ffx_str(char * const str, const size_t len,
            const unsigned int m, const unsigned int r, const bigint_t * n);

void * ffx_memxor(void * d,
                  const void * s1, const void * s2,
                  size_t len);

int ffx_prf(uint8_t * const dst,
            const uint8_t * const K, const size_t k,
            const uint8_t * const src, const size_t len);

int ffx_ciph(uint8_t * const dst,
             const uint8_t * const K, const size_t k,
             const uint8_t * const src);

__END_DECLS

#endif
