
#include <gtest/gtest.h>

#include "cbmi.h"

#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

using namespace ClvLib;

// ============================================================================
// Define the Duck interface
// ============================================================================

// Pure virtual API needs to be added for this Duck concept
template <typename...>
struct DuckConcept
{
    virtual std::string name() const noexcept = 0;
    virtual std::string walk() const noexcept = 0;
    virtual std::string quack() const noexcept = 0;
};
// Delegating API boilerplate needs to be added for model injection
template <typename BaseT>
struct DuckDelegate : BaseT
{
    using BaseT::BaseT;
    std::string name() const noexcept
    {
        return BaseT::DelegateTo().name();
    }
    std::string walk() const noexcept
    {
        return BaseT::DelegateTo().walk();
    }
    std::string quack() const noexcept
    {
        return BaseT::DelegateTo().quack();
    }
};

// Define what it is to be a Duck
using Duck = Poly<DuckConcept, DuckDelegate>;

// ============================================================================
// Above work is shared for all types that conform to the Duck interface
// Implement various types of ducks below
// ============================================================================

struct DuckA
{
    std::string name() const noexcept
    {
        return "DuckA";
    }
    std::string walk() const noexcept
    {
        return "walks like a duck";
    }
    std::string quack() const noexcept
    {
        return "quacks like a duck";
    }
};

struct DuckB
{
    std::string name() const noexcept
    {
        return "DuckB";
    }
    std::string walk() const noexcept
    {
        return "walks like a duck";
    }
    std::string quack() const noexcept
    {
        return "quacks like a duck";
    }
};

struct DuckC
{
    DuckC(int i)
        : mI(i){};
    DuckC(const DuckC &) = default;
    DuckC(DuckC &&) = default;

    std::string name() const noexcept
    {
        return "DuckC[" + std::to_string(mI) + "]";
    }
    std::string walk() const noexcept
    {
        return "walks like a duck";
    }
    std::string quack() const noexcept
    {
        return "quacks like a duck";
    }
    void swim(){}; // additional optional

  private:
    int mI;
};

// ============================================================================
// Duck Tests
// ============================================================================

TEST(ImplCBMI, DuckTypeVector)
{
    std::vector<Duck> ducks;

    ducks.emplace_back(DuckA());
    ducks.push_back(DuckB());
    ducks.push_back(DuckC(42));

    for (auto &&duck : ducks)
    {
        EXPECT_EQ(duck.walk(), "walks like a duck");
        EXPECT_EQ(duck.quack(), "quacks like a duck");
        std::cout << duck.name() << " " << duck.walk() << " and " << duck.quack() << "\n";
    }
}

TEST(ImplCBMI, DuckTypeVector2)
{
    std::vector<Duck> ducks;

    auto duckA = DuckA();
    auto duckB = DuckB();
    auto duckC = DuckC(42);

    ducks.push_back(&duckA); // Insert pointer to
    ducks.push_back(&duckB);
    ducks.push_back(&duckC);
    ducks.push_back(&duckA); // Insert pointer to
    ducks.push_back(&duckB);
    ducks.push_back(&duckC);
    ducks.pop_back();       // Delete non-owning entry
    ducks.push_back(duckA); // Copy
    ducks.push_back(duckB);
    ducks.push_back(duckC);
    ducks.push_back(duckA); // Copy again
    ducks.push_back(duckB);
    ducks.push_back(duckC);
    ducks.push_back(DuckA()); // Move
    ducks.push_back(DuckB());
    ducks.push_back(DuckC(96));

    for (auto &&duck : ducks)
    {
        EXPECT_EQ(duck.walk(), "walks like a duck");
        EXPECT_EQ(duck.quack(), "quacks like a duck");
        std::cout << duck.name() << " " << duck.walk() << " and " << duck.quack() << "\n";
    }
}

TEST(ImplCBMI, DuckTypeUnorderedMap)
{
    std::unordered_map<uint64_t, Duck> ducks;

    auto duckA = DuckA();
    const auto duckB = DuckB();
    auto duckC = DuckC(96);

    ducks.insert_or_assign(0, DuckA());
    ducks.insert_or_assign(1, DuckB());
    ducks.insert_or_assign(2, DuckC(42));
    ducks.insert_or_assign(3, &duckA);
    ducks.insert_or_assign(4, &duckB);
    ducks.insert_or_assign(5, &duckC);

    ducks.erase(0);
    ducks.erase(4);

    for (auto &&[key, duck] : ducks)
    {
        EXPECT_EQ(duck.walk(), "walks like a duck");
        EXPECT_EQ(duck.quack(), "quacks like a duck");
        std::cout << duck.name() << " " << duck.walk() << " and " << duck.quack() << "\n";
    }
}

TEST(ImplCBMI, DuckTypeVectorSharedPtr)
{
    using DuckPtr = std::shared_ptr<Duck>;
    std::vector<DuckPtr> ducks;

    ducks.emplace_back(new Duck(DuckA()));
    ducks.emplace_back(new Duck(DuckB()));
    ducks.emplace_back(new Duck(DuckC(42)));
    for (auto &&duck : ducks)
    {
        EXPECT_EQ(duck->walk(), "walks like a duck");
        EXPECT_EQ(duck->quack(), "quacks like a duck");
    }
}

TEST(ImplCBMI, DuckType)
{
    auto dp1 = std::make_shared<DuckC>(96);
    EXPECT_EQ(dp1->walk(), "walks like a duck");
    EXPECT_EQ(dp1->quack(), "quacks like a duck");
}

TEST(ImplCBMI, DuckTypePolyAssign)
{
    Duck d1 = DuckC(96);
    EXPECT_EQ(d1.walk(), "walks like a duck");
    EXPECT_EQ(d1.quack(), "quacks like a duck");

    Duck d2 = DuckC(96);
    EXPECT_EQ(d1.name(), d2.name());

    Duck d3 = DuckB();
    d2 = d3;
    EXPECT_NE(d1.name(), d2.name());
}

TEST(ImplCBMI, DuckTypeConstPolyAssign)
{
    const auto di1 = DuckC(96);     // Create implementation
    Duck d1 = di1;                  // Make a deep copy
    const Duck d2 = std::move(di1); // Now move --> di1 is no longer valid
    EXPECT_EQ(d1.walk(), "walks like a duck");
    EXPECT_EQ(d1.quack(), "quacks like a duck");
    EXPECT_EQ(d2.walk(), "walks like a duck");
    EXPECT_EQ(d2.quack(), "quacks like a duck");
}

TEST(ImplCBMI, DuckTypeImplAssign)
{
    Duck d1 = DuckA();
    d1 = DuckB();
    EXPECT_EQ(d1.name(), "DuckB");
}

// ============================================================================
// HOWTO: Create a generic Poly
// Define a generic interface
// ============================================================================

template <typename TypeT>
struct GenConcept
{
    virtual TypeT proc(TypeT) = 0;
};

template <typename BaseT, typename TypeT>
struct GenDelegate : BaseT
{
    using BaseT::BaseT;
    TypeT proc(TypeT t)
    {
        return BaseT::DelegateTo().proc(t);
    }
};

/**
    @brief Alias template for Poly class with specific concept, model and interface types
    @tparam TypeT The type parameter for the various Gen instantiations
    @details This shows how to define generic Concept, Model, and Interface that is able
    to accept template parameters that are passed through to them via the Poly aliased
    wrapper. Although this example shows a single template argument the Poly wrapper
    supports an arbitrary number of template arguments passing through. See the 'Arg'
    tests below for that.
*/
template <typename TypeT>
using Gen = Poly<GenConcept, GenDelegate, TypeT>;

// ============================================================================
// Above work is shared for all types that conform to the Gen interface
// Implement various generics below
// ============================================================================

using GenInt = Gen<int>;

struct Gen2Int
{
    int proc(int i)
    {
        return i * 2;
    }
};

struct Gen3Int
{
    int proc(int i)
    {
        return i * 3;
    }
};

template <typename TypeT>
struct Gen4
{
    TypeT proc(TypeT i)
    {
        return i * 4;
    }
};

// ============================================================================
// Gen Tests
// ============================================================================

TEST(ImplCBMI, GenIntVector)
{
    std::vector<GenInt> iv;

    iv.push_back(Gen2Int());
    iv.push_back(Gen3Int());
    iv.push_back(Gen4<int>());

    for (auto &&gen : iv)
    {
        EXPECT_EQ(gen.proc(3), gen.proc(1) * 3);
    }
}

TEST(ImplCBMI, GenIntPolyAssign)
{
    GenInt g1 = Gen2Int();
    GenInt g2 = Gen3Int();
    EXPECT_EQ(g1.proc(1), 2);
    EXPECT_EQ(g2.proc(1), 3);
    g1 = g2;
    EXPECT_EQ(g1.proc(1), 3);
}

// ============================================================================
// HOWTO: Create a generic Poly with multiple template args
// Define a generic interface
// ============================================================================

template <typename Arg1T, typename Arg2T, typename RetT>
struct ArgConcept
{
    virtual RetT proc(Arg1T, Arg2T) = 0;
};

template <typename BaseT, typename Arg1T, typename Arg2T, typename RetT>
struct ArgDelegate : BaseT
{
    using BaseT::BaseT;
    RetT proc(Arg1T a1, Arg2T a2)
    {
        return BaseT::DelegateTo().proc(a1, a2);
    }
};

template <typename Arg1T, typename Arg2T, typename RetT>
using Arg = Poly<ArgConcept, ArgDelegate, Arg1T, Arg2T, RetT>;

// ============================================================================
// Above work is shared for all types that conform to the Gen interface
// Implement various generics below
// ============================================================================

using ArgDis = Arg<double, int, std::string>;

struct Arg1
{
    std::string proc(double d, int i)
    {
        return std::to_string(i * d);
    }
};

struct Arg2
{
    std::string proc(double d, int i)
    {
        return std::to_string(i * 2 * d);
    }
};

template <typename Arg1T, typename Arg2T, typename RetT>
struct ArgX
{
    RetT proc(Arg1T a1, Arg2T a2)
    {
        return std::to_string(a2 * 2 * a1);
    }
};

// ============================================================================
// Gen Tests
// ============================================================================

TEST(ImplCBMI, ArgVector)
{
    std::vector<ArgDis> av;

    av.push_back(Arg1());
    av.push_back(Arg2());
    av.push_back(ArgX<double, int, std::string>());

    for (auto &&arg : av)
    {
        EXPECT_EQ((arg.proc(3, 7)), (arg.proc(1, 3 * 7)));
    }
}

// ============================================================================
// Using loose functions as the implementation
// Use the framework to create a Sean Parent style polymorphic object family
// with behaviors defined by loose functions rather than member functions.
// The purpose of this code is to demonstrate how the PolyCMI template can be
// used to create a polymorphic object family with behaviors defined by loose
// functions, rather than member functions. This approach can be useful in
// situations where you want to separate the implementation of an object's
// behavior from its definition, or when you want to provide a default
// implementation for a behavior that can be overridden by a custom
// implementation.
// ============================================================================

// ============================================================================
// C++ '20 Concept to make checking if a type conforms to the required interface
// simpler
template <typename IsDrawableT>
concept IsDrawable = requires(IsDrawableT d) { d.draw(); };

// ============================================================================
// The concept
template <typename...>
struct DrawConcept
{
    virtual std::string draw() const noexcept = 0;
};

// ============================================================================
// The interface delegator.
template <typename BaseT>
struct DrawInterface : BaseT
{
    using BaseT::BaseT;
    std::string draw() const noexcept
    {
        return BaseT::DelegateTo().draw();
    }
};

// ============================================================================
// The model delegates to loose functions, not member functions.
template <typename BaseT>
struct DrawModel : BaseT
{
    using BaseT::BaseT;
    std::string draw() const noexcept final
    {
        return draw_fn(BaseT::DelegateTo());
    }
};

// ============================================================================
// Slightly more complex alias due to final implementation being non-member
// functions.
using Draw = PolyCMI<DrawConcept, DrawInterface, DrawModel>;

// ============================================================================
// This allows all intrinsically drawable to use their builtin .draw()
// functions as a fallback. It will be called by the Model delegator.
template <typename DrawableT>
    requires IsDrawable<DrawableT>
std::string draw_fn(const DrawableT &d)
{
    return d.draw();
}

// ============================================================================
// No fallback for drawing Square; see std::string draw_fn(const Square &)
struct Square
{
};

// ============================================================================
// Both Circle and Line implement drawing in member functions, as enabled above
// by the function template std::string draw_fn(const DrawableT &d)
struct Circle
{
    std::string draw() const noexcept
    {
        return "MemberCircle";
    }
};
struct Line
{
    std::string draw() const noexcept
    {
        return "MemberLine";
    }
};

// ============================================================================
// This is the draw function for Square, called by the Model delegator. Square
// would not have a way to be drawn without this.
std::string draw_fn(const Square &)
{
    return "Square";
}

// ============================================================================
// This redefines how to draw a 'Line' even though 'line' is already drawable
// via the std::string draw_fn(const DrawableT &d) template / trampoline
// above. This function will be selected due to being a better match.
std::string draw_fn(const Line &)
{
    return "Line";
}

// ============================================================================
TEST(ImplCBMI, draw_loose_impl)
{
    std::vector<Draw> dv;
    dv.push_back(Circle());
    dv.push_back(Square());
    dv.push_back(Circle());
    dv.push_back(Line());
    dv.push_back(dv[1]);

    std::string out;

    for (auto &&shape : dv)
    {
        out.append(shape.draw());
        out.append("\n");
    }

    EXPECT_EQ("MemberCircle\nSquare\nMemberCircle\nLine\nSquare\n", out);
}
