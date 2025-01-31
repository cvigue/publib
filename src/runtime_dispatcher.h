#ifndef CLVLIB_RUNTIME_DISPATCHER_H
#define CLVLIB_RUNTIME_DISPATCHER_H

#include <variant>
#include <tuple>
#include <type_traits>

namespace ClvLib {

namespace {
// Helper to extract parameter types of a function
template <typename T>
struct function_traits;

// Specialization for member functions
template <typename ReturnType, typename ClassType, typename... Args>
struct function_traits<ReturnType (ClassType::*)(Args...)>
{
    using arg_tuple = std::tuple<Args...>;
};

// Helper template to apply std::remove_reference to each type in the tuple
template <typename T>
struct remove_reference_tuple;

template <typename... Types>
struct remove_reference_tuple<std::tuple<Types...>>
{
    using type = std::tuple<typename std::remove_reference<Types>::type...>;
};

template <typename T>
using remove_reference_tuple_t = typename remove_reference_tuple<T>::type;
} // namespace

/**
    @brief Dispatcher macro using std::visit
    @details This macro is used to dispatch to the correct member function of the currently
    held variant type. It uses the FUNC_NAME to dispatch to find the correct member function
    and checks to ensure the correct argument types are passed to the member function. If
    the argument types are incorrect, a static_assert will be triggered. If the function is
    not found, a compile-time error will be triggered. If the function name is overloaded,
    in the target interface, the RT_DISPATCH_OVERLOAD macro should be used instead.

    RT_DISPATCH_OVERLOAD is much less strict about the argument types. Be careful when
    using it.
*/
#define RT_DISPATCH(FUNC_NAME)                                                                            \
    template <typename... ArgsT>                    \
    auto FUNC_NAME(ArgsT &&...args)                 \
    {                                               \
        return std::visit([&args...](auto &&v) {    \
            using func_type = decltype(&std::remove_reference_t<decltype(v)>::FUNC_NAME); \
            using func_arg_tuple = typename function_traits<func_type>::arg_tuple; \
            using args_tuple = remove_reference_tuple_t<std::tuple<ArgsT...>>; \
            static_assert(std::is_same_v<args_tuple, func_arg_tuple>, "Argument(s) not of expected type"); \
            return v.FUNC_NAME(std::forward<ArgsT>(args)...); }, \
                          *this);                   \
    }

/**
    @brief Dispatcher macro using std::visit with somewhat flexible argument types
    @details This macro is used to dispatch to the correct member function of the currently
    held variant type. It uses the FUNC_NAME to dispatch to the correct member function and
    tries forward the given arguments to the member function. If the function is not found,
    a compile-time error will be triggered. If the function name is overloaded, in the target
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


#if defined(__cpp_lib_reflection) && __cpp_lib_reflection >= 202207L

template <typename... Ts>
struct runtime_dispatch : std::variant<Ts...>
{
    using std::variant<Ts...>::variant;
    // TODO: Reflection magic here to get the member functions
    // Once we can get a list of function names we could use that list
    // to generate the RT_DISPATCH and RT_DISPATCH_OVERLOAD macros
    static_assert(false, "Reflection not yet implemented");
};

#else

template <typename... Ts>
using runtime_dispatch = std::variant<Ts...>;

#endif

} // namespace ClvLib

#endif //
