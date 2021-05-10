#include <ubiq/fpe/internal/ffx.h>

#include <arpa/inet.h>
#include <stdlib.h>
#include <math.h>

struct ff3_1_ctx
{
    struct ffx_ctx ffx;
};

int ff3_1_ctx_create(struct ff3_1_ctx ** const ctx,
                     const uint8_t * const keybuf, const size_t keylen,
                     const uint8_t * const twkbuf,
                     const unsigned int radix)
{
    /*
     * maxlen for ff3-1:
     * = 2 * log_radix(2**96)
     * = 2 * log_radix(2**48 * 2**48)
     * = 2 * (log_radix(2**48) + log_radix(2**48))
     * = 2 * (2 * log_radix(2**48))
     * = 4 * log_radix(2**48)
     * = 4 * log2(2**48) / log2(radix)
     * = 4 * 48 / log2(radix)
     * = 192 / log2(radix)
     */
    const size_t maxtxtlen = (double)192 / log2(radix);
    int res;

    res = -EINVAL;
    if (twkbuf) {
        res = ffx_ctx_create(
            (void **)ctx,
            sizeof(struct ff3_1_ctx), offsetof(struct ff3_1_ctx, ffx),
            keybuf, keylen,
            twkbuf, 7,
            maxtxtlen,
            7, 7,
            radix);
        if (res == 0) {
            /* the key used in ff3-1 is the reverse of the given key */
            ffx_revb((*ctx)->ffx.key.buf,
                     (*ctx)->ffx.key.buf, (*ctx)->ffx.key.len);
        }
    }

    return res;
}

void ff3_1_ctx_destroy(struct ff3_1_ctx * const ctx)
{
    ffx_ctx_destroy((void *)ctx, offsetof(struct ff3_1_ctx, ffx));
}

/*
 * The comments below reference the steps of the algorithm described here:
 *
 * https://nvlpubs.nist.gov/nistpubs/SpecialPublications/NIST.SP.800-38Gr1-draft.pdf
 */
static
int ff3_1_cipher(struct ff3_1_ctx * const ctx,
                 char * const Y,
                 const char * const X,
                 const uint8_t * T /* T is always 56 bits */,
                 const int encrypt)
{
    /* Step 1 */
    const unsigned int n = strlen(X);
    const unsigned int v = n / 2, u = n - v;

    struct {
        void * buf;
        size_t len;
    } scratch;

    uint8_t P[16];
    uint8_t Tw[2][4];

    char * A, * B, * C;

    bigint_t y, c;

    /* use the default tweak if none is given */
    if (!T) {
        T = ctx->ffx.twk.buf;
    }

    /* check the text length */
    if (n < ctx->ffx.txtlen.min ||
        n > ctx->ffx.txtlen.max) {
        return -EINVAL;
    }

    bigint_init(&y);
    bigint_init(&c);

    scratch.len = 3 * (u + 2);
    scratch.buf = malloc(scratch.len);
    if (!scratch.buf) {
        bigint_deinit(&c);
        bigint_deinit(&y);
        return -ENOMEM;
    }

    A = scratch.buf;
    B = A + u + 2;
    C = B + u + 2;

    /* Step 2 */
    if (encrypt) {
        memcpy(A, X + 0, u); A[u] = '\0';
        memcpy(B, X + u, v); B[v] = '\0';
    } else {
        memcpy(B, X + 0, u); B[u] = '\0';
        memcpy(A, X + u, v); A[v] = '\0';
    }

    /* Step 3 */
    memcpy(&Tw[0][0], &T[0], 3);
    Tw[0][3] = T[3] & 0xf0;

    memcpy(&Tw[1][0], &T[4], 3);
    Tw[1][3] = (T[3] & 0x0f) << 4;

    for (unsigned int i = 0; i < 8; i++) {
        /* Step 4i */
        const uint8_t * const W = Tw[(i + !!encrypt) % 2];
        const unsigned int m = ((i + !!encrypt) % 2) ? u : v;

        uint8_t * numb;
        size_t numc;

        /* Step 4ii */
        /* W ^ i */
        memcpy(P, W, 4);
        P[3] ^= encrypt ? i : (7 - i);
        /*
         * reverse @B and convert numeral string to integer
         * which is then exported to a number as a byte array
         */
        ffx_revs(C, B);
        bigint_set_str(&c, C, ctx->ffx.radix);
        numb = bigint_export(&c, &numc);
        if (12 <= numc) {
            memcpy(&P[4], numb, 12);
        } else {
            /* zero pad on left, if necessary */
            memset(&P[4], 0, 12 - numc);
            memcpy(&P[4 + (12 - numc)], numb, numc);
        }
        free(numb);

        /* Step 4iii */
        ffx_revb(P, P, sizeof(P));
        ffx_ciph(&ctx->ffx, P, P);
        ffx_revb(P, P, sizeof(P));

        /* Step 4iv */
        bigint_import(&y, P, sizeof(P));

        /* Step 4v */
        /*
         * set c to the reversal of A converted
         * to an integer under the radix
         */
        ffx_revs(C, A);
        bigint_set_str(&c, C, ctx->ffx.radix);
        /* c = rev(A) +/- y */
        if (encrypt) {
            bigint_add(&c, &c, &y);
        } else {
            bigint_sub(&c, &c, &y);
        }
        /* now set @y to radix**m */
        bigint_set_ui(&y, ctx->ffx.radix);
        bigint_pow_ui(&y, &y, m);
        /* c = (rev(A) +/- y) mod radix**m */
        bigint_mod(&c, &c, &y);

        /* Step 4vi */
        ffx_str(C, u + 2, m, ctx->ffx.radix, &c);
        ffx_revs(C, C);

        {
            char * const tmp = A;
            /* Step 4vii */
            A = B;
            /* Step 4viii */
            B = C;
            C = tmp;
        }
    }

    /* Step 5 */
    if (encrypt) {
        strcpy(Y, A);
        strcat(Y, B);
    } else {
        strcpy(Y, B);
        strcat(Y, A);
    }

    memset(scratch.buf, 0, scratch.len);
    free(scratch.buf);
    memset(P, 0, sizeof(P));

    bigint_deinit(&c);
    bigint_deinit(&y);

    return 0;
}

int ff3_1_encrypt(struct ff3_1_ctx * const ctx,
                  char * const Y,
                  const char * const X,
                  const uint8_t * const T /* T is always 56 bits */)
{
    return ff3_1_cipher(ctx, Y, X, T, 1);
}

int ff3_1_decrypt(struct ff3_1_ctx * const ctx,
                  char * const Y,
                  const char * const X,
                  const uint8_t * const T /* T is always 56 bits */)
{
    return ff3_1_cipher(ctx, Y, X, T, 0);
}
