
#include <gtest/gtest.h>


#include "ci_string.h"

using namespace ClvLib;

TEST(ci_string_tests, compare_same)
{
    auto s1 = ci_string("Hello World");
    auto s2 = ci_string("Hello World");

    EXPECT_EQ(s1, s2);
}

TEST(ci_string_tests, compare_similar)
{
    auto s1 = ci_string("Hello World!");
    auto s2 = ci_string("Hello World");

    EXPECT_NE(s1, s2);
}

TEST(ci_string_tests, compare_ne)
{
    auto s1 = ci_string("Some other string");
    auto s2 = ci_string("Hello World");

    EXPECT_NE(s1, s2);
}

TEST(ci_string_tests, compare_ci1)
{
    auto s1 = ci_string("hello World");
    auto s2 = ci_string("Hello World");

    EXPECT_EQ(s1, s2);
}

TEST(ci_string_tests, compare_ci2)
{
    auto s1 = ci_string("HELLO WORLD");
    auto s2 = ci_string("Hello World");

    EXPECT_EQ(s1, s2);
}

TEST(ci_string_tests, convert)
{
    auto s1 = ci_string("Hello World");
    auto s2 = to_string(s1);
    auto s3 = to_ci_str(s2);

    EXPECT_EQ(std::string("Hello World"), s2);
    EXPECT_EQ(s1, s3);
}

