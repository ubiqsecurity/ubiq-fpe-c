#include <gtest/gtest.h>
#include <ubiq/fpe/internal/bn.h>

TEST(bn, set_str)
{
    char n5[] = "00011010";

    uint8_t * numb;
    size_t numc;
    bigint_t n;

    bigint_init(&n);

    bigint_set_str(&n, n5, 5);
    EXPECT_EQ(bigint_cmp_si(&n, 755), 0);

    bigint_deinit(&n);
}

static
void radix_test(const char * const input, const char * const ialpha,
                const char * const oalpha, const char * const expect)
{
    bigint_t n;
    int r1, r2;

    std::string output;

    /* @n will be the numerical value of @inp */
    bigint_init(&n);

    r1 = __bigint_set_str(&n, input, ialpha);
    ASSERT_EQ(r1, 0);

    r1 = __bigint_get_str(nullptr, 0, oalpha, &n);
    ASSERT_GT(r1, 0);
    output.resize(r1);

    r2 = __bigint_get_str((char *)output.data(), r1, oalpha, &n);
    EXPECT_EQ(r1, r2);
    EXPECT_EQ(std::string(output), expect);

    bigint_deinit(&n);
}

TEST(radix, dec2hex)
{
    radix_test("100", "0123456789", "0123456789ABCDEF", "64");
}

TEST(radix, oct2hex)
{
    radix_test("100", "01234567", "0123456789ABCDEF", "40");
}

TEST(radix, dec2dec)
{
    radix_test("@$#", "!@#$%^&*()", "0123456789", "132");
}

TEST(radix, oct2dec)
{
    radix_test("@$#", "!@#$%^&*", "0123456789", "90");
}
