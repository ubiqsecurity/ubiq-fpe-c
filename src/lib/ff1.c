#include <ubiq/fpe/ff1.h>
#include <ubiq/fpe/internal/ffx.h>

#include <arpa/inet.h>
#include <stdlib.h>
#include <math.h>

static
int ff1_cipher(char * const Y,
               const uint8_t * const K, const size_t k,
               const uint8_t * const T, const size_t t,
               const char * const X, const unsigned int radix,
               const int encrypt)
{
    /* Step 1 */
    const unsigned int n = strlen(X);
    const unsigned int u = n / 2, v = n - u;

    /* Step 3, 4 */
    const unsigned int b = ((unsigned int)ceil(log2(radix) * v) + 7) / 8;
    const unsigned int d = 4 * ((b + 3) / 4) + 4;

    const unsigned int p = 16;
    const unsigned int q = ((t + b + 1 + 15) / 16) * 16;
    const unsigned int z = q - (t + b + 1);
    const unsigned int r = ((d + 15) / 16) * 16;

    struct {
        void * buf;
        size_t len;
    } scratch;

    char * A, * B, * C;
    uint8_t * P, * Q, * R;

    bigint_t y, c;

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
    P[3] = 0;
    P[4] = radix >> 8;
    P[5] = radix;
    P[6] = 10;
    P[7] = u;
    *(uint32_t *)&P[8]  = htonl(n);
    *(uint32_t *)&P[12] = htonl(t);

    /* Step 6i, partial */
    memcpy(Q, T, t);
    memset(Q + t, 0, z);

    for (unsigned int i = 0; i < 10; i++) {
        /* Step 6v */
        const unsigned int m = ((i + !!encrypt) % 2) ? u : v;

        uint8_t * numb;
        size_t numc;

        /* Step 6i, partial */
        bigint_set_str(&c, B, radix);
        numb = bigint_export(&c, &numc);
        Q[t + z] = encrypt ? i : (9 - i);
        if (b <= numc) {
            memcpy(&Q[t + z + 1], numb, b);
        } else {
            memset(&Q[t + z + 1], 0, b - numc);
            memcpy(&Q[t + z + 1 + (b - numc)], numb, numc);
        }
        free(numb);

        /* Step 6ii */
        ffx_prf(R, K, k, P, p + q);

        /* Step 6iii */
        for (unsigned int j = 16; j < r; j += 16) {
            memset(&R[j], 0, 16 - sizeof(j));
            *(unsigned int *)&R[j + 16 - sizeof(j)] = htonl(j);

            ffx_memxor(&R[j], &R[0], &R[j], 16);

            ffx_ciph(&R[j], K, k, &R[j]);
        }

        /* Step 6iv */
        bigint_import(&y, R, d);

        /* Step 6vi */
        bigint_set_str(&c, A, radix);
        if (encrypt) {
            bigint_add(&c, &c, &y);
        } else {
            bigint_sub(&c, &c, &y);
        }
        bigint_set_ui(&y, radix);
        bigint_pow_ui(&y, &y, m);
        bigint_mod(&c, &c, &y);

        /* Step 6vii */
        ffx_str(C, v + 2, m, radix, &c);

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

int ff1_encrypt(char * const Y,
                const uint8_t * const K, const size_t k,
                const uint8_t * const T, const size_t t,
                const char * const X, const unsigned int radix)
{
    return ff1_cipher(Y, K, k, T, t, X, radix, 1);
}

int ff1_decrypt(char * const Y,
                const uint8_t * const K, const size_t k,
                const uint8_t * const T, const size_t t,
                const char * const X, const unsigned int radix)
{
    return ff1_cipher(Y, K, k, T, t, X, radix, 0);
}
