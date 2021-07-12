#include <ubiq/fpe/internal/bn.h>
#include <ubiq/fpe/internal/ffx.h>

#include <string.h>

int __bigint_set_str(bigint_t * const x,
                     const char * const str, const char * const alpha)
{
    const int len = strlen(str);

    /*
     * the alphabet can be anything and doesn't have
     * to be in a recognized canonical order. the only
     * requirement is that every value in the list be
     * unique. checking that constraint is an expensive
     * undertaking, so it is assumed. as such, the radix
     * is simply the number of characters in the alphabet.
     */
    const int rad = strlen(alpha);

    bigint_t m, a;
    int i, err;

    /* @n will be the numerical value of @str */
    bigint_set_ui(x, 0);

    /*
     * @m is a multiplier used to multiply each digit
     * of the input into its correct position in @n
     */
    bigint_init(&m);
    bigint_set_ui(&m, 1);

    /*
     * @a is a temporary value used
     * to add each digit into @n
     */
    bigint_init(&a);

    err = 0;
    for (i = 0; i < len; i++) {
        const char * pos;

        /*
         * determine index/position in the alphabet.
         * if the character is not present the input
         * is not valid.
         */
        pos = strchr(alpha, str[len - 1 - i]);
        if (!pos) {
            err = -EINVAL;
            break;
        }

        /*
         * multiply the digit into the correct position
         * and add it to the result
         */
        bigint_mul_ui(&a, &m, pos - alpha);
        bigint_add(x, x, &a);

        bigint_mul_ui(&m, &m, rad);
    }

    bigint_deinit(&a);
    bigint_deinit(&m);

    return err;
}

int __bigint_get_str(char * const str, const size_t len,
                     const char * const alpha, const bigint_t * const _x)
{
    const int rad = strlen(alpha);

    bigint_t x;
    int i;

    /*
     * to convert the numerical value, repeatedly
     * divide (storing the result and the remainder)
     * @n by the desired radix of the output.
     *
     * the remainder is the current digit; the result
     * of the division becomes the input to the next
     * iteration
     */

    bigint_init(&x);
    bigint_mul_ui(&x, _x, 1);
    i = 0;

    while (bigint_cmp_si(&x, 0) != 0) {
        int r;

        bigint_div_ui(&x, &r, &x, rad);
        if (str && i < len) {
            str[i] = alpha[r];
        }
        i++;
    }

    /* handle the case where the initial value was 0 */
    if (!i) {
        if (str && i < len) {
            str[i] = alpha[0];
        }
        i++;
    }

    if (str && i <= len) {
        /*
         * to simplify conversion from a number to a string,
         * the output digits are stored in reverse order.
         * reverse the final value so that the output is correct
         */
        ffx_revb(str, str, i);
    }

    bigint_deinit(&x);

    return i;
}
