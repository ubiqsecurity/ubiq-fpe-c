#include <gtest/gtest.h>
#include <ubiq/fpe/ff1.h>

static
void ff1_test(const uint8_t * const K, const size_t k,
              const uint8_t * const T, const size_t t,
              const char * const PT, const char * const CT,
              const unsigned int radix)
{
    struct ff1_ctx * ctx;
    char * out;
    int res;

    ASSERT_EQ(strlen(PT), strlen(CT));
    out = (char *)calloc(strlen(PT) + 1, 1);
    ASSERT_NE(out, nullptr);

    res = ff1_ctx_create(&ctx, K, k, T, t, 0, SIZE_MAX, radix);
    EXPECT_EQ(res, 0);
    if (res == 0) {
        EXPECT_EQ(ff1_encrypt(ctx, out, PT, NULL, 0), 0);
        EXPECT_EQ(strcmp(out, CT), 0);

        EXPECT_EQ(ff1_decrypt(ctx, out, CT, NULL, 0), 0);
        EXPECT_EQ(strcmp(out, PT), 0);

        ff1_ctx_destroy(ctx);
    }

    free(out);
}

TEST(ff1, nist1)
{
    const uint8_t K[] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
    };

    const uint8_t T[] = {};

    const char PT[] = "0123456789";
    const char CT[] = "2433477484";

    ff1_test(K, sizeof(K), T, sizeof(T), PT, CT, 10);
}

TEST(ff1, nist2)
{
    const uint8_t K[] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
    };

    const uint8_t T[] = {
        0x39, 0x38, 0x37, 0x36, 0x35, 0x34, 0x33, 0x32, 0x31, 0x30,
    };

    const char PT[] = "0123456789";
    const char CT[] = "6124200773";

    ff1_test(K, sizeof(K), T, sizeof(T), PT, CT, 10);
}

TEST(ff1, nist3)
{
    const uint8_t K[] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
    };

    const uint8_t T[] = {
        0x37, 0x37, 0x37, 0x37, 0x70, 0x71, 0x72, 0x73, 0x37, 0x37, 0x37,
    };

    const char PT[] = "0123456789abcdefghi";
    const char CT[] = "a9tv40mll9kdu509eum";

    ff1_test(K, sizeof(K), T, sizeof(T), PT, CT, 36);
}

TEST(ff1, nist4)
{
    const uint8_t K[] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
        0xef, 0x43, 0x59, 0xd8, 0xd5, 0x80, 0xaa, 0x4f,
    };

    const uint8_t T[] = {};

    const char PT[] = "0123456789";
    const char CT[] = "2830668132";

    ff1_test(K, sizeof(K), T, sizeof(T), PT, CT, 10);
}

TEST(ff1, nist5)
{
    const uint8_t K[] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
        0xef, 0x43, 0x59, 0xd8, 0xd5, 0x80, 0xaa, 0x4f,
    };

    const uint8_t T[] = {
        0x39, 0x38, 0x37, 0x36, 0x35, 0x34, 0x33, 0x32, 0x31, 0x30,
    };

    const char PT[] = "0123456789";
    const char CT[] = "2496655549";

    ff1_test(K, sizeof(K), T, sizeof(T), PT, CT, 10);
}

TEST(ff1, nist6)
{
    const uint8_t K[] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
        0xef, 0x43, 0x59, 0xd8, 0xd5, 0x80, 0xaa, 0x4f,
    };

    const uint8_t T[] = {
        0x37, 0x37, 0x37, 0x37, 0x70, 0x71, 0x72, 0x73, 0x37, 0x37, 0x37,
    };

    const char PT[] = "0123456789abcdefghi";
    const char CT[] = "xbj3kv35jrawxv32ysr";

    ff1_test(K, sizeof(K), T, sizeof(T), PT, CT, 36);
}

TEST(ff1, nist7)
{
    const uint8_t K[] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
        0xef, 0x43, 0x59, 0xd8, 0xd5, 0x80, 0xaa, 0x4f,
        0x7f, 0x03, 0x6d, 0x6f, 0x04, 0xfc, 0x6a, 0x94
    };

    const uint8_t T[] = {};

    const char PT[] = "0123456789";
    const char CT[] = "6657667009";

    ff1_test(K, sizeof(K), T, sizeof(T), PT, CT, 10);
}

TEST(ff1, nist8)
{
    const uint8_t K[] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
        0xef, 0x43, 0x59, 0xd8, 0xd5, 0x80, 0xaa, 0x4f,
        0x7f, 0x03, 0x6d, 0x6f, 0x04, 0xfc, 0x6a, 0x94
    };

    const uint8_t T[] = {
        0x39, 0x38, 0x37, 0x36, 0x35, 0x34, 0x33, 0x32, 0x31, 0x30,
    };

    const char PT[] = "0123456789";
    const char CT[] = "1001623463";

    ff1_test(K, sizeof(K), T, sizeof(T), PT, CT, 10);
}

TEST(ff1, nist9)
{
    const uint8_t K[] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
        0xef, 0x43, 0x59, 0xd8, 0xd5, 0x80, 0xaa, 0x4f,
        0x7f, 0x03, 0x6d, 0x6f, 0x04, 0xfc, 0x6a, 0x94
    };

    const uint8_t T[] = {
        0x37, 0x37, 0x37, 0x37, 0x70, 0x71, 0x72, 0x73, 0x37, 0x37, 0x37,
    };

    const char PT[] = "0123456789abcdefghi";
    const char CT[] = "xs8a0azh2avyalyzuwd";

    ff1_test(K, sizeof(K), T, sizeof(T), PT, CT, 36);
}

TEST(ff1, base2)
{
    const uint8_t K[] = {
        0xf4, 0xa1, 0x16, 0xd6, 0xee, 0x40, 0x6a, 0x53,
        0xa5, 0x6c, 0xbe, 0x0f, 0x4a, 0xa7, 0xb1, 0x00,
        0x1c, 0xdc, 0x0a, 0x55, 0xca, 0xc9, 0x63, 0xcf,
        0x5a, 0xce, 0x39, 0x04, 0x88, 0xb3, 0x47, 0x7a
    };
    const uint8_t T[] = {
        0xfd, 0x7f, 0x4b, 0x99, 0x45, 0xa3, 0xc5, 0x35,
        0xad, 0xb4, 0x72, 0x00, 0x27, 0x11, 0x6c, 0xa0,
        0xf4, 0x98, 0x7d, 0x7f, 0x3f, 0xdb, 0xa9, 0xbb,
        0xc4, 0x0e, 0x75, 0x37, 0x5f, 0xea, 0xa6, 0x3c
    };

    const char PT[] = "00000101011011011101001001010011100111100011001";
    const char CT[] = "10110101001110101101110000011000000011111100111";

    ff1_test(K, sizeof(K), T, sizeof(T), PT, CT, 2);
}
