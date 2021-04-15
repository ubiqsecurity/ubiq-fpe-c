#include <gtest/gtest.h>
#include <ubiq/fpe/ff3_1.h>

static
void ff3_1_test(const uint8_t * const K, const size_t k,
                const uint8_t * const T /* always 56 bits */,
                const char * const PT, const char * const CT,
                const unsigned int radix)
{
    char * out;

    ASSERT_EQ(strlen(PT), strlen(CT));
    out = (char *)calloc(strlen(PT) + 1, 1);
    ASSERT_NE(out, nullptr);

    EXPECT_EQ(ff3_1_encrypt(out, K, k, T, PT, radix), 0);
    EXPECT_EQ(strcmp(out, CT), 0);

    EXPECT_EQ(ff3_1_decrypt(out, K, k, T, CT, radix), 0);
    EXPECT_EQ(strcmp(out, PT), 0);

    free(out);
}

TEST(ff3_1, ubiq1)
{
    const uint8_t K[] = {
        0xef, 0x43, 0x59, 0xd8, 0xd5, 0x80, 0xaa, 0x4f,
        0x7f, 0x03, 0x6d, 0x6f, 0x04, 0xfc, 0x6a, 0x94,
    };

    const uint8_t T[7] = { 0 };

    const char PT[] = "890121234567890000";
    const char CT[] = "075870132022772250";

    ff3_1_test(K, sizeof(K), T, PT, CT, 10);
}

TEST(ff3_1, ubiq2)
{
    const uint8_t K[] = {
        0xef, 0x43, 0x59, 0xd8, 0xd5, 0x80, 0xaa, 0x4f,
        0x7f, 0x03, 0x6d, 0x6f, 0x04, 0xfc, 0x6a, 0x94,
    };

    const uint8_t T[7] = {
        0x39, 0x38, 0x37, 0x36, 0x35, 0x34, 0x33,
    };

    const char PT[] = "890121234567890000";
    const char CT[] = "251467746185412673";

    ff3_1_test(K, sizeof(K), T, PT, CT, 10);
}

TEST(ff3_1, ubiq3)
{
    const uint8_t K[] = {
        0xef, 0x43, 0x59, 0xd8, 0xd5, 0x80, 0xaa, 0x4f,
        0x7f, 0x03, 0x6d, 0x6f, 0x04, 0xfc, 0x6a, 0x94,
    };

    const uint8_t T[7] = {
        0x37, 0x37, 0x37, 0x37, 0x70, 0x71, 0x72,
    };

    const char PT[] = "89012123456789abcd";
    const char CT[] = "aw75tnkkj9ceflnqx8";

    ff3_1_test(K, sizeof(K), T, PT, CT, 36);
}

TEST(ff3_1, ubiq4)
{
    const uint8_t K[] = {
        0xef, 0x43, 0x59, 0xd8, 0xd5, 0x80, 0xaa, 0x4f,
        0x7f, 0x03, 0x6d, 0x6f, 0x04, 0xfc, 0x6a, 0x94,
        0x3b, 0x80, 0x6a, 0xeb, 0x63, 0x08, 0x27, 0x1f,
    };

    const uint8_t T[7] = { 0 };

    const char PT[] = "890121234567890000";
    const char CT[] = "327701863379108161";

    ff3_1_test(K, sizeof(K), T, PT, CT, 10);
}

TEST(ff3_1, ubiq5)
{
    const uint8_t K[] = {
        0xef, 0x43, 0x59, 0xd8, 0xd5, 0x80, 0xaa, 0x4f,
        0x7f, 0x03, 0x6d, 0x6f, 0x04, 0xfc, 0x6a, 0x94,
        0x3b, 0x80, 0x6a, 0xeb, 0x63, 0x08, 0x27, 0x1f,
    };

    const uint8_t T[7] = {
        0x39, 0x38, 0x37, 0x36, 0x35, 0x34, 0x33,
    };

    const char PT[] = "890121234567890000";
    const char CT[] = "738670454850774517";

    ff3_1_test(K, sizeof(K), T, PT, CT, 10);
}

TEST(ff3_1, ubiq6)
{
    const uint8_t K[] = {
        0xef, 0x43, 0x59, 0xd8, 0xd5, 0x80, 0xaa, 0x4f,
        0x7f, 0x03, 0x6d, 0x6f, 0x04, 0xfc, 0x6a, 0x94,
        0x3b, 0x80, 0x6a, 0xeb, 0x63, 0x08, 0x27, 0x1f,
    };

    const uint8_t T[7] = {
        0x37, 0x37, 0x37, 0x37, 0x70, 0x71, 0x72,
    };

    const char PT[] = "89012123456789abcd";
    const char CT[] = "81wr04fi9op4ve2w2z";

    ff3_1_test(K, sizeof(K), T, PT, CT, 36);
}

TEST(ff3_1, ubiq7)
{
    const uint8_t K[] = {
        0xef, 0x43, 0x59, 0xd8, 0xd5, 0x80, 0xaa, 0x4f,
        0x7f, 0x03, 0x6d, 0x6f, 0x04, 0xfc, 0x6a, 0x94,
        0x3b, 0x80, 0x6a, 0xeb, 0x63, 0x08, 0x27, 0x1f,
        0x65, 0xcf, 0x33, 0xc7, 0x39, 0x1b, 0x27, 0xf7,
    };

    const uint8_t T[7] = { 0 };

    const char PT[] = "890121234567890000";
    const char CT[] = "892299037726855422";

    ff3_1_test(K, sizeof(K), T, PT, CT, 10);
}

TEST(ff3_1, ubiq8)
{
    const uint8_t K[] = {
        0xef, 0x43, 0x59, 0xd8, 0xd5, 0x80, 0xaa, 0x4f,
        0x7f, 0x03, 0x6d, 0x6f, 0x04, 0xfc, 0x6a, 0x94,
        0x3b, 0x80, 0x6a, 0xeb, 0x63, 0x08, 0x27, 0x1f,
        0x65, 0xcf, 0x33, 0xc7, 0x39, 0x1b, 0x27, 0xf7,
    };

    const uint8_t T[7] = {
        0x39, 0x38, 0x37, 0x36, 0x35, 0x34, 0x33,
    };

    const char PT[] = "890121234567890000";
    const char CT[] = "045013216693726967";

    ff3_1_test(K, sizeof(K), T, PT, CT, 10);
}

TEST(ff3_1, ubiq9)
{
    const uint8_t K[] = {
        0xef, 0x43, 0x59, 0xd8, 0xd5, 0x80, 0xaa, 0x4f,
        0x7f, 0x03, 0x6d, 0x6f, 0x04, 0xfc, 0x6a, 0x94,
        0x3b, 0x80, 0x6a, 0xeb, 0x63, 0x08, 0x27, 0x1f,
        0x65, 0xcf, 0x33, 0xc7, 0x39, 0x1b, 0x27, 0xf7,
    };

    const uint8_t T[7] = {
        0x37, 0x37, 0x37, 0x37, 0x70, 0x71, 0x72,
    };

    const char PT[] = "89012123456789abcd";
    const char CT[] = "i5cz928n3azucq8kjl";

    ff3_1_test(K, sizeof(K), T, PT, CT, 36);
}
