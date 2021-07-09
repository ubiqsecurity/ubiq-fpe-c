#include <gtest/gtest.h>
#include <ubiq/fpe/internal/radix.h>

static
void radix_test(const char * const input, const char * const ialpha,
                const char * const oalpha, const char * const expect)
{
    char output[64];
    int res;

    res = radix_convert(output, oalpha, input, ialpha);
    EXPECT_EQ(res, 0);
    EXPECT_EQ(std::string(output), expect);
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
