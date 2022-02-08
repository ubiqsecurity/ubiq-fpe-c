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
void bigint_set_ui(bigint_t * const x, const unsigned int n)
{
    mpz_set_ui(*x, n);
}

int __bigint_set_str(bigint_t * const x,
                     const char * const str, const char * const alpha);

int __u32_bigint_set_str(bigint_t * const x,
                    const uint32_t * const str, const uint32_t * const alpha);

static inline
int bigint_set_str(bigint_t * const x,
                   const char * const str, const unsigned int radix)
{
    return mpz_set_str(*x, str, radix);
}

int __bigint_get_str(char * const str, const size_t len,
                     const char * const alpha, const bigint_t * const x);

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

__END_DECLS

#endif
