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
void __u32_radix_test(const char * const input, const char * const ialpha,
                  const char * const oalpha, const char * const expect)
{
    // bigint_t n;
    // int r1, r2;

    // std::vector<uint32_t> output;

    // uint32_t * u32_input = (uint32_t *)calloc(strlen(input) + 1, sizeof(uint32_t));
    // uint32_t * u32_ialpha = (uint32_t *)calloc(strlen(ialpha) + 1, sizeof(uint32_t));
    // uint32_t * u32_oalpha = (uint32_t *)calloc(strlen(oalpha) + 1, sizeof(uint32_t));
    // uint32_t * u32_expect = (uint32_t *)calloc(strlen(expect) + 1, sizeof(uint32_t));

    // _copy(u32_input, input);
    // _copy(u32_ialpha, ialpha);
    // _copy(u32_oalpha, oalpha);
    // _copy(u32_expect, expect);

    // /* @n will be the numerical value of @inp */
    // bigint_init(&n);

    // r1 = __u32_bigint_set_str(&n, u32_input, u32_ialpha);
    // ASSERT_EQ(r1, 0);

    // r1 = __u32_bigint_get_str(nullptr, 0, u32_oalpha, &n);
    // ASSERT_GT(r1, 0);
    // output.resize(r1);

    // r2 = __u32_bigint_get_str(output.data(), r1, u32_oalpha, &n);
    // EXPECT_EQ(r1, r2);
    // for (int i=0; i < output.size(); i++) {
    //   EXPECT_EQ(output[i], u32_expect[i]);
    // }

    // free(u32_input);
    // free(u32_ialpha);
    // free(u32_oalpha);
    // free(u32_expect);

    // bigint_deinit(&n);
}

static
void __radix_test(const char * const input, const char * const ialpha,
                  const char * const oalpha, const char * const expect)
{
    bigint_t n;
    int r1, r2;

    std::vector<char> output;
    output.resize(50);

    /* @n will be the numerical value of @inp */
    bigint_init(&n);

    // std::cerr << " input(" << input << ")   expect(" << expect << ")"<< std::endl;
    // std::cerr << " ialpha(" << ialpha << ")   oalpha(" << oalpha << ")"<< std::endl;
    r1 = __bigint_set_str(&n, input, ialpha);
    ASSERT_EQ(r1, 0);


    r2 = __bigint_get_str(output.data(), output.size(), oalpha, &n);
    EXPECT_EQ(r2, 0) ;
    EXPECT_EQ(strcmp(output.data(), expect), 0) << " output(" << output.data() << ")   expect(" << expect << ")"<< std::endl;


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


    /* convert from one radix to another */
    __u32_radix_test(input, ialpha, oalpha, expect);
    /* test that the conversion can be successfully reversed */
    __u32_radix_test(expect, oalpha, ialpha, input);
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

TEST(radix, invalid_dest_buffer)
{
    bigint_t n;
    int r1(0);
    int r2(0);
    const char data []= "123456789";
    const char input_radix [] = "0123456789";
    const char output_radix [] = "0123456789ABC";
    std::vector<char> output;


    bigint_init(&n);

    r1 = __bigint_set_str(&n, data, input_radix);
    ASSERT_EQ(r1, 0);

    output.resize(50);

    // Make sure dest buffer is TOO small
    r2 = __bigint_get_str(output.data(), 1, output_radix, &n);
    EXPECT_EQ(r2, -ENOMEM);

    bigint_deinit(&n);
}

TEST(radix, invalid_characters_buffer)
{
    bigint_t n;
    int r1(0);
    const char data []= "123456789";
    const char input_radix [] = "ABCDEFG";
    bigint_init(&n);

    r1 = __bigint_set_str(&n, data, input_radix);
    ASSERT_EQ(r1, -EINVAL);

  
    bigint_deinit(&n);
}

// Use arbitrary radix 62 and test conversions back and forth
TEST(radix, radix62)
{
    char data [5] = "\0\0\0\0";
    std::vector<char> input_radix;
    const char output_radix [] = "0123456879";

    input_radix.resize(62);

    // Arbitrary input string
    for (int i = 0;i < input_radix.size(); i++) {
        input_radix[i] = '0' + i;
    }

    data[0] = input_radix[1];
    radix_test(data, input_radix.data(), output_radix, "1");

    data[0] = input_radix.back();
    radix_test(data, input_radix.data(), output_radix, "61");

}

// Use arbitrary radix > 62 and test conversions back and forth
TEST(radix, radix63)
{
    char data [5] = "\0\0\0\0";
    std::vector<char> input_radix;
    const char output_radix [] = "0123456879";

    input_radix.resize(63);

    for (int i = 0;i < input_radix.size(); i++) {
        input_radix[i] = '0' + i;
    }

    data[0] = input_radix[1];
    radix_test(data, input_radix.data(), output_radix, "1");

    data[0] = input_radix.back();
    radix_test(data, input_radix.data(), output_radix, "62");

}

TEST(chars, mapset)
{
    int r1;

    char src[]      = "BCDEFGHIJ";
    char expected[] = "123456789";

    const char input_set[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char output_set[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    char * dst1;
    char * dst2;

    dst1 = strdup(src);
    r1 = map_characters(dst1,src,input_set, output_set);
    ASSERT_EQ(r1, 0);
    EXPECT_EQ(strcmp(dst1, expected),0) << " dst1 = '" << dst1 << "'";

    dst2 = strdup(dst1);
    r1 = map_characters(dst2, dst1, output_set, input_set);
    ASSERT_EQ(r1, 0);
    EXPECT_EQ(strcmp(dst2, src),0);

    free(dst1);
    free(dst2);

}

TEST(chars, mapset_2)
{
    unsigned long long r1 = 0;

    char src[]      = "JIHGFEDCBABCDEFGHIJ";
    char expected[] = "9876543210123456789";

    const char input_set[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char output_set[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    char * dst1;
    char * dst2;

    dst1 = strdup(src);

    r1 = map_characters(dst1,src,input_set, output_set);
    ASSERT_EQ(r1, 0);
    EXPECT_EQ(strcmp(dst1, expected),0) << " dst1 = '" << dst1 << "'";

    dst2 = strdup(dst1);
    r1 = map_characters(dst2, dst1, output_set, input_set);
    ASSERT_EQ(r1, 0);
    EXPECT_EQ(strcmp(dst2, src),0);

    free(dst1);
    free(dst2);

}



TEST(chars, non_std)
{
    unsigned long long r1 = 0;

    char src[] = "!@#$%^&*()";
    char expected[] = "02468ACEGI";

    const char input_set[] =  "!a@b#c$d%e^f&g*h(i)";
    const char output_set[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    char * dst1;
    char * dst2;

    dst1 = strdup(src);
    ASSERT_EQ(map_characters(dst1,src,input_set, output_set), 0);

    EXPECT_EQ(strcmp(dst1, expected),0) << " dst1 = '" << dst1 << "'";

    dst2 = strdup(dst1);
    r1 = map_characters(dst2, dst1, output_set, input_set);
    ASSERT_EQ(r1, 0);
    EXPECT_EQ(strcmp(dst2, src),0);

    free(dst1);
    free(dst2);


}

TEST(chars, mapset_invalid)
{
    unsigned long long r1 = 0;

    char src[]      = "JIHGFEDCBABCDEFGHIJ";
    char expected[] = "9876543210123456789";

    const char input_set[] = "BCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char output_set[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    char * dst1;

    dst1 = strdup(src);

    r1 = map_characters(dst1,src,input_set, output_set);
    EXPECT_NE(r1, 0);

    free(dst1);

}

TEST(radix, t1)
{
    unsigned long long r1 = 0;

    char src[]      = "1234567890ABCDEFabcdef";
    const char i_radix[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const char o_radix[] = "0123456789";
    char expect[] = "45117932320280791338835719979215890389";

    std::vector<char> data(50);
  
   radix_test(src, i_radix, o_radix, expect);
}

TEST(radix, t2)
{
    unsigned long long r1 = 0;

    char src[]      = "3456789AB2CDEFGHcdefgh";
    const char i_radix[] = "23456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz01";
    const char o_radix[] = "0123456789";
    char expect[] = "45117932320280791338835719979215890389";

    std::vector<char> data(50);
  
   radix_test(src, i_radix, o_radix, expect);
}

TEST(radix, t3)
{
    unsigned long long r1 = 0;

    char src[]      = "6KNFyZss7SBQ7w7udUwsv8";
    const char i_radix[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const char o_radix[] = "23456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz01";
    char expect[] = "8MPH0buu9UDS9y9wfWyuxA";

    std::vector<char> data(50);
  
   radix_test(src, i_radix, o_radix, expect);
}


TEST(radix, t4)
{
    unsigned long long r1 = 0;
    // char src[]      = "959223218629794";
    //  const char i_radix[] = "0123456789";
    char src[]      = "9G0L29YNJ6";
    const char i_radix[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char o_radix[] = "!\"#$%'()*+,./0123456789:<=>?ABCDEFGHIJKLMNOPQRSTUVWXYZ[]_`abcdefghijklmnopqrstuvwxyz{}~";

    char expect[] = "=J*K42c(";

    std::vector<char> data(50);
  
    printf("radix %d\n", strlen(o_radix));

   radix_test(src, i_radix, o_radix, expect);

   radix_test(expect, o_radix, i_radix, src);

}

TEST(radix, u32)
{
    unsigned long long r1 = 0;
    // char src[]      = "959223218629794";
    //  const char i_radix[] = "0123456789";
    char src[]      = "1234567890";
    const char i_radix[] = "0123456789";
    const wchar_t o_radix[] = L"0123456789";

    const wchar_t expect[] = L"1234567890";

    std::vector<wchar_t> data(50);
  
    bigint_t n;

    bigint_init(&n);

    r1 = __bigint_set_str(&n, src, i_radix);
    EXPECT_EQ(r1, 0);

    r1 = __u32_bigint_get_str((uint32_t *)data.data(), data.size(),  (uint32_t *)o_radix, &n);
    EXPECT_EQ(r1, 0);

    EXPECT_EQ(memcmp(expect, data.data(), sizeof(expect)), 0);

}

TEST(radix, u32_b)
{
    unsigned long long r1 = 0;
    // char src[]      = "959223218629794";
    //  const char i_radix[] = "0123456789";
    char src[]      = "1234567890";
    const char i_radix[] = "0123456789";
    const wchar_t o_radix[] = L"0123456789";

    const wchar_t expect[] = L"1234567890";

    std::vector<wchar_t> data(50);
  
    bigint_t n;

    bigint_init(&n);

    r1 = __bigint_set_str(&n, src, i_radix);
    EXPECT_EQ(r1, 0);

    r1 = __u32_bigint_get_str((uint32_t *)data.data(), data.size(),  (uint32_t *)o_radix, &n);
    EXPECT_EQ(r1, 0);

    EXPECT_EQ(memcmp(expect, data.data(), sizeof(expect)), 0);

}

TEST(radix, u32_c)
{
    unsigned long long r1 = 0;
    // char src[]      = "959223218629794";
    //  const char i_radix[] = "0123456789";
    char src[]      = "1234567890";
    const char i_radix[] = "0123456789";
    const wchar_t o_radix[] = L"ÊËÌÍÎÏðñòó";

    const wchar_t expect[] = L"ËÌÍÎÏðñòóÊ";

    std::vector<wchar_t> data(50);
  
    bigint_t n;

    bigint_init(&n);

    r1 = __bigint_set_str(&n, src, i_radix);
    EXPECT_EQ(r1, 0);

    r1 = __u32_bigint_get_str((uint32_t *)data.data(), data.size(),  (uint32_t *)o_radix, &n);
    EXPECT_EQ(r1, 0);

    EXPECT_EQ(memcmp(expect, data.data(), sizeof(expect)), 0);

}

TEST(radix, u32_d)
{
    unsigned long long r1 = 0;
    // char src[]      = "959223218629794";
    //  const char i_radix[] = "0123456789";
    char src[]      = "1234567890";
    const char i_radix[] = "0123456789";
    const wchar_t o_radix[] = L"ĵĶķĸĹϺϻϼϽϾ";

    const wchar_t expect[] = L"ĶķĸĹϺϻϼϽϾĵ";

    std::vector<wchar_t> data(50);
  
    bigint_t n;

    bigint_init(&n);

    r1 = __bigint_set_str(&n, src, i_radix);
    EXPECT_EQ(r1, 0);

    r1 = __u32_bigint_get_str((uint32_t *)data.data(), data.size(),  (uint32_t *)o_radix, &n);
    EXPECT_EQ(r1, 0);

    EXPECT_EQ(memcmp(expect, data.data(), sizeof(expect)), 0);

}

TEST(radix, u32_e)
{
    unsigned long long r1 = 0;
    // char src[]      = "959223218629794";
    //  const char i_radix[] = "0123456789";
    char src[]      = "1234567890";
    const char i_radix[] = "0123456789";
    const wchar_t o_radix[] = L"ĵĶķĸĹϺϻϼϽϾ";

    const wchar_t expect[] = L"ĶķĸĹϺϻϼϽϾĵ";

    std::vector<wchar_t> data(50);
  
    bigint_t n;

    bigint_init(&n);

    r1 = __bigint_set_str(&n, src, i_radix);
    EXPECT_EQ(r1, 0);

    r1 = __u32_bigint_get_str((uint32_t *)data.data(), data.size(),  (uint32_t *)o_radix, &n);
    EXPECT_EQ(r1, 0);

    EXPECT_EQ(memcmp(expect, data.data(), sizeof(expect)), 0);

}

TEST(radix, u32_f)
{
    unsigned long long r1 = 0;
    // char src[]      = "959223218629794";
    //  const char i_radix[] = "0123456789";
    char src[]      = "1234567890";
    const char i_radix[] = "0123456789";
    const wchar_t o_radix[] = L"ĵĶķĸĹϺϻϼϽϾ";

    const wchar_t expect[] = L"ĶķĸĹϺϻϼϽϾĵ";

    std::vector<wchar_t> data(50);
  
    bigint_t n;

    bigint_init(&n);

    r1 = __bigint_set_str(&n, src, i_radix);
    EXPECT_EQ(r1, 0);

    r1 = __u32_bigint_get_str((uint32_t *)data.data(), 2,  (uint32_t *)o_radix, &n);
    EXPECT_EQ(r1, -ENOMEM);

}

TEST(radix, u32_g)
{
    unsigned long long r1 = 0;
    // char src[]      = "959223218629794";
    //  const char i_radix[] = "0123456789";
    char src[]      = "1234567890";
    const char i_radix[] = "0123456789";
    const wchar_t o_radix[] = L"012345678Ͼ";

    const wchar_t expect[] = L"12345678Ͼ0";

    std::vector<wchar_t> data(50);
  
    bigint_t n;

    bigint_init(&n);

    r1 = __bigint_set_str(&n, src, i_radix);
    EXPECT_EQ(r1, 0);

    r1 = __u32_bigint_get_str((uint32_t *)data.data(), data.size(),  (uint32_t *)o_radix, &n);
    EXPECT_EQ(r1, 0);

    EXPECT_EQ(memcmp(expect, data.data(), sizeof(expect)), 0);

}

#ifdef NODEF
TEST(radix, negative)
{
    bigint_t n;
    bigint_t den;
    int r1(0);
    int r2(0);
    int r3(0);
    std::vector<char> output;

    const char numerator []= "-14103068008475998766";
    const char denominator []= "10000";
    const char input_radix [] = "0123456789";
    bigint_init(&n);
    bigint_init(&den);

    r1 = bigint_set_str(&n, numerator, 10);
    ASSERT_EQ(r1, 0);

    r1 = bigint_set_str(&den, denominator, 10);
    ASSERT_EQ(r1, 0);

    output.resize(50);

    r2 = bigint_get_str(output.data(), output.size(), 10, &n);
    EXPECT_EQ(r2, 0);
//    EXPECT_EQ(strcmp(output.data(), data), 0);

    r2 = bigint_get_str(output.data(), output.size(), 10, &den);
    EXPECT_EQ(r2, 0);
    EXPECT_EQ(strcmp(output.data(), denominator), 0);

    bigint_mod(&n, &n, &den);
    EXPECT_EQ(r3, 0);
    
    r2 = bigint_get_str(output.data(), output.size(), 10, &n);
    EXPECT_EQ(r2, 0);
    std::cout << output.data() << std::endl;

    bigint_deinit(&n);
    bigint_deinit(&den);
}

TEST(radix, negative2)
{
    int num = -100;
    int den = 98;

    int r = num % den;

    std::cout << r << std::endl;

}
#endif