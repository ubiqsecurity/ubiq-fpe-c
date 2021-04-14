#include <ubiq/fpe/internal/ffx.h>

#include <arpa/inet.h>
#include <stdlib.h>
#include <math.h>

#include <openssl/evp.h>

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

static
int ff1_prf(uint8_t * const dst,
            const uint8_t * const K, const size_t k,
            const uint8_t * const src, const size_t len)
{
    const EVP_CIPHER * ciph;

    int res;

    res = -EINVAL;
    switch (k) {
    case 16: ciph = EVP_aes_128_cbc(); break;
    case 24: ciph = EVP_aes_192_cbc(); break;
    case 32: ciph = EVP_aes_256_cbc(); break;
    default: ciph = NULL;              break;
    }

    if (ciph) {
        static const uint8_t IV[16] = { 0 };

        EVP_CIPHER_CTX * const ctx = EVP_CIPHER_CTX_new();
        int dstl;

        EVP_EncryptInit_ex(ctx, ciph, NULL, K, IV);
        EVP_CIPHER_CTX_set_padding(ctx, 0);
        for (unsigned int i = 0; i < len / 16; i++) {
            EVP_EncryptUpdate(ctx, dst, &dstl, src + 16 * i, 16);
        }
        EVP_EncryptFinal_ex(ctx, dst, &dstl);
        EVP_CIPHER_CTX_free(ctx);
    }

    return res;
}

static
int ff1_ciph(uint8_t * const dst,
             const uint8_t * const K, const size_t k,
             const uint8_t * const src)
{
    ff1_prf(dst, K, k, src, 16);
}

int ff1_encrypt(char * const Y,
                const uint8_t * const K, const size_t k,
                const uint8_t * const T, const size_t t,
                const char * const X, const unsigned int radix)
{
    const unsigned int n = strlen(X);
    const unsigned int u = n / 2, v = n - u;

    const unsigned int b = ((unsigned int)ceil(log2(radix) * v) + 7) / 8;
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

    bigint_t y, c;

    bigint_init(&y);
    bigint_init(&c);

    scratch.len = 3 * (v + 2) + p + q + r + ((((d + 15) / 16) - 1) * 16);
    scratch.buf = malloc(scratch.len);
    if (!scratch.buf) {
        bigint_deinit(&c);
        bigint_deinit(&y);
        return -ENOMEM;
    }

    /*
     * TODO: maybe P, Q, and R should be at the
     * front so that they can be 16-byte aligned
     */
    A = scratch.buf;
    B = A + v + 2;
    C = B + v + 2;
    P = C + v + 2;
    Q = P + p;
    R = Q + q;

    memcpy(A, X + 0, u); A[u] = '\0';
    memcpy(B, X + u, v); B[v] = '\0';

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

    for (unsigned int i = 0; i < 10; i++) {
        const unsigned int m = (i % 2) ? v : u;

        char * tmp;
        uint8_t * numb;
        size_t numc;

        bigint_set_str(&c, B, radix);
        numb = bigint_export(&c, &numc);

        memcpy(Q, T, t);
        memset(Q + t, 0, z);
        Q[t + z] = i;
        if (b <= numc) {
            memcpy(&Q[t + z + 1], numb, b);
        } else {
            memset(&Q[t + z + 1], 0, b - numc);
            memcpy(&Q[t + z + 1 + (b - numc)], numb, numc);
        }

        free(numb);

        ff1_prf(R, K, k, P, p + q);

        for (unsigned int j = 1; j < (d + 15) / 16; j++) {
            memset(&R[16 * j], 0, 16 - sizeof(j));
            *(unsigned int *)&R[16 * j + 16 - sizeof(j)] = htonl(j);

            memxor(&R[16 * j], &R[0], &R[16 * j], 16);

            ff1_ciph(&R[16 * j], K, k, &R[16 * j]);
        }

        bigint_import(&y, R, d);
        bigint_set_str(&c, A, radix);
        bigint_add(&c, &c, &y);
        bigint_set_ui(&y, radix);
        bigint_pow_ui(&y, &y, m);
        bigint_mod(&c, &c, &y);

        ffx_str(C, v + 2, m, radix, &c);

        tmp = A;
        A = B;
        B = C;
        C = tmp;
    }

    strcpy(Y, A);
    strcat(Y, B);

bailout:
    memset(scratch.buf, 0, scratch.len);
    free(scratch.buf);

    bigint_deinit(&c);
    bigint_deinit(&y);
}
