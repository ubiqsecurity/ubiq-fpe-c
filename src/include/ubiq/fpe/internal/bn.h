#ifndef UBIQ_FPE_INTERNAL_BN_H
#define UBIQ_FPE_INTERNAL_BN_H

#include <sys/cdefs.h>
#include <errno.h>
#include <stdint.h>

#include <gmp.h>

__BEGIN_DECLS

typedef mpz_t bigint_t;

static inline
void bigint_init(bigint_t * const x)
{
    mpz_init(*x);
}

static inline
void bigint_deinit(bigint_t * const x)
{
    mpz_set_ui(*x, 0);
    mpz_clear(*x);
}

static inline
void bigint_swap(bigint_t * const x, bigint_t * const y)
{
    mpz_swap(*x, *y);
}

static inline
void bigint_set_ui(bigint_t * const x, const unsigned int n)
{
    mpz_set_ui(*x, n);
}

int __u32_bigint_set_str(bigint_t * const x,
                    const uint32_t * const str, const uint32_t * const alpha);

static inline
int bigint_set_str(bigint_t * const x,
                   const char * const str, const unsigned int radix)
{
    return mpz_set_str(*x, str, radix);
}

int __bigint_set_str(bigint_t * const x,
                     const char * const str, const char * const alpha);

// Makes assumption about character set being used rad <= 10 (0-9), rad <= 36 (0-9a-z), rad <= 62 (0-9a-zA-Z), 63 < rad (\x01 - \xFF)
int __bigint_set_str_radix(bigint_t * const x,
                     const char * const str, const size_t radix);

int __u32_bigint_get_str(uint32_t * const str, const size_t len,
                    const uint32_t * const alpha, const bigint_t * const x);

static inline
int bigint_get_str(char * const str, const size_t len,
                   const unsigned int radix,
                   const bigint_t * const x)
{
    const unsigned int sib = mpz_sizeinbase(*x, radix);
    int res;

    res = -ENOMEM;
    if (len >= sib + 1 + !!(mpz_cmp_si(*x, 0) < 0)) {
        mpz_get_str(str, radix, *x);
        res = 0;
    }

    return res;
}

// Allow getting the string value of a big number where the string does not 
// conform to the standards for bignum, 0-9, a-z, A-Z.
// Returns 0 or -ENOMEM if len is not big enough.
// str and len MUST account for null terminator
int __bigint_get_str(char * const str, const size_t len,
                     const char * const alpha, const bigint_t * const x);

// Makes assumption about character set being used rad <= 10 (0-9), rad <= 36 (0-9a-z), rad <= 62 (0-9a-zA-Z), 63 < rad (\x01 - \xFF)
int __bigint_get_str_radix(char * const str, const size_t len,
                     const size_t radix, const bigint_t * const x);

static inline
void * bigint_export(const bigint_t * const x, size_t * const count)
{
    return mpz_export(NULL, count, 1, 1, 1, 0, *x);
}

static inline
void bigint_import(bigint_t * const x,
                   const void * const buf, const size_t len)
{
    mpz_import(*x, len, 1, 1, 1, 0, buf);
}

static inline
int bigint_cmp(const bigint_t * const x, const bigint_t * const y)
{
    return mpz_cmp(*x, *y);
}

static inline
int bigint_cmp_si(const bigint_t * const x, const int y)
{
    return mpz_cmp_si(*x, y);
}

static inline
void bigint_add(bigint_t * const res,
                const bigint_t * const top, const bigint_t * const btm)
{
    mpz_add(*res, *top, *btm);
}

static inline
void bigint_sub(bigint_t * const res,
                const bigint_t * const top, const bigint_t * const btm)
{
    mpz_sub(*res, *top, *btm);
}

static inline
void bigint_mul_ui(bigint_t * const res,
                   const bigint_t * const m1, const unsigned int m2)
{
    mpz_mul_ui(*res, *m1, m2);
}

static inline
void bigint_pow_ui(bigint_t * const res,
                   const bigint_t * const base, const unsigned int exp)
{
    mpz_pow_ui(*res, *base, exp);
}

static inline
void bigint_div_ui(bigint_t * const q, unsigned int * const r,
                   const bigint_t * const n, const unsigned int d)
{
    *r = mpz_tdiv_q_ui(*q, *n, d);
}

static inline
void bigint_mod(bigint_t * const res,
                const bigint_t * const num, const bigint_t * const den)
{
    mpz_mod(*res, *num, *den);
}

int map_characters(
    char * const dst, 
    const char * const src,
    const char * const src_chars,
    const char * const dst_chars);

int map_characters_from_u32(char * const dst, const uint8_t * const src,
    const uint32_t * const src_chars,
    const char * const dst_chars);

int map_characters_to_u32(uint8_t * const dst, const char * const src,
    const char * const src_chars,
    const uint32_t * const dst_chars) ;

const char * get_standard_bignum_radix(
    const size_t radix);


__END_DECLS

#endif
