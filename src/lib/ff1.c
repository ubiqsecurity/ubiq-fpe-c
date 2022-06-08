#include <ubiq/fpe/ff1.h>
#include <ubiq/fpe/internal/ffx.h>

#include <arpa/inet.h>
#include <stdlib.h>
#include <math.h>
#include <unistr.h>

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

int ff1_ctx_create_custom_radix(struct ff1_ctx ** const ctx,
                   const uint8_t * const keybuf, const size_t keylen,
                   const uint8_t * const twkbuf, const size_t twklen,
                   const size_t mintwklen, const size_t maxtwklen,
                   const char * const custom_radix_str) 
{
    int res = 0;
    const size_t maxtxtlen =
        sizeof(maxtxtlen) <= 4 ? SIZE_MAX : ((size_t)1 << 32);

    // Test the radix string to determine if it matches natively supported 
    // radix lengths.  If so, simply use the standard radix length, not the
    // string.

    size_t radix_len = strlen(custom_radix_str);

    const char * bignum_radix_str = get_standard_bignum_radix(radix_len);

    if (bignum_radix_str != NULL && strncmp(bignum_radix_str, custom_radix_str, radix_len) == 0) {
        res = ffx_ctx_create(
        (void **)ctx,
        sizeof(struct ff1_ctx), offsetof(struct ff1_ctx, ffx),
        keybuf, keylen,
        twkbuf, twklen,
        maxtxtlen,
        mintwklen, maxtwklen,
        radix_len);
    } else {
        res = ffx_ctx_create_custom_radix_str(
        (void **)ctx,
        sizeof(struct ff1_ctx), offsetof(struct ff1_ctx, ffx),
        keybuf, keylen,
        twkbuf, twklen,
        maxtxtlen,
        mintwklen, maxtwklen,
        custom_radix_str);
    }
    return res;
}




void ff1_ctx_destroy(struct ff1_ctx * const ctx)
{
    ffx_ctx_destroy((void *)ctx, offsetof(struct ff1_ctx, ffx));
}

/*
 * The comments below reference the steps of the algorithm described here:
 *
 * https://nvlpubs.nist.gov/nistpubs/SpecialPublications/NIST.SP.800-38Gr1-draft.pdf
 */
static
int ff1_cipher(struct ff1_ctx * const ctx,
               char * const Y,
               const char * const _X,
               const uint8_t * T, size_t t,
               const int encrypt)
{

    // Input character set may be non-standard or even UTF8.

    // Since we know the radix and custom radix character sets,
    // We are going to map X to a standardized character set NOW, as long as radix is <= 255
    
    // We then convert back to the custom radix at the end.
    // If radix <= 62, we can use built in big number processing, otherwise we need to use the radix mapping string

    char * X = strdup(_X);

    // if (ctx->ffx.custom_radix_str) {
    //     X = calloc(strlen(_X) + 1, sizeof(char));
    //     map_characters(X, X, ctx->ffx.custom_radix_str, get_standard_bignum_radix(ctx->ffx.radix));
    // } else if (ctx->ffx.u32_custom_radix_str) {
    //     X = calloc(u8_mbsnlen(_X, strlen(_X) + 1), sizeof(char));
    //     map_characters_from_u32(X, _X, ctx->ffx.u32_custom_radix_str, get_standard_bignum_radix(ctx->ffx.radix));
    // }

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

    char * A, * B;
    uint8_t * P, * Q, * R;

    unsigned int q;

    bigint_t nA, nB, y, mU, mV;

    /* use the default tweak when none is supplied */
    if (T == NULL) {
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

    scratch.len = 2 * (v + 2) + p + q + r;
    scratch.buf = malloc(scratch.len);
    if (!scratch.buf) {
        return -ENOMEM;
    }

    bigint_init(&nA);
    bigint_init(&nB);
    bigint_init(&y);
    bigint_init(&mU);
    bigint_init(&mV);

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
    memset(Q + t, 0, q - (t + b + 1));

    /*
     * internally, we treat the string A and B as
     * big integers for the duration of the algorithm.
     * this speeds things up by avoiding having to
     * convert back and forth.
     * 
     * If the ctx was created with a custom radix string, then use is custom radix
     * information to convert to a bigint
     */
    if (ctx->ffx.custom_radix_str != NULL)  {
        __bigint_set_str(&nA, A, ctx->ffx.custom_radix_str);
        __bigint_set_str(&nB, B, ctx->ffx.custom_radix_str);
    } else {
        bigint_set_str(&nA, A, ctx->ffx.radix);
        bigint_set_str(&nB, B, ctx->ffx.radix);
    }

    /* calculate radix**u and radix**v for use in the loop */
    bigint_set_ui(&mU, ctx->ffx.radix);
    bigint_pow_ui(&mU, &mU, u);
    bigint_mul_ui(&mV, &mU, 1);
    if (u != v) {
        bigint_mul_ui(&mV, &mV, ctx->ffx.radix);
    }

    for (unsigned int i = 0; i < 10; i++) {
        /* Step 6v */
        bigint_t * const mX = ((i + !!encrypt) % 2) ? &mU : &mV;

        uint8_t * numb;
        size_t numc;

        /* Step 6i, partial */
        Q[q - b - 1] = encrypt ? i : (9 - i);

        /*
         * export the integer representing the string @B as
         * a byte array representation and store it into @Q
         */
        numb = bigint_export(&nB, &numc);
        if (b <= numc) {
            memcpy(&Q[q - b], numb, b);
        } else {
            /* pad on the left with zeros, if needed */
            memset(&Q[q - b], 0, b - numc);
            memcpy(&Q[q - numc], numb, numc);
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
            unsigned int * const rP =
                (unsigned int *)&R[16 - sizeof(unsigned int)];
            const unsigned int w = htonl(j);

            *rP ^= w;
            ffx_ciph(&ctx->ffx, &R[j * 16], &R[0]);
            *rP ^= w;
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
        if (encrypt) {
            bigint_add(&y, &nA, &y);
        } else {
            bigint_sub(&y, &nA, &y);
        }
        /* Step 6viii */
        bigint_swap(&nA, &nB);

        /* Step 6ix, skipped Step 6vii */
        /* c = (A +/- y) mod radix**m */
        bigint_mod(&nB, &y, mX);

        /*
         * the code above avoids converting the result
         * of the big integer math back to a string and
         * instead leaves it as a big integer. this
         * obviates the need for Step 6vii and also
         * allows us to combine the math with Step 6ix
         * and store the result of the big integer math
         * directly into nB as a big integer.
         */
    }

    /* convert the big integers back to strings */
    // Optimized out Step 7 but going directly back to the buffer Y.  Needed 
    // to change order of a couple operations due to null terminator 
    // when re-assembling data

    if (encrypt) {
        if (ctx->ffx.custom_radix_str != NULL)  {
            ffx_str_custom_radix(Y, v + 2, u, ctx->ffx.custom_radix_str, &nA);
            ffx_str_custom_radix(Y+u, v + 2, v, ctx->ffx.custom_radix_str, &nB);
        } else if (ctx->ffx.u32_custom_radix_str != NULL)  {
            ffx_str_u32_custom_radix(Y, v + 2, u, ctx->ffx.u32_custom_radix_str, &nA);
            ffx_str_u32_custom_radix(Y+u, v + 2, v, ctx->ffx.u32_custom_radix_str, &nB);
        } else {
            ffx_str(Y, v + 2, u, ctx->ffx.radix, &nA);
            ffx_str(Y+u, v + 2, v, ctx->ffx.radix, &nB);
        }
    } else {
        if (ctx->ffx.custom_radix_str != NULL)  {
            ffx_str_custom_radix(Y, v + 2, u, ctx->ffx.custom_radix_str, &nB);
            ffx_str_custom_radix(Y+u, v + 2, v, ctx->ffx.custom_radix_str, &nA);
        } else if (ctx->ffx.u32_custom_radix_str != NULL)  {
            ffx_str_u32_custom_radix(Y, v + 2, u, ctx->ffx.u32_custom_radix_str, &nB);
            ffx_str_u32_custom_radix(Y+u, v + 2, v, ctx->ffx.u32_custom_radix_str, &nA);
        } else {
            ffx_str(Y, v + 2, u, ctx->ffx.radix, &nB);
            ffx_str(Y+u, v + 2, v, ctx->ffx.radix, &nA);
        }
    }

    /* Step 7 */
    // strcpy(Y, A);
    // strcat(Y, B);

    memset(scratch.buf, 0, scratch.len);
    free(scratch.buf);

    bigint_deinit(&mV);
    bigint_deinit(&mU);
    bigint_deinit(&y);
    bigint_deinit(&nB);
    bigint_deinit(&nA);

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
