#include <gtest/gtest.h>
#include <ubiq/fpe/internal/ffx.h>

#include <string.h>
#include <unistr.h>
#include <uniwidth.h>
#include <wchar.h>
#include <chrono>


TEST(ffx, revs)
{
    char s[] = "abcde";
    char d[sizeof(s)];

    ASSERT_EQ(d, ffx_revs(d, s));
    EXPECT_EQ(strcmp(d, "edcba"), 0);

    ASSERT_EQ(s, ffx_revs(s, s));
    EXPECT_EQ(strcmp(s, "edcba"), 0);

    s[strlen(s) - 1] = '\0';
    ASSERT_EQ(d, ffx_revs(d, s));
    EXPECT_EQ(strcmp(d, "bcde"), 0);

    ASSERT_EQ(s, ffx_revs(s, s));
    EXPECT_EQ(strcmp(s, "bcde"), 0);
}

TEST(ffx, str)
{
    char s[5];
    bigint_t n;

    bigint_init(&n);
    bigint_set_str(&n, "559", 10);

    EXPECT_EQ(ffx_str(s, 5, 4, 12, &n), 0);
    EXPECT_EQ(strcmp(s, "03a7"), 0);

    bigint_deinit(&n);
}


// TEST(ffx, str_custom)
// {
//     char s[5];
//     bigint_t n;

//     bigint_init(&n);
//     bigint_set_str(&n, "559", 10);

//     s[0] = '\0';
//     EXPECT_EQ(ffx_str_custom_radix(s, 5, 4, (uint8_t*)"0123456789ab", &n), 0);
//     EXPECT_EQ(strcmp(s, "03a7"), 0);

//     bigint_deinit(&n);
// }

// TEST(ffx, str_custom_2)
// {
//     const char str[] = "1234567890";
//     char s[sizeof(str)];
//     bigint_t n;

//     bigint_init(&n);
//     bigint_set_str(&n, str, 10);

//     s[0] = '\0';
//     EXPECT_EQ(ffx_str_custom_radix(s, sizeof(s), sizeof(s) - 1, (uint8_t*)"0123456789", &n), 0);
//     EXPECT_EQ(strcmp(s, str), 0) << s << " " << str;

//     bigint_deinit(&n);
// }


// TEST(ffx, ffx_str_u32_custom_radix)
// {
//     char s[5];
//     bigint_t n;
//     const uint32_t * radix = (uint32_t*)L"0123456789ab";
//     const char * expected = u8"03a7";
//     const char str[] = "0559";

//     bigint_init(&n);
//     bigint_set_str(&n, str, 10);

//     s[0] = '\0';
//     EXPECT_EQ(ffx_str_u32_custom_radix(s, sizeof(s), u8_mbsnlen((uint8_t*)str, strlen(str)), radix, &n), 0);
//     EXPECT_EQ(strcmp(s, expected), 0) << s ;
//     EXPECT_EQ(u8_mbsnlen((uint8_t*)str, strlen(str)), u8_mbsnlen((uint8_t*)s, strlen(s)));

//     bigint_deinit(&n);
// }

// TEST(ffx, ffx_str_u32_custom_radix_2)
// {
//     setlocale(LC_ALL, "C.UTF-8");
//     const char str[] = "1234567890";
//     char s[30];
//     const uint32_t * radix = (uint32_t*)L"ĵĶķĸĹϺϻϼϽϾ";
//     const char * expected = u8"ĶķĸĹϺϻϼϽϾĵ";
//     // const char * u8 = u8"1Ķ2Ķ3";//ĵĶķĸ";// ĵĶķĸĹϺϻϼϽϾ";
//     // uint32_t * u32_radix = (uint32_t*)calloc(50, sizeof(uint32_t));
//     // size_t len = 50;
//     bigint_t n;

//     // printf("u8 %s\n", u8);

//     // u32_radix = u8_to_u32((uint8_t *)u8, strlen(u8), u32_radix, &len);

//     // printf("len(%d) strlen(%d), u8_mbsnlen(%d)\n", len, strlen(u8), u8_mbsnlen((uint8_t*)u8, strlen(u8)));
//     // if (printf("u32_radix = %S\n", u32_radix) < 0) {
//     //     perror("printf");
//     // }

//     // uint8_t * s2 = u8_strchr((uint8_t*)u8, U'Ķ');
//     // if (s2 != NULL) {
//     //     printf("strchr '%s'\n", s2);
//     // }

//     // for (int i = 0; i < u32_strlen(u32_radix); i++) {
//     //     printf("%C \n", u32_radix[i]);
//     // }

//     bigint_init(&n);
//     bigint_set_str(&n, str, 10);
//     s[0] = '\0';
// // printf("sizeof(s) - %d\n", sizeof(s));
//     EXPECT_EQ(ffx_str_u32_custom_radix(s, sizeof(s), u8_mbsnlen((uint8_t*)str, strlen(str)), radix, &n), 0);
//     EXPECT_EQ(strcmp(s, expected), 0) << s ;
//     EXPECT_EQ(u8_mbsnlen((uint8_t*)str, strlen(str)), u8_mbsnlen((uint8_t*)s, strlen(s)));

// //     bigint_deinit(&n);
// }

// TEST(ffx, ffx_str_u32_custom_radix_2b)
// {
//     setlocale(LC_ALL, "C.UTF-8");
//     const char str[] = "0000001234567890";
//     char s[sizeof(str) * 4];
//     const char * expected = u8"ĵĵĵĵĵĵĶķĸĹϺϻϼϽϾĵ";
//     const uint32_t * radix = (uint32_t*)L"ĵĶķĸĹϺϻϼϽϾ";
//     // const char * u8 = u8"1Ķ2Ķ3";//ĵĶķĸ";// ĵĶķĸĹϺϻϼϽϾ";
//     // uint32_t * u32_radix = (uint32_t*)calloc(50, sizeof(uint32_t));
//     // size_t len = 50;
//     bigint_t n;

//     // printf("u8 %s\n", u8);

//     // u32_radix = u8_to_u32((uint8_t *)u8, strlen(u8), u32_radix, &len);

//     // printf("len(%d) strlen(%d), u8_mbsnlen(%d)\n", len, strlen(u8), u8_mbsnlen((uint8_t*)u8, strlen(u8)));
//     // if (printf("u32_radix = %S\n", u32_radix) < 0) {
//     //     perror("printf");
//     // }

//     // uint8_t * s2 = u8_strchr((uint8_t*)u8, U'Ķ');
//     // if (s2 != NULL) {
//     //     printf("strchr '%s'\n", s2);
//     // }

//     // for (int i = 0; i < u32_strlen(u32_radix); i++) {
//     //     printf("%C \n", u32_radix[i]);
//     // }

//     bigint_init(&n);
//     bigint_set_str(&n, str, 10);
//     // s[0] = '\0';
// // printf("sizeof(s) - %d\n", sizeof(s));
//     EXPECT_EQ(ffx_str_u32_custom_radix(s, sizeof(s), u8_mbsnlen((uint8_t*)str, strlen(str)) , radix, &n), 0);
//     EXPECT_EQ(strcmp(s, expected), 0) << s << " " << expected;
//     EXPECT_EQ(u8_mbsnlen((uint8_t*)str, strlen(str)), u8_mbsnlen((uint8_t*)s, strlen(s)));

// //     bigint_deinit(&n);
// }


// TEST(ffx, ffx_str_u32_custom_radix_3)
// {
//     // setlocale(LC_ALL, "C.UTF-8");
//     const char str[] = "1234567890";
//     const uint32_t * radix = (uint32_t*)L"012345678Ͼ";
//     char s[30];
//     const char * expected = u8"12345678Ͼ0";
//     // const char * u8 = u8"1Ķ2Ķ3";//ĵĶķĸ";// ĵĶķĸĹϺϻϼϽϾ";
//     // uint32_t * u32_radix = (uint32_t*)calloc(50, sizeof(uint32_t));
//     // size_t len = 50;
//     bigint_t n;

//     // printf("u8 %s\n", u8);

//     // u32_radix = u8_to_u32((uint8_t *)u8, strlen(u8), u32_radix, &len);

//     // printf("len(%d) strlen(%d), u8_mbsnlen(%d)\n", len, strlen(u8), u8_mbsnlen((uint8_t*)u8, strlen(u8)));
//     // if (printf("u32_radix = %S\n", u32_radix) < 0) {
//     //     perror("printf");
//     // }

//     // uint8_t * s2 = u8_strchr((uint8_t*)u8, U'Ķ');
//     // if (s2 != NULL) {
//     //     printf("strchr '%s'\n", s2);
//     // }

//     // for (int i = 0; i < u32_strlen(u32_radix); i++) {
//     //     printf("%C \n", u32_radix[i]);
//     // }

//     bigint_init(&n);
//     bigint_set_str(&n, str, 10);
// //    s[0] = '\0';
//     EXPECT_EQ(ffx_str_u32_custom_radix(s, sizeof(s), u8_mbsnlen((uint8_t*)str, strlen(str)), radix, &n), 0);
//     EXPECT_EQ(strcmp(s, expected), 0);
//     EXPECT_EQ(u8_mbsnlen((uint8_t*)str, strlen(str)), u8_mbsnlen((uint8_t*)s, strlen(s)));

//     bigint_deinit(&n);
// }

// TEST(ffx, ffx_str_u32_custom_radix_4)
// {
//     setlocale(LC_ALL, "C.UTF-8");
//     const char str[] = "000001234567890";
//     const uint32_t * radix = (uint32_t *)L"012345678Ͼ";
//     char s[30];
//     const char * expected = u8"0000012345678Ͼ0";
//     // const char * u8 = u8"1Ķ2Ķ3";//ĵĶķĸ";// ĵĶķĸĹϺϻϼϽϾ";
//     // uint32_t * u32_radix = (uint32_t*)calloc(50, sizeof(uint32_t));
//     // size_t len = 50;
//     bigint_t n;

//     // printf("u8 %s\n", u8);

//     // u32_radix = u8_to_u32((uint8_t *)u8, strlen(u8), u32_radix, &len);

//     // printf("len(%d) strlen(%d), u8_mbsnlen(%d)\n", len, strlen(u8), u8_mbsnlen((uint8_t*)u8, strlen(u8)));
//     // if (printf("u32_radix = %S\n", u32_radix) < 0) {
//     //     perror("printf");
//     // }

//     // uint8_t * s2 = u8_strchr((uint8_t*)u8, U'Ķ');
//     // if (s2 != NULL) {
//     //     printf("strchr '%s'\n", s2);
//     // }

//     // for (int i = 0; i < u32_strlen(u32_radix); i++) {
//     //     printf("%C \n", u32_radix[i]);
//     // }

//     bigint_init(&n);
//     bigint_set_str(&n, str, 10);
// //    s[0] = '\0';
//     EXPECT_EQ(ffx_str_u32_custom_radix(s, sizeof(s), u8_mbsnlen((uint8_t*)str, strlen(str)) , radix, &n), 0);
//     EXPECT_EQ(strcmp(s, expected), 0);
//     EXPECT_EQ(u8_mbsnlen((uint8_t*)str, strlen(str)), u8_mbsnlen((uint8_t*)s, strlen(s)));

//     bigint_deinit(&n);
// }

TEST(ffx, u8_to_u32)
{
    setlocale(LC_ALL, "C.UTF-8");
    const uint8_t * u8 = (uint8_t *)u8"ϽϾϿ";// 03a7";
    uint32_t * u32 = NULL;//(uint32_t *)calloc(50, sizeof(uint32_t));
    uint8_t * u8_out = NULL;//(uint8_t *)calloc(50, sizeof(uint8_t));

    size_t len1 = u8_strlen(u8);
    size_t len2 = u8_mbsnlen(u8, len1);

    printf("len1 (%d), len2(%d)\n", len1, len2);

    uint32_t * s = u32;

     auto start= std::chrono::steady_clock::now();

    for (int i = 0;i < 1000000; i++) {
        size_t len = 20;
        u32 = u8_to_u32(u8,len1, NULL, &len);
        // printf("TEST %S %d\n",u32, len);
//        len = 20;
        u8_out = u32_to_u8(u32,u32_strlen(u32), NULL, &len);
        // printf("TEST %s %d\n",u8_out, len);
        free(u32);
        free(u8_out);
    }
    auto end = std::chrono::steady_clock::now();
        std::cerr << "\tSelect total: " << std::chrono::duration<double, std::milli>(end-start).count() << " ms " << std::endl;

    // printf(" %S %d\n",  u32, s == u32);
    
    // free(u32);


}