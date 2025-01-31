#ifndef CLVLIB_RUNTIME_DISPATCHER_H
#define CLVLIB_RUNTIME_DISPATCHER_H

#include <variant>
#include <tuple>
#include <type_traits>

#include "tuple_help.h"

namespace ClvLib {


/**
    @brief Dispatcher macro using std::visit
    @details This macro is used to dispatch to the correct member function of the currently
    held variant type. It uses the FUNC_NAME to dispatch to the correct member function
    and checks to ensure the correct argument types are passed to the member function. If
    the argument types are incorrect a static_assert will be triggered. If the function is
    not found, a compile-time error will be triggered. If the function name is overloaded
    in the target interface, the RT_DISPATCH_OVERLOAD macro should be used instead as this 
    macro will consider the member function ambiguous. This macro is more strict about the
    argument types passed to the member function.

    RT_DISPATCH_OVERLOAD is much less strict about the argument types. Be careful when
    using it.
*/
#define RT_DISPATCH(FUNC_NAME)                      \
    template <typename... ArgsT>                    \
    auto FUNC_NAME(ArgsT &&...args)                 \
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
    @brief Dispatcher macro using std::visit with somewhat flexible argument types
    @details This macro is used to dispatch to the correct member function of the currently
    held variant type. It uses the FUNC_NAME to dispatch to the correct member function and
    tries forward the given arguments to the member function. If the function is not found,
    a compile-time error will be triggered. If the function name is overloaded in the target
    interface, the best match will be forwarded to, but things like implicit conversions
    can cause issues. Be careful when using this macro.
*/
#define RT_DISPATCH_OVERLOAD(FUNC_NAME)                                          \
    template <typename... ArgsT>                                                 \
    auto FUNC_NAME(ArgsT &&...args)                                              \
    {                                                                            \
        return std::visit([&args...](auto &&v)                                   \
                          { return v.FUNC_NAME(std::forward<ArgsT>(args)...); }, \
                          *this);                                                \
    }

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

#endif //
