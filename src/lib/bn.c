#include <ubiq/fpe/internal/bn.h>
#include <ubiq/fpe/internal/ffx.h>

#include <string.h>
#include <unistr.h>

/*
 * Convert a numerical value in a given alphabet to a number
 *
 * An alphabet consists of single-byte symbols in which each
 * symbol represents the numerical value associated with its
 * index/position in the alphabet. for example, consider the
 * alphabet:
 *   !@#$%^&*()
 * In this alphabet ! occupies index 0 and is therefore
 * assigned that value. @ = 1, # = 2, etc. Furthermore, the
 * alphabet contains 10 characters, so that becomes the radix
 * of the input. Using the alphabet above, an input of @$#
 * translates to a value of 132 (one hundred thirty-two,
 * decimal).
 *
 * If the alphabet above were instead:
 *   !@#$%^&*
 * The radix would be 8 and an input of @$# translates to a
 * value of 90 (ninety, decimal).
 *
 * The function returns 0 or a negative error number
 */
int __u32_bigint_set_str(bigint_t * const x,
                     const uint32_t * const str, const uint32_t * const alpha)
{
    const int len = u32_strlen(str);

    /*
     * the alphabet can be anything and doesn't have
     * to be in a recognized canonical order. the only
     * requirement is that every value in the list be
     * unique. checking that constraint is an expensive
     * undertaking, so it is assumed. as such, the radix
     * is simply the number of characters in the alphabet.
     */
    const int rad = u32_strlen(alpha);

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
        const uint32_t * pos;

        /*
         * determine index/position in the alphabet.
         * if the character is not present the input
         * is not valid.
         */
        pos = u32_strchr(alpha, str[len - 1 - i]);
        if (!pos) {
            err = -EINVAL;
            break;
        }

        /*
         * multiply the digit into the correct position
         * and add it to the result
         */
        if (pos != alpha) {
            if (pos - alpha == 1) {
                bigint_add(x, x, &m);
            } else {
                bigint_mul_ui(&a, &m, pos - alpha);
                bigint_add(x, x, &a);
            }
        }

        bigint_mul_ui(&m, &m, rad);
    }

    bigint_deinit(&a);
    bigint_deinit(&m);

    return err;
}

/*
 * This function requires str to be pre-allocated and LEN to be large enough
 * to include null terminator.
 * Will return 0 on success and < 0 if there is an error
 */
int __u32_bigint_get_str(uint32_t * const str, const size_t len,
                   const uint32_t * const alpha, const bigint_t * const _x)
{
    const int rad = u32_strlen(alpha);

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
        if (i < len) {
            str[i] = alpha[r];
        }
        i++;
    }

    /* handle the case where the initial value was 0 */
    if (!i) {
        if (i < len) {
            str[i] = alpha[0];
        }
        i++;
    }

    if (i <= len) {
        /*
         * to simplify conversion from a number to a string,
         * the output digits are stored in reverse order.
         * reverse the final value so that the output is correct
         */
        ffx_revu32(str, str, i);
    }

    bigint_deinit(&x);

    return i;
}

/*
 * Convert a numerical value in a given alphabet to a number
 *
 * An alphabet consists of single-byte symbols in which each
 * symbol represents the numerical value associated with its
 * index/position in the alphabet. for example, consider the
 * alphabet:
 *   !@#$%^&*()
 * In this alphabet ! occupies index 0 and is therefore
 * assigned that value. @ = 1, # = 2, etc. Furthermore, the
 * alphabet contains 10 characters, so that becomes the radix
 * of the input. Using the alphabet above, an input of @$#
 * translates to a value of 132 (one hundred thirty-two,
 * decimal).
 *
 * If the alphabet above were instead:
 *   !@#$%^&*
 * The radix would be 8 and an input of @$# translates to a
 * value of 90 (ninety, decimal).
 *
 * The function returns 0 or a negative error number
 */
int __bigint_set_str(bigint_t * const x,
                     const char * const str, const char * const alpha)
{
  static const char * csu = "__bigint_set_str";
  int debug = 0;
  int err = 0;

  (debug) && printf("START DEBUG %s str(%s)   alpha(%s)\n", csu, str, alpha);


    const size_t rad = strlen(alpha);
    if (rad <= 62) {
        // Cannot make any assumption about the alpha character set.  Assume that the 
        // character set does NOT match the expected bignum values.
        size_t len = strlen(str);
        char * mapped = calloc(len + 1, sizeof(char));
        if (mapped == NULL) {
            err = -ENOMEM;
        } else {
            mapped[len] = '\0';
            (debug) && printf("%s set_str (%s)\n", csu, str);
            err = map_characters(mapped, str, alpha, get_standard_bignum_radix(rad));
            (debug) && printf(" %s mapped (%s)\n", csu, mapped);
            if (!err) {
                err = bigint_set_str(x, mapped, rad);
                (debug) && gmp_printf("%s x %Zd\n", csu, x);
            }

        }
        free(mapped);
    } else {
    const int len = strlen(str);

    /*
    * the alphabet can be anything and doesn't have
    * to be in a recognized canonical order. the only
    * requirement is that every value in the list be
    * unique. checking that constraint is an expensive
    * undertaking, so it is assumed. as such, the radix
    * is simply the number of characters in the alphabet.
    */
    // const int rad = strlen(alpha);

    bigint_t m, a;
    int i;

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
  }
  (debug) && printf("END DEBUG %s err(%d)\n\n", csu, err);
  return err;
}


/*
 * This function returns the number of bytes (that would have been)
 * written to the output string (if the length of the available space
 * is sufficient). not including a nul terminator. A nul terminator
 * is never written. In short, success is determined by the return value
 * being less than or equal to @len.
 */
int __bigint_get_str(char * const str, const size_t len,
                     const char * const alpha, const bigint_t * const _x)
{

  static const char * csu = "__bigint_get_str";
  int debug = 0;
  int i = 0;
  const size_t rad = strlen(alpha);
  int err = 0;

  (debug) && printf("START DEBUG %s rad(%d)\n", csu, rad);

  if (rad <= 62) {

    (debug) && printf("DEBUG %s len(%d)\n", csu, len);
    (debug) && printf("DEBUG %s alpha(%s)\n", csu, alpha);
    err = bigint_get_str(str, len, rad, _x);
    (debug) && gmp_printf("__bigint_get_str   _x %Zd\n", _x);
    if (!err) {
      (debug) && printf("__bigint_get_str  str BEFORE(%s)\n", str);
      err = map_characters(str, str, get_standard_bignum_radix(rad), alpha);
      (debug) && printf("__bigint_get_str  str  AFTER(%s)\n", str);
    }

    (debug) && printf("DEBUG %s i(%d)\n", csu, i);
    (debug) && printf("DEBUG %s s(%s)\n", csu, str);
  } else {

    bigint_t x;

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
        if (i < len) {
            str[i] = alpha[r];
        }
        i++;
    }

    /* handle the case where the initial value was 0 */
    if (!i) {
        if (i < len) {
            str[i] = alpha[0];
        }
        i++;
    }
    // Make sure to set null terminator
    str[i] = '\0';
    if (i <= len) {
        /*
         * to simplify conversion from a number to a string,
         * the output digits are stored in reverse order.
         * reverse the final value so that the output is correct
         */
        ffx_revb(str, str, i);
    }
    (debug) && printf("DEBUG %s s(%s)\n", csu, str);

    bigint_deinit(&x);
  }
  (debug) && printf("DEBUG %s ret(%d)\n", csu, i);
  if (i > len) {
      err = -ENOMEM;
  }
  (debug) && printf("END DEBUG %s i(%d)  err(%d)\n\n", csu, i, err);
  return err;

}

/* dst already exists and has null terminator */

int map_characters(char * const dst, const char * const src,
    const char * const src_chars,
    const char * const dst_chars) 
{
    int debug = 0;

    (debug) && printf("src_chars (%s) len(%d)\n", src_chars, strlen(src_chars));

    size_t len = strlen(src);
    for (int i = 0; i < len; i++) {
        char * pos = strchr(src_chars, src[i]);
        if (!pos) {
            (debug) && printf("Unable to find %c \n", src[i]);
            return -EINVAL;
        }
        dst[i] = dst_chars[pos - src_chars];
    }
    return 0;
}

const char * get_standard_bignum_radix(
    const size_t radix) {
    static const char radix10[] = "0123456789";
    static const char radix36[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    static const char radix62[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    if (radix <= 10) {
        return radix10;
    }
    if (radix <= 36) {
        return radix36;
    }
    if (radix <= 62) {
        return radix62;
    }
    return NULL;
}
