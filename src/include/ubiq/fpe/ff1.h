#ifndef UBIQ_FPE_FF1_H
#define UBIQ_FPE_FF1_H

#include <sys/cdefs.h>
#include <stdint.h>

__BEGIN_DECLS

int ff1_encrypt(char * const Y,
                const uint8_t * const K, const size_t k,
                const uint8_t * const T, const size_t t,
                const char * const X, const unsigned int radix);
int ff1_decrypt(char * const Y,
                const uint8_t * const K, const size_t k,
                const uint8_t * const T, const size_t t,
                const char * const X, const unsigned int radix);

__END_DECLS

#endif
