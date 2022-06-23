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

TEST(ffx, u8_to_u32)
{
    setlocale(LC_ALL, "C.UTF-8");
    const uint8_t * u8 = (uint8_t *)u8"ϽϾϿϽϾϿϽϾϿ";// 03a7";

    size_t len1 = u8_strlen(u8);
    size_t len2 = u8_mbsnlen(u8, len1);

    auto start= std::chrono::steady_clock::now();

    for (int i = 0;i < 1000000; i++) {
        uint32_t u32[20];
        uint8_t u8_out[20];
        uint8_t * u8_out_2(nullptr);
        uint32_t * u32_2(nullptr);
        size_t len = sizeof(u32) / sizeof(uint32_t);

        memset(u32, 0,sizeof(u32));
        memset(u8_out, 0,sizeof(u8_out));

        u32_2 = u8_to_u32(u8,len1, u32, &len);
        if (len > sizeof(u32) / sizeof(uint32_t)) {
            printf("free u32\n");
            free(u32_2);
        }
        len = sizeof(u8_out) / sizeof(uint8_t);
        u8_out_2 = u32_to_u8(u32_2,u32_strlen(u32_2), u8_out, &len);
        if (len > sizeof(u8_out) / sizeof(uint8_t))
        {
            printf("free u8\n");
            free(u8_out_2);
        }
    }
    auto end = std::chrono::steady_clock::now();
    std::cerr << "\tSelect total: " << std::chrono::duration<double, std::milli>(end-start).count() << " ms " << std::endl;



}