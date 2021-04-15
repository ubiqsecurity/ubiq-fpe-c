#ifndef UBIQ_FPE_FF1_H
#define UBIQ_FPE_FF1_H

#include <sys/cdefs.h>
#include <stdint.h>
#include <stddef.h>

__BEGIN_DECLS

struct ff1_ctx;

int ff1_ctx_create(struct ff1_ctx ** const ctx,
                   const uint8_t * const keybuf, const size_t keylen,
                   const uint8_t * const twkbuf, const size_t twklen,
                   const size_t mintwklen, const size_t maxtwklen,
                   const unsigned int radix);

int ff1_encrypt(struct ff1_ctx * const ctx,
                char * const Y,
                const char * const X,
                const uint8_t * const T, const size_t t);
int ff1_decrypt(struct ff1_ctx * const ctx,
                char * const Y,
                const char * const X,
                const uint8_t * const T, const size_t t);

void ff1_ctx_destroy(struct ff1_ctx * const ctx);

__END_DECLS

#endif
