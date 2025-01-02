
#pragma once

#include <memory>

namespace ClvLib {

/**
 * @brief Make an existing type shared_from_this enabled via inheritance
 * @tparam BaseT1 base type to enable
 *
 * Provides the typical boilerplate associated with making shared_pointer intrusive in an easy
 * to reuse wrapper. Includes restricting the new type to heap-only instantiation and shared_ptr
 * duplication. Also includes a forwarding Create factory function.
 *
 * This template can be used easily to define a shared_from_this enabled type:
 *
 *      struct test_struct
 *      {
 *        test_struct() = default;
 *        test_struct(int a, int b)
 *            : i1(a), i2(b)
 *        {
 *        }
 *        int i1 = 42;
 *        int i2 = 43;
 *      };
 *
 *      using SharedTest = SharedThis<test_struct>;
 *
 *      auto s = SharedTest::Create(1, 2);
 */
template <typename BaseT1>
struct SharedThis : std::enable_shared_from_this<SharedThis<BaseT1>>, BaseT1
{
    using ptr = std::shared_ptr<SharedThis>;
    using base = BaseT1;

    template <typename... ArgsT>
    [[nodiscard]] static ptr Create(ArgsT &&...args) noexcept(noexcept(SharedThis(std::forward<ArgsT>(args)...)))
    {
        return ptr(new SharedThis(std::forward<ArgsT>(args)...));
    }
    ptr GetShared() noexcept // See: https://en.cppreference.com/w/cpp/memory/enable_shared_from_this/shared_from_this
    {
        return std::enable_shared_from_this<SharedThis<BaseT1>>::shared_from_this();
    }

  private:
    template <typename... ArgsT, typename = decltype(BaseT1(std::declval<ArgsT>()...))>
    SharedThis(ArgsT &&...args) noexcept(noexcept(BaseT1(std::forward<ArgsT>(args)...)))
        : BaseT1(std::forward<ArgsT>(args)...)
    {
    }
};
/**
 * @brief Produce a shared_from_this enabled instance of an existing type
 * @tparam BaseT1 base type to enable
 * @tparam ArgsT variadic args - forwarded to
 * @param args Arguments to forward to the BaseT1 constructor
 * @return SharedThis<BaseT1>::ptr shared_ptr to the instantiated object
 */
template <typename BaseT1, typename... ArgsT>
[[nodiscard]] inline typename SharedThis<BaseT1>::ptr make_intrusive(ArgsT &&...args) noexcept(noexcept(SharedThis<BaseT1>::Create(std::forward<ArgsT>(args)...)))
{
    return SharedThis<BaseT1>::Create(std::forward<ArgsT>(args)...);
}

} // namespace ClvLib
