#include <ubiq/fpe/internal/bn.h>
#include <ubiq/fpe/internal/ffx.h>

#include <string.h>
#include <unistr.h>
#include <uniwidth.h>
#include <wchar.h>

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
    int debug = 0;
    const char * csu = "__u32_bigint_get_str";
    const int rad = u32_strlen(alpha);

    (debug) && printf("%s len(%d) rad(%d) alpha(%S) \n", csu, len, rad, alpha);
   
   if (str == NULL) {
       return -EINVAL;
   }

    bigint_t x;
    int i;
    int err = 0;

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
     wprintf(L"__u32_bigint_get_str str(%S)\n",str);
    if (i <= len) {
        /*
         * to simplify conversion from a numbers to a string,
         * the output digits are stored in reverse order.
         * reverse the final value so that the output is correct
         */
        ffx_revu32(str, str, i);
    } else {
        err = -ENOMEM;
    }

    bigint_deinit(&x);

    return err;
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
            err = __bigint_set_str_radix(x, mapped, rad);
            (debug) && gmp_printf("%s x %Zd\n", csu, x);
        }

    }
    free(mapped);

  (debug) && printf("END DEBUG %s err(%d)\n\n", csu, err);
  return err;
}

int __bigint_set_str_radix(bigint_t * const x,
                     const char * const str, const size_t radix)
{
  static const char * csu = "__bigint_set_str_radix";
  int debug = 0;
  int err = 0;

  (debug) && printf("START DEBUG %s str(%s)   radix(%s)\n", csu, str, radix);


    if (radix <= 62) {
        // Assumption that the character set matches valid ranges for 2-62
        err = bigint_set_str(x, str, radix);
        (debug) && gmp_printf("%s x %Zd\n", csu, x);
    } else {
        const int len = strlen(str);

        // Alphabet is assumed to be \x01 - \xFF meaning the character integer value is related 
        // the index in the alpha string so we can simply skip that portion of the logic.


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
            size_t pos;

            /*
            * determine index/position in the alphabet.
            * if the character is not present the input
            * is not valid.
            */
            pos = ((uint8_t)str[len - 1 - i]); // values 1 - 255 since we don't support 0 (null terminator)

        /*
         * multiply the digit into the correct position
         * and add it to the result
         */
            bigint_mul_ui(&a, &m, pos - 1); // 0 - 254 (radix 255)
            bigint_add(x, x, &a);

            bigint_mul_ui(&m, &m, radix);
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
    const size_t rad = strlen(alpha);
    int err = 0;

    (debug) && printf("START DEBUG %s rad(%d)\n", csu, rad);


    (debug) && printf("DEBUG %s len(%d)\n", csu, len);
    (debug) && printf("DEBUG %s alpha(%s)\n", csu, alpha);
    err = __bigint_get_str_radix(str, len, rad, _x);
    (debug) && gmp_printf("__bigint_get_str   _x %Zd\n", _x);
    if (!err) {
        (debug) && printf("__bigint_get_str  str BEFORE(%s)\n", str);
        err = map_characters(str, str, get_standard_bignum_radix(rad), alpha);
        (debug) && printf("__bigint_get_str  str  AFTER(%s)\n", str);
    }

    (debug) && printf("DEBUG %s err(%d)\n", csu, err);
    (debug) && printf("DEBUG %s s(%s)\n", csu, str);
    return err;
}

int __bigint_get_str_radix(char * const str, const size_t len,
                     const size_t radix, const bigint_t * const _x)
{

  static const char * csu = "__bigint_get_str";
  int debug = 0;
  int err = 0;

  (debug) && printf("START DEBUG %s rad(%d)\n", csu, radix);

  if (radix <= 62) {

    err = bigint_get_str(str, len, radix, _x);

  } else {
    const char * const alpha = get_standard_bignum_radix(radix);
    bigint_t x;
    int i = 0;
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

    while (bigint_cmp_si(&x, 0) != 0) {
        int r;

        bigint_div_ui(&x, &r, &x, radix);
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
    (debug) && printf("DEBUG %s ret(%d)\n", csu, i);
    if (!err && i > len) {
        err = -ENOMEM;
    }
  }
  (debug) && printf("END DEBUG %s err(%d) str(%s) \n\n", csu, err, str);
  return err;

}

/* dst already exists and has null terminator */

int map_characters(char * const dst, const char * const src,
    const char * const src_chars,
    const char * const dst_chars) 
{
    const char * csu = "map_characters";
    int debug = 0;

    (debug) && printf("%s src(%s) strlen(%d) src_chars (%s) len(%d) dst_chars(%s)\n", csu, src, strlen(src), src_chars, strlen(src_chars), dst_chars);

    size_t len = strlen(src);
    for (int i = 0; i < len; i++) {
        char * pos = strchr(src_chars, src[i]);
        (debug) && printf("%s %d %c\n", csu, i, *pos);
        if (!pos) {
            (debug) && printf("Unable to find %c \n", src[i]);
            return -EINVAL;
        }
        (debug) && printf("%s %d %c\n", csu, i, *pos);
        dst[i] = dst_chars[pos - src_chars];
    }
    (debug) && printf("%s dst(%s)\n", csu, dst);
    return 0;
}

int map_characters_from_u32(char * const dst, const uint8_t * const src,
    const uint32_t * const src_chars,
    const char * const dst_chars) 
{
    int debug = 0;
    uint32_t * tmp = NULL;
    size_t src_len = 0;

    (debug) && printf("src_chars (%S) len(%d)\n", src_chars, u32_strlen(src_chars));
    tmp = u8_to_u32(src, u8_strlen(src) + 1, NULL, &src_len);
    (debug) && printf("tmp (%S) len(%d) src_len(%d)\n", tmp, u32_strlen(tmp), src_len);

//    size_t len = strlen(src);
    for (int i = 0; i < src_len - 1; i++) {
        uint32_t * pos = u32_strchr(src_chars, tmp[i]);
        if (!pos) {
            (debug) && printf("Unable to find %c \n", src[i]);
            return -EINVAL;
        }
        dst[i] = dst_chars[pos - src_chars];
    }
//    dst[src_len] = '\0';
    free(tmp);
    return 0;
}

int map_characters_to_u32(uint8_t * const dst, const char * const src,
    const char * const src_chars,
    const uint32_t * const dst_chars) 
{
    int res = 0;
    int debug = 0;
    size_t src_len = strlen(src);
    uint32_t * tmp = calloc(src_len + 1, sizeof(uint32_t));

    (debug) && printf("dst_chars (%S) len(%d)\n", dst_chars, u32_strlen(dst_chars));
    // (debug) && printf("src_chars (%S) len(%d)\n", src_chars, src_len);
//    tmp = u8_to_u32(src, u8_strlen(src) + 1, NULL, &src_len);
    // (debug) && printf("tmp (%S) len(%d) src_len(%d)\n", tmp, u32_strlen(tmp), src_len);

//    size_t len = strlen(src);
    for (int i = 0; i < src_len ; i++) {
        char * pos = strchr(src_chars, src[i]);
        if (!pos) {
            (debug) && printf("Unable to find %c \n", src[i]);
            return -EINVAL;
        }
        tmp[i] = dst_chars[pos - src_chars];
    }
    uint8_t * x = u32_to_u8(tmp, src_len + 1, NULL, &src_len);
    if (x) {
        strcpy(dst, x);
        free(x);
    } else {
        res = -ENOMEM;
    }
//    dst[src_len] = '\0';
    // (debug) && printf("dst (%S) tmp(%S) src_len(%d)\n", dst, tmp, src_len);
    free(tmp);
    return res;
}

const char * get_standard_bignum_radix(
    const size_t radix) {
    static const char radix10[] = "0123456789";
    static const char radix36[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    static const char radix62[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    static const char radix255[] =     "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f" 
                                   "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f"
                                   "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f"
                                   "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f"
                                   "\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f"
                                   "\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f"
                                   "\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f"
                                   "\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e\x7f"
                                   "\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f"
                                   "\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f"
                                   "\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf"
                                   "\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf"
                                   "\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf"
                                   "\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf"
                                   "\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef"
                                   "\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff";

    if (radix <= 10) {
        return radix10;
    }
    if (radix <= 36) {
        return radix36;
    }
    if (radix <= 62) {
        return radix62;
    }
    return radix255;
}
