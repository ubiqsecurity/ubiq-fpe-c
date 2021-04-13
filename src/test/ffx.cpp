#include <gtest/gtest.h>
#include <ubiq/fpe/internal/ffx.h>

#include <string.h>

TEST(ffx, revs)
{
    char s[] = "abcde";
    ASSERT_EQ(s, ffx_revs(s));
    EXPECT_EQ(strcmp(s, "edcba"), 0);

    s[strlen(s) - 1] = '\0';
    ASSERT_EQ(s, ffx_revs(s));
    EXPECT_EQ(strcmp(s, "bcde"), 0);
}

TEST(ffx, numr)
{
    char n2[] = "10000000";
    char n5[] = "00011010";

    bigint_t n;

    bigint_init(&n);

    EXPECT_EQ(ffx_num2(&n, n2), 0);
    EXPECT_EQ(bigint_cmp_si(&n, 128), 0);

    EXPECT_EQ(ffx_numr(&n, n5, 5), 0);
    EXPECT_EQ(bigint_cmp_si(&n, 755), 0);

    bigint_deinit(&n);
}

TEST(ffx, str)
{
    char s[5];
    bigint_t n;

    bigint_init(&n);
    bigint_set_str(&n, "559", 10);

    EXPECT_EQ(ffx_str(s, 4, 12, &n), 0);
    EXPECT_EQ(strcmp(s, "03a7"), 0);

    bigint_deinit(&n);
}
