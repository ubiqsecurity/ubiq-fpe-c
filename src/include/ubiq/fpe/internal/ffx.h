#ifndef UBIQ_FPE_INTERNAL_FFX_H
#define UBIQ_FPE_INTERNAL_FFX_H

#include <sys/cdefs.h>

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <ubiq/fpe/internal/bn.h>

#include <openssl/evp.h>

__BEGIN_DECLS

uint8_t * ffx_revb(uint8_t * const dst,
                   const uint8_t * const src, const size_t len);

uint32_t * ffx_revu32(uint32_t * const dst,
                  const uint32_t * const src, const size_t len);

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

struct ffx_ctx
{
    const EVP_CIPHER * ciph;
    EVP_CIPHER_CTX * evp;

    unsigned int radix;
    struct {
        size_t min, max;
    } txtlen, twklen;
    struct {
        uint8_t * buf;
        size_t len;
    } twk;
};

int ffx_prf(struct ffx_ctx * const ctx,
            uint8_t * const dst, const uint8_t * const src, const size_t len);
int ffx_ciph(struct ffx_ctx * const ctx,
             uint8_t * const dst, const uint8_t * const src);

int ffx_ctx_create(void ** const _ctx,
                   const size_t len, const size_t off,
                   const uint8_t * const keybuf, const size_t keylen,
                   const uint8_t * const twkbuf, const size_t twklen,
                   const size_t maxtxtlen,
                   const size_t mintwklen, const size_t maxtwklen,
                   const unsigned int radix);
void ffx_ctx_destroy(void * const ctx, const size_t off);

__END_DECLS

#endif
