#ifndef CLVLIB_RUNTIME_DISPATCHER_H
#define CLVLIB_RUNTIME_DISPATCHER_H

#include <variant>
#include <tuple>
#include <type_traits>

#include "tuple_help.h"

namespace ClvLib {

/**
    @brief Implementation macro for dispatching using std::visit
    @details Dispatches to the correct member function of the currently held variant type with strict argument type checking.
             The CVQ parameter specifies const/noexcept qualifiers.
*/
#define RT_DISPATCHIMPL(FUNC_NAME, CVQ)             \
    template <typename... ArgsT>                    \
    auto FUNC_NAME(ArgsT &&...args) CVQ             \
    {                                               \
        return std::visit([&args...](auto &&v) {    \
            using func_type = decltype(&std::remove_reference_t<decltype(v)>::FUNC_NAME); \
            using func_arg_tuple = remove_cv_tuple_t<remove_reference_tuple_t<typename function_traits<func_type>::arg_tuple>>; \
            using args_tuple = remove_cv_tuple_t<remove_reference_tuple_t<std::tuple<ArgsT...>>>; \
            static_assert(std::is_same_v<args_tuple, func_arg_tuple>, "Argument(s) not of expected type"); \
            return v.FUNC_NAME(std::forward<ArgsT>(args)...); }, \
                          *this);                   \
    }

/**
    @brief Dispatcher macro using std::visit for non-const member functions
    @details Dispatches to the correct non-const member function of the currently held variant type.
             Strict argument type checking is enforced via static_assert.
*/
#define RT_DISPATCH(FUNC_NAME) RT_DISPATCHIMPL(FUNC_NAME, /* empty CVQ */)

/**
    @brief Variants of RT_DISPATCH with specific qualifiers
*/
#define RT_DISPATCH_CONST(FUNC_NAME) RT_DISPATCHIMPL(FUNC_NAME, const)
#define RT_DISPATCH_NOEXCEPT(FUNC_NAME) RT_DISPATCHIMPL(FUNC_NAME, noexcept)
#define RT_DISPATCH_CNE(FUNC_NAME) RT_DISPATCHIMPL(FUNC_NAME, const noexcept)

/**
    @brief Implementation macro for dispatching with flexible argument types
    @details Dispatches to the correct member function with looser argument type checking.
             The CVQ parameter specifies const/noexcept qualifiers.
*/
#define RT_DISPATCH_OVERLOADIMPL(FUNC_NAME, CVQ)                                                      \
    template <typename... ArgsT>                                                                      \
    auto FUNC_NAME(ArgsT &&...args) CVQ                                                               \
    {                                                                                                 \
        return std::visit([&args...](auto &&v) { return v.FUNC_NAME(std::forward<ArgsT>(args)...); }, \
                          *this);                                                                     \
    }

/**
    @brief Dispatcher macro using std::visit with flexible argument types
    @details Dispatches to the correct member function (const or non-const) with looser argument type checking.
*/
#define RT_DISPATCH_OVERLOAD(FUNC_NAME) RT_DISPATCH_OVERLOADIMPL(FUNC_NAME, /* empty CVQ */)

/**
    @brief Variants of RT_DISPATCH_OVERLOAD with specific qualifiers
*/
#define RT_DISPATCH_OVERLOAD_CONST(FUNC_NAME) RT_DISPATCH_OVERLOADIMPL(FUNC_NAME, const)
#define RT_DISPATCH_OVERLOAD_NOEXCEPT(FUNC_NAME) RT_DISPATCH_OVERLOADIMPL(FUNC_NAME, noexcept)
#define RT_DISPATCH_OVERLOAD_CNE(FUNC_NAME) RT_DISPATCH_OVERLOADIMPL(FUNC_NAME, const noexcept)

// TODO: Reflection magic here to get the member functions
// Once we can get a list of function names we could use that list
// to generate the RT_DISPATCH and RT_DISPATCH_OVERLOAD macros

template <typename... Ts>
struct runtime_dispatch : std::variant<Ts...>
{
    using types = std::tuple<Ts...>;
    using std::variant<Ts...>::variant;
};

} // namespace ClvLib

#endif // CLVLIB_RUNTIME_DISPATCHER_H