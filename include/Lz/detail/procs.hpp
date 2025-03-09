#pragma once

#ifndef LZ_DETAIL_PROCS_HPP
#define LZ_DETAIL_PROCS_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/traits.hpp>
#include <cstddef>
#include <iterator>
#include <limits>
#include <tuple>

#ifndef NDEBUG
#include <exception>
#endif // NDEBUG

#if defined(__cpp_lib_stacktrace) && LZ_HAS_INCLUDE(<stacktrace>)
#include <stacktrace>
#endif

#include <exception>

namespace lz {
namespace detail {

[[noreturn]] inline void assertion_fail(const char* file, const int line, const char* func, const char* message) {
#if defined(__cpp_lib_stacktrace) && LZ_HAS_INCLUDE(<stacktrace>)
    auto st = std::stacktrace::current();
    auto str = std::to_string(st);
    std::fprintf(stderr, "%s:%d assertion failed in function '%s' with message:\n\t%s\nStacktrace:\n%s\n", file, line, func,
                 message, str.c_str());
#else
    std::fprintf(stderr, "%s:%d assertion failed in function '%s' with message:\n\t%s\n", file, line, func, message);
#endif
    std::terminate();
}

#define LZ_ASSERT(CONDITION, MSG) ((CONDITION) ? ((void)0) : (lz::detail::assertion_fail(__FILE__, __LINE__, __func__, MSG)))

template<class Iterable>
LZ_NODISCARD constexpr auto begin(Iterable&& c) noexcept(noexcept(std::forward<Iterable>(c).begin()))
    -> enable_if<!std::is_array<remove_reference_t<Iterable>>::value, decltype(std::forward<Iterable>(c).begin())> {
    return std::forward<Iterable>(c).begin();
}

template<class Iterable>
LZ_NODISCARD constexpr auto end(Iterable&& c) noexcept(noexcept(std::forward<Iterable>(c).end()))
    -> enable_if<!std::is_array<remove_reference_t<Iterable>>::value, decltype(std::forward<Iterable>(c).end())> {
    return std::forward<Iterable>(c).end();
}

template<class Iterable>
LZ_NODISCARD constexpr auto begin(Iterable&& c) noexcept(noexcept(std::begin(c)))
    -> enable_if<std::is_array<remove_reference_t<Iterable>>::value, decltype(std::begin(c))> {
    return std::begin(c);
}

template<class Iterable>
LZ_NODISCARD constexpr auto end(Iterable&& c) noexcept(noexcept(std::end(c)))
    -> enable_if<std::is_array<remove_reference_t<Iterable>>::value, decltype(std::end(c))> {
    return std::end(c);
}

template<class Arithmetic>
LZ_CONSTEXPR_CXX_14 bool is_even(const Arithmetic value) noexcept {
    return (value % 2) == 0;
}

template<class... Ts>
constexpr void decompose(const Ts&...) noexcept {
}

template<class Result, class Arithmetic>
LZ_CONSTEXPR_CXX_14 Result round_even(const Arithmetic a, const Arithmetic b) noexcept {
    LZ_ASSERT(a != 0 && b != 0, "division by zero error");
    if (b == 1) {
        return static_cast<Result>(a);
    }
    if (b == -1) {
        return -static_cast<Result>(a);
    }
    if (is_even(a) && is_even(b)) {
        return static_cast<Result>(a / b);
    }
    return static_cast<Result>(a / b) + 1;
}

template<class Iterator, class S>
LZ_CONSTEXPR_CXX_14 diff_type<Iterator> distance_impl(Iterator begin, S end, std::forward_iterator_tag) {
    diff_type<Iterator> dist = 0;
    for (; begin != end; ++begin, ++dist) {
    }
    return dist;
}

template<class Iterator, class S>
constexpr diff_type<Iterator> distance_impl(Iterator begin, S end, std::random_access_iterator_tag) {
    return end - begin;
}
} // namespace detail

LZ_MODULE_EXPORT_SCOPE_BEGIN

/**
 * @brief Gets the distance of an iterable. If iterable is not random access, the operation will be O(n), otherwise O(1).
 *
 * @param begin The beginning of the iterable
 * @param end The end of the iterable
 * @return The length of the iterable
 */
template<class Iterator, class S>
constexpr diff_type<Iterator> distance(Iterator begin, S end) {
    return detail::distance_impl(std::move(begin), std::move(end), iter_cat_t<Iterator>{});
}

#ifdef LZ_HAS_CXX_17

/**
 * @brief Gets the size of a container or iterable if it has a .size() method. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * std::cout << lz::size(vec) << '\n'; // prints 5
 * ```
 *
 * @param i The container to get the size from.
 * @return The size of the container.
 */
template<class Iterable>
LZ_NODISCARD constexpr auto size(const Iterable& i) noexcept(noexcept(std::size(i))) -> decltype(std::size(i)) {
    static_assert(sized<Iterable>::value, "Iterable must be sized/contain a .size() method");
    return std::size(i);
}

#else

/**
 * @brief Gets the size of a container or iterable if it has a .size() method. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * std::cout << lz::size(vec) << '\n'; // prints 5
 * ```
 *
 * @param c The container to get the size from.
 * @return The size of the container.
 */
template<class Iterable>
LZ_NODISCARD constexpr auto size(const Iterable& i) noexcept(noexcept(i.size())) -> decltype(i.size()) {
    static_assert(sized<Iterable>::value, "Iterable must be sized/contain a .size() method");
    return i.size();
}

/**
 * @brief Gets the size of a container or iterable if it has a .size() method. Example:
 * ```cpp
 * int arr[] = { 1, 2, 3, 4, 5 };
 * std::cout << lz::size(arr) << '\n'; // prints 5
 * ```
 *
 * @param c The container to get the size from.
 * @return The size of the container.
 */
template<class T, size_t N>
LZ_NODISCARD constexpr std::size_t size(const T (&)[N]) noexcept {
    return N;
}

#endif

/**
 * @brief Gets the size of an iterable. If the iterable is not sized, it will calculate the size by iterating over the iterable,
 * which is O(n). Example:
 * ```cpp
 * // Sized, making it O(1)
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * std::cout << lz::eager_size(vec) << '\n'; // prints 5
 *
 * // Not sized, making it O(n)
 * auto not_sized = lz::c_string("Hello");
 * std::cout << lz::eager_size(not_sized) << '\n'; // prints 5
 * ```
 *
 * @param c The iterable to get the size of.
 * @return The size of the iterable.
 */
template<class Iterable>
LZ_NODISCARD constexpr auto eager_size(Iterable&& c) -> detail::enable_if<detail::sized<Iterable>::value, decltype(lz::size(c))> {
    return lz::size(c);
}

/**
 * @brief Gets the size of an iterable. If the iterable is not sized, it will calculate the size by iterating over the iterable,
 * which is O(n), unless it is random access. Example:
 * ```cpp
 * // Sized, making it O(1)
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * std::cout << lz::eager_size(vec) << '\n'; // prints 5
 *
 * // Not sized, not random access, making it O(n)
 * auto not_sized = lz::c_string("Hello");
 * std::cout << lz::eager_size(not_sized) << '\n'; // prints 5
 * ```
 *
 * @param c The iterable to get the size of.
 * @return The size of the iterable.
 */
template<class Iterable>
LZ_NODISCARD constexpr detail::enable_if<!detail::sized<Iterable>::value, std::size_t> eager_size(Iterable&& c) {
    using std::distance;
    return static_cast<std::size_t>(distance(std::begin(c), std::end(c)));
}

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz
#endif // LZ_DETAIL_PROCS_HPP
