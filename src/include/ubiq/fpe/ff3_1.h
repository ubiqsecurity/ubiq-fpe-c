#ifndef UBIQ_FPE_FF3_1_H
#define UBIQ_FPE_FF3_1_H

#include <sys/cdefs.h>
#include <stdint.h>
#include <stddef.h>

__BEGIN_DECLS

struct ff3_1_ctx;

int ff3_1_ctx_create(struct ff3_1_ctx ** const ctx,
                     const uint8_t * const keybuf, const size_t keylen,
                     const uint8_t * const twkbuf,
                     const unsigned int radix);

int ff3_1_encrypt(struct ff3_1_ctx * const ctx,
                  char * const Y,
                  const char * const X, const uint8_t * const T);
int ff3_1_decrypt(struct ff3_1_ctx * const ctx,
                  char * const Y,
                  const char * const X, const uint8_t * const T);

void ff3_1_ctx_destroy(struct ff3_1_ctx * const ctx);

__END_DECLS

#endif
