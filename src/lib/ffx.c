#include <ubiq/fpe/internal/ffx.h>

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
