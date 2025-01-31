
#include "gtest/gtest.h"

#include <tuple_help.h>

#include <string>
#include <sstream>

using namespace std::literals;

using namespace ClvLib;

struct dbl
{
    template <typename T>
    auto operator()(T t)
    {
        return t + t;
    }
};

TEST(ExpressionHelpSuite, use_structured_binding)
{
    auto v = std::vector<int>({0, 1, 2});
    auto [zero, one, two] = vector_to_tuple<3>(v);
    EXPECT_EQ(zero, 0);
    EXPECT_EQ(one, 1);
    EXPECT_EQ(two, 2);
}

TEST(ExpressionHelpSuite, use_structured_binding_trunc)
{
    auto v = std::vector({0, 1, 2, 3, 4});
    auto [zero, one, two] = vector_to_tuple<3>(v);
    EXPECT_EQ(zero, 0);
    EXPECT_EQ(one, 1);
    EXPECT_EQ(two, 2);
}

TEST(ExpressionHelpSuite, use_structured_binding_range)
{
    auto v = std::vector({0, 1, 2, 3, 4});
    auto [two, three] = vector_to_tuple<2, 2>(v);
    EXPECT_EQ(two, 2);
    EXPECT_EQ(three, 3);
}

TEST(ExpressionHelpSuite, use_structured_binding_pick)
{
    auto v = std::vector({0, 1, 2, 3, 4});
    auto [zero, two, three] = vector_to_tuple(v, custom_index_sequence<0, 2, 3>{});
    EXPECT_EQ(zero, 0);
    EXPECT_EQ(two, 2);
    EXPECT_EQ(three, 3);
}

TEST(ExpressionHelpSuite, use_structured_binding_ref)
{
    auto v = std::vector<int>({0, 1, 2});
    auto [zero, one, two] = vector_to_tuple_ref<3>(v);
    EXPECT_EQ(zero, 0);
    EXPECT_EQ(one, 1);
    EXPECT_EQ(two, 2);
}

TEST(ExpressionHelpSuite, use_structured_binding_trunc_ref)
{
    auto v = std::vector({0, 1, 2, 3, 4});
    auto [zero, one, two] = vector_to_tuple_ref<3>(v);
    EXPECT_EQ(zero, 0);
    EXPECT_EQ(one, 1);
    EXPECT_EQ(two, 2);
}

TEST(ExpressionHelpSuite, use_structured_binding_range_ref)
{
    auto v = std::vector({0, 1, 2, 3, 4});
    auto [two, three] = vector_to_tuple_ref<2, 2>(v);
    EXPECT_EQ(two, 2);
    EXPECT_EQ(three, 3);
}

TEST(ExpressionHelpSuite, use_structured_binding_pick_ref)
{
    auto v = std::vector({0, 1, 2, 3, 4});
    auto [zero, two, three] = vector_to_tuple_ref(v, custom_index_sequence<0, 2, 3>{});
    EXPECT_EQ(zero, 0);
    EXPECT_EQ(two, 2);
    EXPECT_EQ(three, 3);
}

TEST(ExpressionHelpSuite, for_each_tuple_element)
{
    auto tup1 = std::tuple(1, 2, 3, "Hello"s, 3.14);
    auto tup2 = apply_for_each_r(dbl(), tup1);
    EXPECT_EQ(std::get<0>(tup2), 2);
    EXPECT_EQ(std::get<1>(tup2), 4);
    EXPECT_EQ(std::get<2>(tup2), 6);
    EXPECT_EQ(std::get<3>(tup2), "HelloHello"s);
    EXPECT_EQ(std::get<4>(tup2), 6.28);
    std::stringstream str;
    apply_for_each([&str](auto a)
                   { str << a; },
                   tup2);
    EXPECT_EQ(str.str(), "246HelloHello6.28"s);
}

TEST(ExpressionHelpSuite, tuple_transform)
{
    using tup1 = std::tuple<int, int &, double>;
    using tup2 = remove_reference_tuple_t<tup1>;
    using tup3 = std::tuple<int, int, double>;

    EXPECT_FALSE((std::is_same_v<tup1, tup2>));
    EXPECT_FALSE((std::is_same_v<tup1, tup3>));
    EXPECT_TRUE((std::is_same_v<tup2, tup3>));
}