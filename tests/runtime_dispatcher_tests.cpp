
#include <gtest/gtest.h>

#include <runtime_dispatcher.h>

#include <vector>
#include <string>

struct A
{
    A(int i_)
        : i(i_)
    {
    }
    void f()
    {
    }
    void f1(int)
    {
    }
    double f2(double d)
    {
        return d / 2;
    }
    double f3(int ii, double dd)
    {
        return ii * dd + i;
    }
    void f4(const std::string &s)
    {
    }
    void f5(const std::string &&s)
    {
    }

  private:
    int i;
};

struct B
{
    void f()
    {
    }
    void f1(int)
    {
    }
    double f2(double d)
    {
        return d;
    }
    double f3(int ii, double dd)
    {
        return ii * dd;
    }
    void f4(const std::string &s)
    {
    }
    void f5(const std::string &&s)
    {
    }
    void fn() // should be fine
    {
    }
};

using namespace ClvLib;

struct rtab : runtime_dispatch<A, B>
{
    using runtime_dispatch<A, B>::runtime_dispatch;

    RT_DISPATCH_OVERLOAD(f)
    RT_DISPATCH(f1)
    RT_DISPATCH(f2)
    RT_DISPATCH(f3)
    RT_DISPATCH(f4)
    RT_DISPATCH(f5)
};

TEST(runtime_dispatcher, vectorize)
{
    std::vector<rtab> vec{A(2), B()};

    std::cout << "sizeof(rtab) " << sizeof(rtab) << "\n";

    for (auto &&v : vec)
        v.f();

    for (auto &&v : vec)
        v.f1(42);

    double d = 98.6;
    for (auto &&v : vec)
        d = v.f2(d);

    d = 98.6;
    for (auto &&v : vec)
        d = v.f3(42, d);
}

TEST(runtime_dispatcher, singleA)
{
    rtab r{A(2)};

    r.f();
    r.f1(42);
    EXPECT_EQ(r.f2(98.6), 49.3);
    EXPECT_EQ(r.f3(42, 98.6), 98.6 * 42 + 2);
    r.f4(std::string("hello"));
    r.f5(std::string("hello"));
}

TEST(runtime_dispatcher, singleB)
{
    rtab r{B()};

    r.f();
    r.f1(42);
    EXPECT_EQ(r.f2(98.6), 98.6);
    EXPECT_EQ(r.f3(42, 98.6), 98.6 * 42);
    auto str = std::string("hello");
    r.f4(str);
    r.f5(std::move(str)); // forces rval ref properly
}

// TODO: better UTs