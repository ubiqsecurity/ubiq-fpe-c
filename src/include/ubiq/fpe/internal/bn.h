#ifndef UBIQFPE_INTERNAL_BN_H
#define UBIQFPE_INTERNAL_BN_H

#include <sys/cdefs.h>
#include <errno.h>

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
    mpz_clear(*x);
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
int bigint_set_str(bigint_t * const x,
                   const char * const str, const unsigned int radix)
{
    return mpz_set_str(*x, str, radix);
}

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

__END_DECLS

#endif
