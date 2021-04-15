#include <ubiq/fpe/internal/ffx.h>

#include <arpa/inet.h>
#include <stdlib.h>

static
int ff3_1_cipher(char * const Y,
                 const uint8_t * const K, const size_t k,
                 const uint8_t * const T /* T is always 56 bits */,
                 const char * const X, const unsigned int radix,
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
    uint8_t Tl[4], Tr[4];

    char * A, * B, * C;
    uint8_t * rK;

    bigint_t y, c;

    bigint_init(&y);
    bigint_init(&c);

    scratch.len = k + 3 * (u + 2);
    scratch.buf = malloc(scratch.len);
    if (!scratch.buf) {
        bigint_deinit(&c);
        bigint_deinit(&y);
        return -ENOMEM;
    }

    rK = scratch.buf;

    A = rK + k;
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
    memcpy(&Tl[0], &T[0], 3);
    Tl[3] = T[3] & 0xf0;

    memcpy(&Tr[0], &T[4], 3);
    Tr[3] = (T[3] & 0x0f) << 4;

    ffx_revb(rK, K, k);

    for (unsigned int i = 0; i < 8; i++) {
        /* Step 4i */
        const uint8_t * const W = ((i + !!encrypt) % 2) ? Tr : Tl;
        const unsigned int m = ((i + !!encrypt) % 2) ? u : v;

        uint8_t * numb;
        size_t numc;

        /* Step 4ii */
        memcpy(P, W, 4);
        *(uint32_t *)&P[3] ^= encrypt ? i : (7 - i);
        ffx_revs(C, B);
        bigint_set_str(&c, C, radix);
        numb = bigint_export(&c, &numc);
        if (12 <= numc) {
            memcpy(&P[4], numb, 12);
        } else {
            memset(&P[4], 0, 12 - numc);
            memcpy(&P[4 + (12 - numc)], numb, numc);
        }
        free(numb);

        /* Step 4iii */
        ffx_revb(P, P, sizeof(P));
        ffx_ciph(P, rK, k, P);
        ffx_revb(P, P, sizeof(P));

        /* Step 4iv */
        bigint_import(&y, P, sizeof(P));

        /* Step 4v */
        ffx_revs(C, A);
        bigint_set_str(&c, C, radix);
        if (encrypt) {
            bigint_add(&c, &c, &y);
        } else {
            bigint_sub(&c, &c, &y);
        }
        bigint_set_ui(&y, radix);
        bigint_pow_ui(&y, &y, m);
        bigint_mod(&c, &c, &y);

        /* Step 4vi */
        ffx_str(C, u + 2, m, radix, &c);
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

int ff3_1_encrypt(char * const Y,
                  const uint8_t * const K, const size_t k,
                  const uint8_t * const T /* T is always 56 bits */,
                  const char * const X, const unsigned int radix)
{
    return ff3_1_cipher(Y, K, k, T, X, radix, 1);
}

int ff3_1_decrypt(char * const Y,
                  const uint8_t * const K, const size_t k,
                  const uint8_t * const T,
                  const char * const X, const unsigned int radix)
{
    return ff3_1_cipher(Y, K, k, T, X, radix, 0);
}
