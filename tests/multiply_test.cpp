
#include <gtest/gtest.h>

TEST(MultiplyTests, TestIntegerZero_Zero)
{
    const auto expected = 0;
    const auto actual = 0;
    ASSERT_EQ(expected, actual);
}

TEST(MultiplyTests, TestIntegerZero_One)
{
    const auto expected = 1;
    const auto actual = 0;
    ASSERT_EQ(expected, actual);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}