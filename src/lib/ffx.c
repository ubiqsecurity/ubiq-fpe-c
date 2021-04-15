#include <ubiq/fpe/internal/ffx.h>

#include <math.h>

int ffx_ctx_create(void ** const _ctx,
                   const size_t len, const size_t off,
                   const uint8_t * const keybuf, const size_t keylen,
                   const uint8_t * const twkbuf, const size_t twklen,
                   const size_t maxtxtlen,
                   const size_t mintwklen, const size_t maxtwklen,
                   const unsigned int radix)
{
    struct ffx_ctx * ctx;
    const EVP_CIPHER * ciph;
    size_t mintxtlen;

    switch (keylen) {
    case 16: ciph = EVP_aes_128_cbc(); break;
    case 24: ciph = EVP_aes_192_cbc(); break;
    case 32: ciph = EVP_aes_256_cbc(); break;
    default: ciph = NULL;              break;
    }
    if (!ciph) {
        return -EINVAL;
    }

    if (radix < 2 || radix > 36) {
        return -EINVAL;
    }

    /*
     * for both ff1 and ff3-1: radix**minlen >= 1000000
     *
     * therefore:
     *   minlen = ceil(log_radix(1000000))
     *
     *   = ceil(log_10(1000000) / log_10(radix))
     *   = ceil(6 / log_10(radix))
     */
    mintxtlen = ceil((double)6 / log10(radix));
    if (mintxtlen < 2 || mintxtlen > maxtxtlen) {
        return -EOVERFLOW;
    }

    if (mintwklen > maxtwklen ||
        twklen < mintwklen ||
        (maxtwklen > 0 && twklen > maxtwklen)) {
        return -EINVAL;
    }

    *_ctx = malloc(len + keylen + twklen);
    if (*_ctx) {
        ctx = (void *)((uint8_t *)*_ctx + off);

        ctx->ciph = ciph;
        ctx->evp = EVP_CIPHER_CTX_new();
        if (ctx->evp) {
            ctx->radix = radix;

            ctx->txtlen.min = mintxtlen;
            ctx->txtlen.max = maxtxtlen;

            ctx->twklen.min = mintwklen;
            ctx->twklen.max = maxtwklen;

            ctx->key.buf = (uint8_t *)(ctx + 1);
            ctx->key.len = keylen;
            memcpy(ctx->key.buf, keybuf, keylen);

            ctx->twk.buf = ctx->key.buf + keylen;
            ctx->twk.len = twklen;
            memcpy(ctx->twk.buf, twkbuf, twklen);
        } else {
            free(*_ctx);
            return -ENOMEM;
        }
    }

    return 0;
}

void ffx_ctx_destroy(void * const _ctx, const size_t off)
{
    struct ffx_ctx * const ctx = (void *)((uint8_t *)_ctx + off);

    memset(ctx->key.buf, 0, ctx->key.len);
    EVP_CIPHER_CTX_free(ctx->evp);

    free(_ctx);
}

uint8_t * ffx_revb(uint8_t * const dst,
                   const uint8_t * const src, const size_t len)
{
    size_t i;

    for (i = 0; i < len / 2; i++) {
        const uint8_t t = src[i];
        dst[i] = src[(len - 1) - i];
        dst[(len - 1)- i] = t;
    }

    if (len % 2) {
        dst[i] = src[i];
    }

    return dst;
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

int ffx_prf(struct ffx_ctx * const ctx,
            uint8_t * const dst,
            const uint8_t * const src, const size_t len)
{
    static const uint8_t IV[16] = { 0 };

    int dstl;

    if (len % 16) {
        return -EINVAL;
    }

    EVP_CIPHER_CTX_reset(ctx->evp);
    EVP_EncryptInit_ex(ctx->evp, ctx->ciph, NULL, ctx->key.buf, IV);
    EVP_CIPHER_CTX_set_padding(ctx->evp, 0);
    for (unsigned int i = 0; i < len; i += 16) {
        EVP_EncryptUpdate(ctx->evp, dst, &dstl, &src[i], 16);
    }
    EVP_EncryptFinal_ex(ctx->evp, dst, &dstl);

    return 0;
}

int ffx_ciph(struct ffx_ctx * const ctx,
             uint8_t * const dst, const uint8_t * const src)
{
    return ffx_prf(ctx, dst, src, 16);
}
