#include <ubiq/fpe/ff1.h>
#include <ubiq/fpe/internal/ffx.h>

#include <arpa/inet.h>
#include <stdlib.h>
#include <math.h>

struct ff1_ctx
{
    struct ffx_ctx ffx;
};

int ff1_ctx_create(struct ff1_ctx ** const ctx,
                   const uint8_t * const keybuf, const size_t keylen,
                   const uint8_t * const twkbuf, const size_t twklen,
                   const size_t mintwklen, const size_t maxtwklen,
                   const unsigned int radix)
{
    /*
     * maxlen for ff1 is 2**32
     *
     * if size_t can't hold 2**32, then limit maxlen
     * to the maximum value that it *can* hold
     */
    const size_t maxtxtlen =
        sizeof(maxtxtlen) <= 4 ? SIZE_MAX : ((size_t)1 << 32);

    return ffx_ctx_create(
        (void **)ctx,
        sizeof(struct ff1_ctx), offsetof(struct ff1_ctx, ffx),
        keybuf, keylen,
        twkbuf, twklen,
        maxtxtlen,
        mintwklen, maxtwklen,
        radix);
}

void ff1_ctx_destroy(struct ff1_ctx * const ctx)
{
    ffx_ctx_destroy((void *)ctx, offsetof(struct ff1_ctx, ffx));
}

static
int ff1_cipher(struct ff1_ctx * const ctx,
               char * const Y,
               const char * const X,
               const uint8_t * T, size_t t,
               const int encrypt)
{
    /* Step 1 */
    const unsigned int n = strlen(X);
    const unsigned int u = n / 2, v = n - u;

    /* Step 3, 4 */
    const unsigned int b =
        ((unsigned int)ceil(log2(ctx->ffx.radix) * v) + 7) / 8;
    const unsigned int d = 4 * ((b + 3) / 4) + 4;

    const unsigned int p = 16;
    const unsigned int r = ((d + 15) / 16) * 16;

    struct {
        void * buf;
        size_t len;
    } scratch;

    char * A, * B, * C;
    uint8_t * P, * Q, * R;

    unsigned int q, z;

    bigint_t y, c;

    /* use the default tweak when none is supplied */
    if (t == 0) {
        T = ctx->ffx.twk.buf;
        t = ctx->ffx.twk.len;
    }

    /* check the text and tweak lengths */
    if (n < ctx->ffx.txtlen.min ||
        n > ctx->ffx.txtlen.max ||
        t < ctx->ffx.twklen.min ||
        (ctx->ffx.twklen.max > 0 &&
         t > ctx->ffx.twklen.max)) {
        return -EINVAL;
    }

    /* the number of bytes in Q */
    q = ((t + b + 1 + 15) / 16) * 16;
    /* the number of bytes of zero padding within Q */
    z = q - (t + b + 1);

    bigint_init(&y);
    bigint_init(&c);

    scratch.len = 3 * (v + 2) + p + q + r;
    scratch.buf = malloc(scratch.len);
    if (!scratch.buf) {
        bigint_deinit(&c);
        bigint_deinit(&y);
        return -ENOMEM;
    }

    /*
     * P, Q, and R at the front so that they are all 16-byte
     * aligned. P and Q must remain adjacent since they are
     * concatenated as part of the algorithm
     */
    P = scratch.buf;
    Q = P + p;
    R = Q + q;
    A = R + r;
    B = A + v + 2;
    C = B + v + 2;

    /* Step 2 */
    if (encrypt) {
        memcpy(A, X + 0, u); A[u] = '\0';
        memcpy(B, X + u, v); B[v] = '\0';
    } else {
        memcpy(B, X + 0, u); B[u] = '\0';
        memcpy(A, X + u, v); A[v] = '\0';
    }

    /* Step 5 */
    P[0] = 1;
    P[1] = 2;
    P[2] = 1;
    P[3] = ctx->ffx.radix >> 16;
    P[4] = ctx->ffx.radix >> 8;
    P[5] = ctx->ffx.radix;
    P[6] = 10;
    P[7] = u;
    *(uint32_t *)&P[8]  = htonl(n);
    *(uint32_t *)&P[12] = htonl(t);

    /*
     * Step 6i, partial
     * these parts of @Q are static
     */
    memcpy(Q, T, t);
    memset(Q + t, 0, z);

    for (unsigned int i = 0; i < 10; i++) {
        /* Step 6v */
        const unsigned int m = ((i + !!encrypt) % 2) ? u : v;

        uint8_t * numb;
        size_t numc;

        /* Step 6i, partial */
        Q[t + z] = encrypt ? i : (9 - i);

        /*
         * convert the numeral string indicated by @B
         * to an integer and then export that integer
         * as a byte array representation and store
         * it into @Q
         */
        bigint_set_str(&c, B, ctx->ffx.radix);
        numb = bigint_export(&c, &numc);
        if (b <= numc) {
            memcpy(&Q[t + z + 1], numb, b);
        } else {
            /* pad on the left with zeros, if needed */
            memset(&Q[t + z + 1], 0, b - numc);
            memcpy(&Q[t + z + 1 + (b - numc)], numb, numc);
        }
        free(numb);

        /* Step 6ii */
        ffx_prf(&ctx->ffx, R, P, p + q);

        /*
         * Step 6iii:
         * if r is greater than 16 (it will be a multiple of 16),
         * fill the 2nd and subsequent blocks with the result
         * of ciph(R ^ 1), ciph(R ^ 2), ...
         */
        for (unsigned int j = 1; j < r / 16; j++) {
            const unsigned int l = j * 16;

            memset(&R[l], 0, 16 - sizeof(j));
            *(unsigned int *)&R[l + (16 - sizeof(j))] = htonl(j);

            ffx_memxor(&R[l], &R[0], &R[l], 16);

            ffx_ciph(&ctx->ffx, &R[l], &R[l]);
        }

        /*
         * Step 6iv
         * create an integer from the first @d bytes in @R
         */
        bigint_import(&y, R, d);

        /* Step 6vi */
        /*
         * create an integer from @A in the given radix.
         * set @c to A +/- y
         */
        bigint_set_str(&c, A, ctx->ffx.radix);
        if (encrypt) {
            bigint_add(&c, &c, &y);
        } else {
            bigint_sub(&c, &c, &y);
        }
        /* set @y to radix**m */
        bigint_set_ui(&y, ctx->ffx.radix);
        bigint_pow_ui(&y, &y, m);
        /* c = (A +/- y) mod radix**m */
        bigint_mod(&c, &c, &y);

        /* Step 6vii */
        ffx_str(C, v + 2, m, ctx->ffx.radix, &c);

        {
            char * const tmp = A;
            /* Step 6viii */
            A = B;
            /* Step 6ix */
            B = C;
            C = tmp;
        }
    }

    /* Step 7 */
    if (encrypt) {
        strcpy(Y, A);
        strcat(Y, B);
    } else {
        strcpy(Y, B);
        strcat(Y, A);
    }

    memset(scratch.buf, 0, scratch.len);
    free(scratch.buf);

    bigint_deinit(&c);
    bigint_deinit(&y);

    return 0;
}

int ff1_encrypt(struct ff1_ctx * const ctx,
                char * const Y,
                const char * const X,
                const uint8_t * const T, const size_t t)
{
    return ff1_cipher(ctx, Y, X, T, t, 1);
}

int ff1_decrypt(struct ff1_ctx * const ctx,
                char * const Y,
                const char * const X,
                const uint8_t * const T, const size_t t)
{
    return ff1_cipher(ctx, Y, X, T, t, 0);
}
