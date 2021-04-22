# Format Preserving Encryption in C

An implementation of the NIST-approved FF1 and FF3-1 algorithms in C.

This implementation conforms (as best as possible) to
[Draft SP 800-38G Rev. 1][800-38g1]. The implementation passes all tests
specified by NIST in their Cryptographic Standards and Guidelines
[examples for FF1][ff1-examples]; however, no official examples/samples exist
(or are known) for FF3-1. FF3 is not implemented as NIST has officially
deprecated its use in light of recent [cryptanalysis][ff3-cryptanalysis]
performed on it.

# Building

The library is dependent on [OpenSSL](https://www.openssl.org/)--specifically
libcrypto--and [GMP](https://gmplib.org/). With those dependencies installed,
the library can be built using CMake:
```sh
$ cmake -S. -Bbuild
$ cmake --build build
```
The above commands will build the library as well as the unit tests.

# Testing

To run the tests:
```sh
$ cmake --build build --target test
```
As described above, the unit tests for FF1 come from the NIST guidelines. As
no such guidelines are available for FF3-1, the unit tests verify only that
the encryption and decryption implementations are compatible with each other.

Additionally, `valgrind` can be used to check for the existence of memory
leaks or other errors:
```sh
$ valgrind build/src/test/unittests
```

# Documentation

The interfaces are documented in the respective headers for
[FF1](src/include/ubiq/fpe/ff1.h) and [FF3](src/include/ubiq/fpe/ff3_1.h).

### About alphabets and the radix parameter

The interfaces operate on strings, and the radix parameter determines which
characters are valid within those strings, i.e. the alphabet. For example, if
your radix is 10, then the alphabet for your plain text consists of the
characters in the string "0123456789". If your radix is 16, then the
alphabet is the characters in the string "0123456789abcdef".

More concretely, if you want to encrypt, say, a 16 digit number grouped into
4 groups of 4 using a `-` as a delimiter as in `0123-4567-8901-2345`, then you
would need a radix of at least 11, and you would need to translate the `-`
character to an `a` (as that is the value that follows `9`) prior to the
encryption. Conversely, you would need to translate an `a` to a `-` after
decryption.

This mapping of user inputs to alphabets defined by the radix is not performed
by the library and must be done prior to calling the encrypt and after calling
the decrypt functions.

A radix of up to 36 is supported, and the alphabet for a radix of 36 is
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ".

### Tweaks

Tweaks are very much like Initialization Vectors (IVs) in "traditional"
encryption algorithms. For FF1, the minimun and maximum allowed lengths of
the tweak may be specified by the user, and any tweak length between those
values may be used. For FF3, the size of the tweak is fixed at 7 bytes.

### Input lengths

For both FF1 and FF3-1, the minimum length is determined by the inequality:
- radix<sup>minlen</sup> >= 1000000

or:
- minlen >= 6 / log<sub>10</sub> radix

Thus, the minimum length is determined by the radix and is automatically
calculated from it.

For FF1, the maximum input length is
- 2<sup>32</sup>

For FF3-1, the maximum input length is
- 2 * log<sub>radix</sub> 2<sup>96</sup>

or:
- 192 / log<sub>2</sub> radix

## Examples

The [unit test code](src/test) provides the best and simplest example of how to use the
interfaces.

### FF1
```c
    /*
     * @K is an array of bytes of length @k which must be 16, 24, or 32
     * @T is an array of bytes of length @t which must be between the minimum
     *     and maximum tweak lengths specified to @ff1_ctx_create. In this
     *     case, the values are specified as 0 and SIZE_MAX
     *
     * @radix and @PT are "user inputs"
     */

    struct ff1_ctx * ctx;
    char * out;
    int res;

    /* allocate space for the output */
    out = (char *)calloc(strlen(PT) + 1, 1);

    res = ff1_ctx_create(&ctx, K, k, T, t, 0, SIZE_MAX, radix);
    if (res == 0) {
        /*
         * the tweak and its length are specified as NULL and 0,
         * respectively which indicates that the value specified
         * to the create function should be used.
         *
         * if the tweak and size are specified here, they will be
         * used instead
         */
        ff1_encrypt(ctx, out, PT, NULL, 0);
        ff1_decrypt(ctx, out, out, NULL, 0);

        ff1_ctx_destroy(ctx);
    }

    /* release allocated space */
    free(out);
```
### FF3-1
```c
    /*
     * @K is an array of bytes of length @k which must be 16, 24, or 32
     * @T is an array of bytes of length 7.
     *
     * @radix and @PT are "user inputs"
     */

    struct ff1_ctx * ctx;
    char * out;
    int res;

    /* allocate space for the output */
    out = (char *)calloc(strlen(PT) + 1, 1);

    res = ff3_1_ctx_create(&ctx, K, k, T, radix);
    if (res == 0) {
        /*
         * the tweak is specified as NULL, which indicates
         * that the value specified to the create function
         * should be used.
         *
         * if the tweak is specified here, it must be 7 bytes
         * and long will be used instead
         */
        ff3_1_encrypt(ctx, out, PT, NULL);
        ff3_1_decrypt(ctx, out, out, NULL);

        ff3_1_ctx_destroy(ctx);
    }

    /* release allocated space */
    free(out);
```

[800-38g1]:https://nvlpubs.nist.gov/nistpubs/SpecialPublications/NIST.SP.800-38Gr1-draft.pdf
[ff1-examples]:https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/FF1samples.pdf
[ff3-cryptanalysis]:https://csrc.nist.gov/News/2017/Recent-Cryptanalysis-of-FF3
