#include <gtest/gtest.h>
#include <ubiq/fpe/ff1.h>

TEST(ff1, sample1)
{
    const uint8_t K[] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
    };

    const uint8_t T[] = {};

    char PT[] = "0123456789";
    char CT[sizeof(PT)];

    int res;

    res = ff1_encrypt(CT, K, sizeof(K), T, sizeof(T), PT, 10);
    EXPECT_EQ(res, 0);
    EXPECT_EQ(strcmp(CT, "2433477484"), 0);

    memset(PT, '0', sizeof(PT) - 1);
    res = ff1_decrypt(PT, K, sizeof(K), T, sizeof(T), CT, 10);
    EXPECT_EQ(res, 0);
    EXPECT_EQ(strcmp(PT, "0123456789"), 0);
}

TEST(ff1, sample2)
{
    const uint8_t K[] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
    };

    const uint8_t T[] = {
        0x39, 0x38, 0x37, 0x36, 0x35, 0x34, 0x33, 0x32, 0x31, 0x30,
    };

    char PT[] = "0123456789";
    char CT[sizeof(PT)];

    int res;

    res = ff1_encrypt(CT, K, sizeof(K), T, sizeof(T), PT, 10);
    EXPECT_EQ(res, 0);
    EXPECT_EQ(strcmp(CT, "6124200773"), 0);

    memset(PT, '0', sizeof(PT) - 1);
    res = ff1_decrypt(PT, K, sizeof(K), T, sizeof(T), CT, 10);
    EXPECT_EQ(res, 0);
    EXPECT_EQ(strcmp(PT, "0123456789"), 0);
}

TEST(ff1, sample3)
{
    const uint8_t K[] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
    };

    const uint8_t T[] = {
        0x37, 0x37, 0x37, 0x37, 0x70, 0x71, 0x72, 0x73, 0x37, 0x37, 0x37,
    };

    char PT[] = "0123456789abcdefghi";
    char CT[sizeof(PT)];

    int res;

    res = ff1_encrypt(CT, K, sizeof(K), T, sizeof(T), PT, 36);
    EXPECT_EQ(res, 0);
    EXPECT_EQ(strcmp(CT, "a9tv40mll9kdu509eum"), 0);

    memset(PT, '0', sizeof(PT) - 1);
    res = ff1_decrypt(PT, K, sizeof(K), T, sizeof(T), CT, 36);
    EXPECT_EQ(res, 0);
    EXPECT_EQ(strcmp(PT, "0123456789abcdefghi"), 0);
}

TEST(ff1, sample4)
{
    const uint8_t K[] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
        0xef, 0x43, 0x59, 0xd8, 0xd5, 0x80, 0xaa, 0x4f,
    };

    const uint8_t T[] = {};

    char PT[] = "0123456789";
    char CT[sizeof(PT)];

    int res;

    res = ff1_encrypt(CT, K, sizeof(K), NULL, 0, PT, 10);
    EXPECT_EQ(res, 0);
    EXPECT_EQ(strcmp(CT, "2830668132"), 0);

    memset(PT, '0', sizeof(PT) - 1);
    res = ff1_decrypt(PT, K, sizeof(K), T, sizeof(T), CT, 10);
    EXPECT_EQ(res, 0);
    EXPECT_EQ(strcmp(PT, "0123456789"), 0);
}

TEST(ff1, sample5)
{
    const uint8_t K[] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
        0xef, 0x43, 0x59, 0xd8, 0xd5, 0x80, 0xaa, 0x4f,
    };

    const uint8_t T[] = {
        0x39, 0x38, 0x37, 0x36, 0x35, 0x34, 0x33, 0x32, 0x31, 0x30,
    };

    char PT[] = "0123456789";
    char CT[sizeof(PT)];

    int res;

    res = ff1_encrypt(CT, K, sizeof(K), T, sizeof(T), PT, 10);
    EXPECT_EQ(res, 0);
    EXPECT_EQ(strcmp(CT, "2496655549"), 0);

    memset(PT, '0', sizeof(PT) - 1);
    res = ff1_decrypt(PT, K, sizeof(K), T, sizeof(T), CT, 10);
    EXPECT_EQ(res, 0);
    EXPECT_EQ(strcmp(PT, "0123456789"), 0);
}

TEST(ff1, sample6)
{
    const uint8_t K[] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
        0xef, 0x43, 0x59, 0xd8, 0xd5, 0x80, 0xaa, 0x4f,
    };

    const uint8_t T[] = {
        0x37, 0x37, 0x37, 0x37, 0x70, 0x71, 0x72, 0x73, 0x37, 0x37, 0x37,
    };

    char PT[] = "0123456789abcdefghi";
    char CT[sizeof(PT)];

    int res;

    res = ff1_encrypt(CT, K, sizeof(K), T, sizeof(T), PT, 36);
    EXPECT_EQ(res, 0);
    EXPECT_EQ(strcmp(CT, "xbj3kv35jrawxv32ysr"), 0);

    memset(PT, '0', sizeof(PT) - 1);
    res = ff1_decrypt(PT, K, sizeof(K), T, sizeof(T), CT, 36);
    EXPECT_EQ(res, 0);
    EXPECT_EQ(strcmp(PT, "0123456789abcdefghi"), 0);
}

TEST(ff1, sample7)
{
    const uint8_t K[] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
        0xef, 0x43, 0x59, 0xd8, 0xd5, 0x80, 0xaa, 0x4f,
        0x7f, 0x03, 0x6d, 0x6f, 0x04, 0xfc, 0x6a, 0x94
    };

    const uint8_t T[] = {};

    char PT[] = "0123456789";
    char CT[sizeof(PT)];

    int res;

    res = ff1_encrypt(CT, K, sizeof(K), NULL, 0, PT, 10);
    EXPECT_EQ(res, 0);
    EXPECT_EQ(strcmp(CT, "6657667009"), 0);

    memset(PT, '0', sizeof(PT) - 1);
    res = ff1_decrypt(PT, K, sizeof(K), T, sizeof(T), CT, 10);
    EXPECT_EQ(res, 0);
    EXPECT_EQ(strcmp(PT, "0123456789"), 0);
}

TEST(ff1, sample8)
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

    char PT[] = "0123456789";
    char CT[sizeof(PT)];

    int res;

    res = ff1_encrypt(CT, K, sizeof(K), T, sizeof(T), PT, 10);
    EXPECT_EQ(res, 0);
    EXPECT_EQ(strcmp(CT, "1001623463"), 0);

    memset(PT, '0', sizeof(PT) - 1);
    res = ff1_decrypt(PT, K, sizeof(K), T, sizeof(T), CT, 10);
    EXPECT_EQ(res, 0);
    EXPECT_EQ(strcmp(PT, "0123456789"), 0);
}

TEST(ff1, sample9)
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

    char PT[] = "0123456789abcdefghi";
    char CT[sizeof(PT)];

    int res;

    res = ff1_encrypt(CT, K, sizeof(K), T, sizeof(T), PT, 36);
    EXPECT_EQ(res, 0);
    EXPECT_EQ(strcmp(CT, "xs8a0azh2avyalyzuwd"), 0);

    memset(PT, '0', sizeof(PT) - 1);
    res = ff1_decrypt(PT, K, sizeof(K), T, sizeof(T), CT, 36);
    EXPECT_EQ(res, 0);
    EXPECT_EQ(strcmp(PT, "0123456789abcdefghi"), 0);
}
