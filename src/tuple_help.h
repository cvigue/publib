
#pragma once

#include <cstddef>
#include <vector>
#include <tuple>

namespace ClvLib {

// Custom index sequence representing non-contiguous indices
template <std::size_t... Is>
struct custom_index_sequence
{
};

namespace {
// Helper template for generating an index sequence [Start, Start + N)
template <std::size_t Start, std::size_t N, std::size_t... Indices>
struct make_index_sequence_range_helper
    : make_index_sequence_range_helper<Start, N - 1, Start + N - 1, Indices...>
{
};

template <std::size_t Start, std::size_t... Indices>
struct make_index_sequence_range_helper<Start, 0, Indices...>
{
    using type = std::index_sequence<Indices...>;
};

template <typename T, std::size_t... Is>
auto vector_to_tuple_impl(const std::vector<T> &v, std::index_sequence<Is...>)
{
    return std::make_tuple(v[Is]...);
}

template <typename T, std::size_t... Is>
auto vector_to_tuple_impl_ref(const std::vector<T> &v, std::index_sequence<Is...>)
{
    return std::make_tuple(std::ref(v[Is])...);
}

template <typename T, std::size_t... Is>
auto vector_to_tuple_impl(const std::vector<T> &v, custom_index_sequence<Is...>)
{
    return std::make_tuple(v[Is]...);
}

template <typename T, std::size_t... Is>
auto vector_to_tuple_impl_ref(const std::vector<T> &v, custom_index_sequence<Is...>)
{
    return std::make_tuple(std::ref(v[Is])...);
}
} // namespace

/**
  @brief The purpose of make_index_sequence_range is to generate a compile-time integer sequence
  @tparam Start
  @tparam N

  The purpose of make_index_sequence_range is to generate a compile-time integer sequence from a start index to end
  index. It takes two template parameters:

  - Start - The starting index for the sequence.

  - N - The number of indices to generate.

  It produces a std::index_sequence of integer indices from Start to Start + N.

  For example, make_index_sequence_range<3, 5> would produce a sequence equivalent to std::index_sequence<3, 4, 5, 6, 7>.
  It does this by using a helper template struct called make_index_sequence_range_helper. This recursively generates
  the sequence by decrementing N and expanding the Indices parameter pack on each recursion. Once N reaches 0, it
  terminates the recursion and uses the generated Indices... as the result sequence.

  So make_index_sequence_range provides a simple way to generate a range of indices at compile time for use in other
  templates that may need to do something with a sequential range of values. It encapsulates the recursive logic needed
  to expand the index range. The generated sequence can then be used to parameterize other templates by passing it as
  an argument.
*/
template <std::size_t Start, std::size_t N>
using make_index_sequence_range = typename make_index_sequence_range_helper<Start, N>::type;

/**
  @brief The purpose of vector_to_tuple is to convert a std::vector into a std::tuple.
  @tparam seq The sequence of indexes to insert into the tuple
  @tparam T The type of things stored in the vector
  @param v the vector
  @return auto the tuple containing the specified things from the vector.

  The purpose of vector_to_tuple is to convert a std::vector into a std::tuple. It takes a std::vector as input, where
  T is any type. It produces a std::tuple as output, containing the elements from the input vector in the same order. It
  does this by using a helper function called vector_to_tuple_impl. This helper takes the vector and an integer sequence
  as inputs. The integer sequence is used to "unpack" the elements of the vector into the tuple. The sequence provides
  the indices to access each element.

  The make_index_sequence and make_index_sequence_range helpers are used to generate the integer sequence passed to
  vector_to_tuple_impl.

  So the main logic flow is:

  - Generate index sequence from 0 to vector size
  - Pass vector and index sequence to helper
  - Helper uses index sequence to unpack elements into tuple

  This allows the vector data to be converted into a tuple containing the same elements in order. It transforms the vector
  into a tuple through a sequence of indices. This is sometimes useful if a vector might contain known variables at known
  indexes, as one can then easily unpack the values into nicely named variables using a structured binding in one line
  of code.

  Templates are included to extract by reference or value, and to extract various index ranges or other sequences.
*/
template <typename T, std::size_t... Is>
auto vector_to_tuple(const std::vector<T> &v, custom_index_sequence<Is...>)
{
    return vector_to_tuple_impl(v, custom_index_sequence<Is...>{});
}

template <std::size_t N, typename T>
auto vector_to_tuple(const std::vector<T> &v)
{
    return vector_to_tuple_impl(v, std::make_index_sequence<N>{});
}

template <std::size_t Start, std::size_t N, typename T>
auto vector_to_tuple(const std::vector<T> &v)
{
    return vector_to_tuple_impl(v, make_index_sequence_range<Start, N>{});
}

template <std::size_t N, typename T>
auto vector_to_tuple_ref(const std::vector<T> &v)
{
    return vector_to_tuple_impl_ref(v, std::make_index_sequence<N>{});
}

template <std::size_t Start, std::size_t N, typename T>
auto vector_to_tuple_ref(const std::vector<T> &v)
{
    return vector_to_tuple_impl_ref(v, make_index_sequence_range<Start, N>{});
}
template <typename T, std::size_t... Is>
auto vector_to_tuple_ref(const std::vector<T> &v, custom_index_sequence<Is...>)
{
    return vector_to_tuple_impl_ref(v, custom_index_sequence<Is...>{});
}

/**
 * @brief Applies a function to each element of a tuple.
 * @details This function takes a callable object and a tuple, and applies the callable
 *          to each element of the tuple in order.
 * @tparam FN The type of the callable object.
 * @tparam ArgsT The types of the tuple elements.
 * @param fn The callable object to apply to each tuple element.
 * @param tup The tuple whose elements will be processed.
 * @note This function does not return a value; it is used for its side effects.
 * @example
 * std::tuple<int, double, std::string> myTuple(1, 3.14, "Hello");
 * apply_for_each([](const auto& elem) { std::cout << elem << std::endl; }, myTuple);
 */
template <typename FN, typename... ArgsT>
void apply_for_each(FN &&fn, const std::tuple<ArgsT...> &tup)
{
    std::apply([&fn](auto &&...args)
               { (fn(std::forward<decltype(args)>(args)), ...); },
               tup);
}

/**
 * @brief Applies a function to each element of a tuple and returns a new tuple
 *        with the results.
 * @details This function takes a callable object and a tuple, applies the callable
 *          to each element of the tuple, and returns a new tuple containing the results.
 * @tparam FN The type of the callable object.
 * @tparam ArgsT The types of the tuple elements.
 * @param fn The callable object to apply to each tuple element.
 * @param tup The tuple whose elements will be processed.
 * @return A new tuple containing the results of applying the function to each element.
 * @note The returned tuple has the same number of elements as the input tuple.
 * @example
 * std::tuple<int, double, std::string> myTuple(1, 3.14, "Hello");
 * auto result = apply_for_each_r([](const auto& elem) { return elem * 2; }, myTuple);
 * // result is now std::tuple<int, double, std::string>(2, 6.28, "HelloHello")
 */
template <typename FN, typename... ArgsT>
auto apply_for_each_r(FN &&fn, const std::tuple<ArgsT...> &tup)
{
    return std::apply([&fn](auto &&...args) -> std::tuple<ArgsT...>
                      { return std::tuple<ArgsT...>(fn(std::forward<decltype(args)>(args))...); },
                      tup);
}

// Helper to extract parameter types of a function
template <typename T>
struct function_traits;

// Specialization for member functions
template <typename ReturnType, typename ClassType, typename... Args>
struct function_traits<ReturnType (ClassType::*)(Args...)>
{
    using arg_tuple = std::tuple<Args...>;
};

// Helper alias template to apply a metafunction
template <typename T, template <typename> class MetaFun>
using apply_metafun_t = typename MetaFun<T>::type;

// Metafunction to transform a tuple
template <typename Tuple, template <typename> class MetaFun, std::size_t... Is>
auto tuple_transform_impl(Tuple &&t, std::index_sequence<Is...>)
{
    return std::make_tuple(apply_metafun_t<std::tuple_element_t<Is, std::remove_reference_t<Tuple>>, MetaFun>{}...);
}

/**
    @brief Transforms a tuple using a metaprogramming function.
    @tparam MetaFun meta-function that should be applied to each element of the tuple
    @tparam Tuple The source tuple type
    @param t tuple to transform
    @return auto transformed tuple
*/
template <template <typename> class MetaFun, typename Tuple>
auto tuple_transform(Tuple &&t)
{
    return tuple_transform_impl<Tuple, MetaFun>(std::forward<Tuple>(t),
                                                std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{});
}

/**
    @brief Removes reference from each element of a tuple
    @tparam Tuple
    @note This is a convenience alias for tuple_transform<std::remove_reference>
    that uses declval to get the type of the tuple in an unevaluated context.
*/
template <typename Tuple>
using remove_reference_tuple_t = decltype(tuple_transform<std::remove_reference>(std::declval<Tuple>()));

/**
    @brief Removes cv qual from each element of a tuple
    @tparam Tuple
    @note This is a convenience alias for tuple_transform<std::remove_cv>
    that uses declval to get the type of the tuple in an unevaluated context.
*/
template <typename Tuple>
using remove_cv_tuple_t = decltype(tuple_transform<std::remove_cv>(std::declval<Tuple>()));


} // namespace ClvLib