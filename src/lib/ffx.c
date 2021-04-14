#include <ubiq/fpe/internal/ffx.h>

#include <openssl/evp.h>

uint8_t * ffx_revb(uint8_t * const str, const size_t len)
{
    size_t i;

    for (i = 0; i < len / 2; i++) {
        const uint8_t t = str[i];
        str[i] = str[(len - 1) - i];
        str[(len - 1)- i] = t;
    }

    return str;
}

int ffx_str(char * const str, const size_t len,
            const unsigned int m, const unsigned int r,
            const bigint_t * const n)
{
    int res;

    res = -EINVAL;
    if (bigint_cmp_si(n, 0) >= 0) {
        res = bigint_get_str(str, len, r, n);
        if (res == 0) {
            const size_t len = strlen(str);

            if (len < m) {
                memmove(str + (m - len), str, len + 1);
                memset(str, '0', m - len);
            } else if (len > m) {
                res = -EOVERFLOW;
            }
        }
    }

    return res;
}

void * ffx_memxor(void * d,
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

    return d;
}

int ffx_prf(uint8_t * const dst,
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

        res = 0;
    }

    return res;
}

int ffx_ciph(uint8_t * const dst,
             const uint8_t * const K, const size_t k,
             const uint8_t * const src)
{
    return ffx_prf(dst, K, k, src, 16);
}
