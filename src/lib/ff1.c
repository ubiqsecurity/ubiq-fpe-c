#include <ubiq/fpe/internal/ffx.h>

#include <stdlib.h>

#include <arpa/inet.h>

static
void * memxor(void * d,
              const void * s1, const void * s2,
              size_t len)
{
    while (len) {
        *(uint8_t *)d = *(uint8_t *)s1 ^ *(uint8_t *)s2;

        d = (uint8_t *)d + 1;

        s1 = (uint8_t *)s1 + 1;
        s2 = (uint8_t *)s2 + 1;

        len--;
    }
}

/*
 * TODO: must support in-place operation
 */
static
int ff1_ciph(uint8_t * const dst,
             const uint8_t * const K, const size_t k,
             const uint8_t * const src)
{
    memcpy(dst, src, 16);
}

static
int ff1_prf(uint8_t * const dst,
            const uint8_t * const K, const size_t k,
            const uint8_t * const src, const size_t len)
{
    memcpy(dst, src + len - 16, 16);
}

int ff1_encrypt(char * const Y,
                const uint8_t * const K, const size_t k,
                const uint8_t * const T, const size_t t,
                const char * const X, const unsigned int radix)
{
    const unsigned int n = strlen(X);
    const unsigned int u = n / 2, v = n - u;

    const unsigned int b =
        (((8 * sizeof(radix)) - __builtin_clz(radix)) + 7) / 8;
    const unsigned int d = 4 * ((b + 3) / 4) + 4;

    const unsigned int p = 16;
    const unsigned int q = ((t + b + 1 + 15) / 16) * 16;
    const unsigned int z = q - (t + b + 1);
    const unsigned int r = 16;

    struct {
        void * buf;
        size_t len;
    } scratch;

    char * A, * B, * C;
    uint8_t * P, * Q, * R;

    bigint_t num;

    bigint_init(&num);

    scratch.len = 3 * (v + 1) + p + q + r + ((((d + 15) / 16) - 1) * 16);
    scratch.buf = malloc(scratch.len);
    if (!scratch.buf) {
        bigint_deinit(&num);
        return -ENOMEM;
    }

    A = scratch.buf;
    B = A + v + 1;
    C = B + v + 1;
    P = C + v + 1;
    Q = P + p;
    R = Q + q;

    memcpy(A, X + 0, u); A[u] = '\0';
    memcpy(B, X + u, v); B[v] = '\0';

    P[0] = 1;
    P[1] = 0;
    P[2] = 1;
    P[3] = 0;
    P[4] = radix >> 8;
    P[5] = radix;
    P[6] = 10;
    P[7] = u;
    *(uint32_t *)&P[8]  = htonl(n);
    *(uint32_t *)&P[12] = htonl(t);

    for (unsigned int i = 0; i < 10; i++) {
        void * numb;
        size_t numc;

        ffx_numr(&num, A, radix);
        numb = bigint_export(&num, &numc);

        memcpy(Q, T, t);
        memset(Q + t, 0, z);
        Q[t + z] = i;
        if (b <= numc) {
            memcpy(&Q[t + z + 1], numb, b);
        } else {
            memset(&Q[t + z + 1], 0, b - numc);
            memcpy(&Q[t + z + 1 + (b - numc)], numb, numc);
        }

        ff1_prf(R, K, k, P, p + q);

        for (unsigned int j = 1; j < d; j++) {
            memset(&R[16 * j], 0, 16 - sizeof(j));
            *(unsigned int *)&R[16 * j + 16 - sizeof(j)] = htonl(j);

            memxor(&R[16 * j], &R[0], &R[16 * j], 16);

            ff1_ciph(&R[16 * j], K, k, &R[16 * j]);
        }
    }

    strcpy(Y, A);
    strcat(Y, B);

    memset(scratch.buf, 0, scratch.len);
    free(scratch.buf);

    bigint_deinit(&num);
}
