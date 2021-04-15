#ifndef UBIQ_FPE_FF3_1_H
#define UBIQ_FPE_FF3_1_H

#include <sys/cdefs.h>
#include <stdint.h>
#include <stddef.h>

__BEGIN_DECLS

int ff3_1_encrypt(char * const Y,
                  const uint8_t * const K, const size_t k,
                  const uint8_t * const T,
                  const char * const X, const unsigned int radix);
int ff3_1_decrypt(char * const Y,
                  const uint8_t * const K, const size_t k,
                  const uint8_t * const T,
                  const char * const X, const unsigned int radix);

__END_DECLS

#endif
