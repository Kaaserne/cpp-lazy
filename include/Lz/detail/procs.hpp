#pragma once

#ifndef LZ_DETAIL_PROCS_HPP
#define LZ_DETAIL_PROCS_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/traits.hpp>
#include <cstddef>
#include <iterator>

// clang-format off
#if !defined(NDEBUG) || defined(LZ_DEBUG_ASSERTIONS)
  #define LZ_USE_DEBUG_ASSERTIONS
#endif

#if defined(LZ_USE_DEBUG_ASSERTIONS)
  #include <cstdio>
  #include <exception>
  #if defined(LZ_HAS_CXX_23) && LZ_HAS_INCLUDE(<stacktrace>)
    #include <stacktrace>
  #endif
#endif
// clang-format on

namespace lz {
namespace detail {

#if defined(LZ_USE_DEBUG_ASSERTIONS)

[[noreturn]] inline void
assertion_fail(const char* file, const int line, const char* func, const char* message, const char* expr) {
#if defined(__cpp_lib_stacktrace) && LZ_HAS_INCLUDE(<stacktrace>)
    
    auto st = std::stacktrace::current();
    auto str = std::to_string(st);
    std::fprintf(stderr, "%s:%d assertion \"%s\" failed in function '%s' with message:\n\t%s\nStacktrace:\n%s\n", file, line,
                 expr, func, message, str.c_str());
    
#else // ^^ defined(__cpp_lib_stacktrace) vv !defined(__cpp_lib_stacktrace)

    std::fprintf(stderr, "%s:%d assertion \"%s\" failed in function '%s' with message:\n\t%s\n", file, line, expr, func, message);

#endif // __cpp_lib_stacktrace
    
    std::terminate();
}

#define LZ_ASSERT(CONDITION, MSG)                                                                                                \
    ((CONDITION) ? (static_cast<void>(0)) : (lz::detail::assertion_fail(__FILE__, __LINE__, __func__, MSG, #CONDITION)))

#else // ^^ defined(LZ_USE_DEBUG_ASSERTIONS) vv !defined(LZ_USE_DEBUG_ASSERTIONS)

#define LZ_ASSERT(CONDITION, MSG)

#endif // defined(LZ_USE_DEBUG_ASSERTIONS)

template<class... Ts>
LZ_CONSTEXPR_CXX_14 void decompose(const Ts&...) noexcept {
}

// next_fast: check whether it's faster to go forward or backward when incrementing n steps
// Only relevant for sized (requesting size is then O(1)) bidirectional iterators because
// for random access iterators this is O(1) anyway. For forward iterators this is not O(1),
// but we can't go any other way than forward. Random access iterators will use the same
// implementation as the forward non-sized iterables so we can skip that if statement.

#ifdef LZ_HAS_CXX_17

template<class I>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iter_t<I> next_fast(I&& iterable, diff_iterable_t<I> n) {
    if constexpr (!sized<I>::value || !std::is_same_v<iter_cat_iterable_t<I>, std::bidirectional_iterator_tag>) {
        using diff_type = diff_iterable_t<I>;
        return std::next(detail::begin(std::forward<I>(iterable)), static_cast<diff_type>(n));
    }
    else {
        using diff_type = diff_iterable_t<I>;
        const auto size = static_cast<diff_type>(lz::size(iterable));
        if (n > size / 2) {
            return std::prev(detail::end(std::forward<I>(iterable)), size - n);
        }
        return std::next(detail::begin(std::forward<I>(iterable)), n);
    }
}

// clang-format on

template<class Iterator, class S>
constexpr diff_type<Iterator> distance_impl(Iterator begin, S end) {
    if constexpr (!is_ra<Iterator>::value) {
        diff_type<Iterator> dist = 0;
        for (; begin != end; ++begin, ++dist) {
        }
        return dist;
    }
    else {
        return end - begin;
    }
}

#else

// clang-format off

template<class I>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14
enable_if<sized<I>::value && std::is_same<iter_cat_iterable_t<I>, std::bidirectional_iterator_tag>::value, iter_t<I>>
next_fast(I&& iterable, const diff_iterable_t<I> n) {
    using diff_type = diff_iterable_t<I>;
    const auto size = static_cast<diff_type>(lz::size(iterable));
    if (n > size / 2) {
        return std::prev(detail::end(std::forward<I>(iterable)), size - n);
    }
    return std::next(detail::begin(std::forward<I>(iterable)), n);
}

template<class I>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14
enable_if<!sized<I>::value || !std::is_same<iter_cat_iterable_t<I>, std::bidirectional_iterator_tag>::value, iter_t<I>>
next_fast(I&& iterable, diff_iterable_t<I> n) {
    using diff_type = diff_iterable_t<I>;
    return std::next(detail::begin(std::forward<I>(iterable)), static_cast<diff_type>(n));
}

// clang-format on

template<class Iterator, class S>
LZ_CONSTEXPR_CXX_14 enable_if<!is_ra<Iterator>::value, diff_type<Iterator>> distance_impl(Iterator begin, S end) {
    diff_type<Iterator> dist = 0;
    for (; begin != end; ++begin, ++dist) {
    }
    return dist;
}

template<class Iterator, class S>
constexpr enable_if<is_ra<Iterator>::value, diff_type<Iterator>> distance_impl(Iterator begin, S end) {
    return end - begin;
}

#endif

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
    return detail::distance_impl(std::move(begin), std::move(end));
}

#ifdef LZ_HAS_CXX_17

/**
 * @brief Gets the unsigned size of a iterable or iterable if it has a .size() method. Example:
 * ```cpp
 * int arr[] = { 1, 2, 3, 4, 5 };
 * std::cout << lz::size(arr) << '\n'; // prints 5
 * ```
 *
 * @param i The iterable to get the size from.
 * @return The size of the iterable.
 */
template<class Iterable>
LZ_NODISCARD constexpr auto size(const Iterable& i) noexcept(noexcept(std::size(i))) -> decltype(std::size(i)) {
    static_assert(sized<Iterable>::value, "Iterable must be sized/contain a .size() method");
    return std::size(i);
}

#else

/**
 * @brief Gets the unsigned size of a iterable or iterable if it has a .size() method. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * std::cout << lz::size(vec) << '\n'; // prints 5
 * ```
 *
 * @param i The iterable to get the size from.
 * @return The size of the iterable.
 */
template<class Iterable>
LZ_NODISCARD constexpr auto size(const Iterable& i) noexcept(noexcept(i.size())) -> decltype(i.size()) {
    static_assert(sized<Iterable>::value, "Iterable must be sized/contain a .size() method");
    return i.size();
}

/**
 * @brief Gets the unsigned size of a container or iterable if it has a .size() method. Example:
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

#ifdef LZ_HAS_CXX_20

/**
 * @brief Gets the signed size of a container or iterable if it has a .size() method. Example:
 * ```cpp
 * int arr[] = { 1, 2, 3, 4, 5 };
 * std::cout << lz::ssize(arr) << '\n'; // prints 5
 * ```
 *
 * @param i The container to get the size from.
 * @return The size of the container.
 */
template<class Iterable>
LZ_NODISCARD constexpr auto ssize(const Iterable& i) noexcept(noexcept(std::ssize(i))) -> decltype(std::ssize(i)) {
    static_assert(sized<Iterable>::value, "Iterable must be sized/contain a .size() method");
    return std::ssize(i);
}

#else

/**
 * @brief Gets the signed size of a container or iterable if it has a .size() method. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * std::cout << lz::ssize(vec) << '\n'; // prints 5
 * ```
 *
 * @param i The container to get the size from.
 * @return The size of the container.
 */
template<class Iterable>
LZ_NODISCARD constexpr auto ssize(const Iterable& i) noexcept(noexcept(
    static_cast<detail::common_type<std::ptrdiff_t, typename std::make_signed<decltype(lz::size(i))>::type>>(lz::size(i))))
    -> detail::common_type<std::ptrdiff_t, typename std::make_signed<decltype(lz::size(i))>::type> {
    static_assert(sized<Iterable>::value, "Iterable must be sized/contain a .size() method");
    using T = detail::common_type<std::ptrdiff_t, typename std::make_signed<decltype(lz::size(i))>::type>;
    return static_cast<T>(lz::size(i));
}

/**
 * @brief Gets the signed size of a container or iterable if it has a .size() method. Example:
 * ```cpp
 * int arr[] = { 1, 2, 3, 4, 5 };
 * std::cout << lz::ssize(arr) << '\n'; // prints 5
 * ```
 *
 * @param c The container to get the size from.
 * @return The size of the container.
 */
template<class T, size_t N>
LZ_NODISCARD constexpr std::ptrdiff_t ssize(const T (&)[N]) noexcept {
    return static_cast<std::ptrdiff_t>(N);
}

#endif

#ifdef LZ_HAS_CXX_17

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
LZ_NODISCARD constexpr auto eager_size(Iterable&& c) {
    if constexpr (detail::sized<Iterable>::value) {
        return lz::size(c);
    }
    else {
        using std::distance;
        return static_cast<std::size_t>(distance(std::begin(c), std::end(c)));
    }
}

#else

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

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz
#endif // LZ_DETAIL_PROCS_HPP
