#include <ubiq/fpe/internal/bn.h>
#include <ubiq/fpe/internal/ffx.h>

#include <string.h>

/*
 * Convert a numerical value in a given alphabet to a numerical
 * value in a different one.
 *
 * An alphabet consists of single-byte symbols in which each
 * symbol represents the numerical value associated with its
 * index/position in the alphabet. for example, consider the
 * alphabet:
 *   !@#$%^&*(
 * In this alphabet ! occupies index 0 and is therefore
 * assigned that value. @ = 1, # = 2, etc. Furthermore, the
 * alphabet contains 10 characters, so that becomes the radix
 * of the input. Using the alphabet above, an input of @$#
 * translates to a value of 243 (two hundred forty-three,
 * decimal).
 *
 * If the alphabet above were instead:
 *   !@#$%^&
 * The radix would be 8 and an input of @$# translates to a
 * value of 163 (one hundred sixty-three, decimal).
 */
int radix_convert(char * const out, const char * const oalpha,
                  const char * const inp, const char * const ialpha)
{
    const int strlen_inp = strlen(inp);

    bigint_t n, m, a;
    int i, rad, r;

    /* @n will be the numerical value of @inp */
    bigint_init(&n);
    bigint_set_ui(&n, 0);

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

    /*
     * the alphabet can be anything and doesn't have
     * to be in a recognized canonical order. the only
     * requirement is that every value in the list be
     * unique. checking that constraint is an expensive
     * undertaking, so it is assumed. as such, the radix
     * is simply the number of characters in the alphabet.
     */
    rad = strlen(ialpha);

    for (i = 0; i < strlen_inp; i++) {
        const char * pos;

        /*
         * determine index/position in the alphabet.
         * if the character is not present the input
         * is not valid.
         */
        pos = strchr(ialpha, inp[strlen_inp - 1 - i]);
        if (!pos) {
            bigint_deinit(&a);
            bigint_deinit(&m);
            bigint_deinit(&n);

            return -EINVAL;
        }

        /*
         * multiply the digit into the correct position
         * and add it to the result
         */
        bigint_mul_ui(&a, &m, pos - ialpha);
        bigint_add(&n, &n, &a);

        bigint_mul_ui(&m, &m, rad);
    }

    /* don't need these variables any more */
    bigint_deinit(&a);
    bigint_deinit(&m);

    /*
     * to convert the numerical value, repeatedly
     * divide (storing the result and the remainder)
     * @n by the desired radix of the output.
     *
     * the remainder is the current digit; the result
     * of the division becomes the input to the next
     * iteration
     */

    rad = strlen(oalpha);
    i = 0;

    while (bigint_cmp_si(&n, 0) != 0) {
        bigint_div_ui(&n, &r, &n, rad);
        out[i++] = oalpha[r];
    }

    /* handle the case where the initial value was 0 */
    if (!i) {
        out[i++] = oalpha[0];
    }
    out[i] = '\0';

    /*
     * to simplify conversion from a number to a string,
     * the output digits are stored in reverse order.
     * reverse the final value so that the output is correct
     */
    ffx_revs(out, out);

    bigint_deinit(&n);

    return 0;
}
