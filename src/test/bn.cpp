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

static void _copy(uint32_t * u32_dest, const char * const src) {
  for (int i = 0; i < strlen(src); i++) {
    u32_dest[i] = (uint8_t)src[i];
  }
}

static
void __radix_test(const char * const input, const char * const ialpha,
                  const char * const oalpha, const char * const expect)
{
    bigint_t n, n2;
    bigint_t n32;
    int r1, r2, r3, r4;

    std::string output;
    std::vector<uint32_t> u32_output;

    uint32_t * u32_input = (uint32_t *)calloc(strlen(input) + 1, sizeof(uint32_t));
    uint32_t * u32_ialpha = (uint32_t *)calloc(strlen(ialpha) + 1, sizeof(uint32_t));
    uint32_t * u32_oalpha = (uint32_t *)calloc(strlen(oalpha) + 1, sizeof(uint32_t));
    uint32_t * u32_expect = (uint32_t *)calloc(strlen(expect) + 1, sizeof(uint32_t));

    _copy(u32_input, input);
    _copy(u32_ialpha, ialpha);
    _copy(u32_oalpha, oalpha);
    _copy(u32_expect, expect);

    /* @n will be the numerical value of @inp */
    bigint_init(&n);
    bigint_init(&n2);

    r1 = __bigint_set_str(&n, input, ialpha);
    r3 = __u32_bigint_set_str(&n2, u32_input, u32_ialpha);
    ASSERT_EQ(r1, 0);
    ASSERT_EQ(r3, 0);
    ASSERT_EQ(mpz_cmp(n, n2), 0);

    r1 = __bigint_get_str(nullptr, 0, oalpha, &n);
    r3 = __u32_bigint_get_str(nullptr, 0, u32_oalpha, &n2);
    ASSERT_GT(r1, 0);
    ASSERT_GT(r3, 0);
    output.resize(r1);
    u32_output.resize(r3);
    ASSERT_EQ(r1, r3);

    r2 = __bigint_get_str((char *)output.data(), r1, oalpha, &n);
    EXPECT_EQ(r1, r2);
    EXPECT_EQ(std::string(output), expect);

    r4 = __u32_bigint_get_str(u32_output.data(), r3, u32_oalpha, &n2);
    EXPECT_EQ(r3, r4);
    for (int i=0; i < output.length(); i++) {
      EXPECT_EQ(u32_output[i], u32_expect[i]);
      EXPECT_EQ(u32_output[i], expect[i]);
    }

//    EXPECT_EQ(std::string(output), expect);

    free(u32_input);
    free(u32_ialpha);
    free(u32_oalpha);
    free(u32_expect);

    bigint_deinit(&n);
}

static
void radix_test(const char * const input, const char * const ialpha,
                const char * const oalpha, const char * const expect)
{
    /* convert from one radix to another */
    __radix_test(input, ialpha, oalpha, expect);
    /* test that the conversion can be successfully reversed */
    __radix_test(expect, oalpha, ialpha, input);
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
