#pragma once

#ifndef LZ_DETAIL_PROCS_HPP
#define LZ_DETAIL_PROCS_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/traits.hpp>

// clang-format off

#include <iterator>

#if defined(LZ_DEBUG_ASSERTIONS)
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

LZ_MODULE_EXPORT namespace lz {

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
[[nodiscard]] constexpr auto size(const Iterable& i) noexcept(noexcept(std::size(i))) -> decltype(std::size(i)) {
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
LZ_NODISCARD constexpr size_t size(const T(&)[N]) noexcept {
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
[[nodiscard]] constexpr auto ssize(const Iterable& i) noexcept(noexcept(std::ssize(i))) -> decltype(std::ssize(i)) {
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
LZ_NODISCARD constexpr auto ssize(const Iterable& i) noexcept(
    noexcept(static_cast<detail::common_type<std::ptrdiff_t, typename std::make_signed<decltype(lz::size(i))>::type>>(
        lz::size(i)))) -> detail::common_type<std::ptrdiff_t, typename std::make_signed<decltype(lz::size(i))>::type> {
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

} // namespace lz

namespace lz {
namespace detail {

template<class T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_17 enable_if_t<std::is_object<T>::value, T*> addressof(T& arg) noexcept {
    return reinterpret_cast<T*>(&const_cast<char&>(reinterpret_cast<const volatile char&>(arg)));
}

template<class T>
LZ_NODISCARD constexpr enable_if_t<!std::is_object<T>::value, T*> addressof(T& arg) noexcept {
    return &arg;
}

#if defined(LZ_USE_DEBUG_ASSERTIONS)

[[noreturn]] inline void
assertion_fail(const char* file, const int line, const char* func, const char* message, const char* expr) {
#if defined(LZ_HAS_CXX_23) && defined(__cpp_lib_stacktrace) && LZ_HAS_INCLUDE(<stacktrace>)

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
#define LZ_ASSERT_INCREMENTABLE(CONDITION) LZ_ASSERT(CONDITION, "Cannot increment after end")
#define LZ_ASSERT_DECREMENTABLE(CONDITION) LZ_ASSERT(CONDITION, "Cannot decrement after end")
#define LZ_ASSERT_DEREFERENCABLE(CONDITION) LZ_ASSERT(CONDITION, "Cannot dereference end")
#define LZ_ASSERT_ADDABLE(CONDITION) LZ_ASSERT(CONDITION, "Cannot add after end")
#define LZ_ASSERT_SUBTRACTABLE(CONDITION) LZ_ASSERT(CONDITION, "Cannot subtract before end")
#define LZ_ASSERT_SUB_ADDABLE(CONDITION) LZ_ASSERT(CONDITION, "Cannot add after end/cannot subtract before begin")
#define LZ_ASSERT_COMPATIBLE(CONDITION) LZ_ASSERT(CONDITION, "Incompatible iterators")

#else // ^^ defined(LZ_USE_DEBUG_ASSERTIONS) vv !defined(LZ_USE_DEBUG_ASSERTIONS)

#define LZ_ASSERT(CONDITION, MSG)
#define LZ_ASSERT_INCREMENTABLE(CONDITION)
#define LZ_ASSERT_DECREMENTABLE(CONDITION)
#define LZ_ASSERT_DEREFERENCABLE(CONDITION)
#define LZ_ASSERT_ADDABLE(CONDITION)
#define LZ_ASSERT_SUBTRACTABLE(CONDITION)
#define LZ_ASSERT_SUB_ADDABLE(CONDITION)
#define LZ_ASSERT_COMPATIBLE(CONDITION)

#endif // defined(LZ_USE_DEBUG_ASSERTIONS)

#ifndef LZ_HAS_CXX_17

template<class... Ts>
LZ_CONSTEXPR_CXX_14 void decompose(const Ts&...) noexcept {
}

#endif

// next_fast(_safe): check whether it's faster to go forward or backward when incrementing n steps
// Only relevant for sized (requesting size is then O(1)) bidirectional iterators because
// for random access iterators this is O(1) anyway. For forward iterators this is not O(1),
// but we can't go any other way than forward. Random access iterators will use the same
// implementation as the forward non-sized iterables so we can skip that if statement.

#ifdef LZ_HAS_CXX_17

template<class I>
[[nodiscard]] constexpr iter_t<I> next_fast(I&& iterable, diff_iterable_t<I> n) {
    using iter = iter_t<I>;

    if constexpr (is_sized_v<I> && !is_sentinel_v<iter, sentinel_t<I>> && is_bidi_v<iter>) {
        const auto size = lz::ssize(iterable);
        if (n > size / 2) {
            return std::prev(detail::end(std::forward<I>(iterable)), size - n);
        }
    }
    return std::next(detail::begin(std::forward<I>(iterable)), n);
}

template<class I>
[[nodiscard]] constexpr iter_t<I> next_fast_safe(I&& iterable, const diff_iterable_t<I> n) {
    using diff_type = diff_iterable_t<I>;

    auto begin = iterable.begin();
    auto end = iterable.end();

    if constexpr (is_sized_v<I> && is_bidi_v<iter_t<I>> && !is_sentinel_v<iter_t<I>, sentinel_t<I>>) {
        const auto size = lz::ssize(iterable);
        if (n >= size) {
            return end;
        }
        return next_fast(std::forward<I>(iterable), n);
    }
    else {
        for (diff_type i = 0; i < n && begin != end; ++i, ++begin) {
        }
        return begin;
    }
}

template<class Iterator, class S>
[[nodiscard]] constexpr diff_type<Iterator> distance_impl(Iterator begin, S end) {
    if constexpr (!is_ra_v<Iterator>) {
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

template<class I>
LZ_NODISCARD LZ_CONSTEXPR_CXX_17
    enable_if_t<is_sized<I>::value && !is_sentinel<iter_t<I>, sentinel_t<I>>::value && is_bidi<iter_t<I>>::value, iter_t<I>>
    next_fast(I&& iterable, const diff_iterable_t<I> n) {

    const auto size = lz::ssize(iterable);
    if (n > size / 2) {
        return std::prev(detail::end(std::forward<I>(iterable)), size - n);
    }
    return std::next(detail::begin(std::forward<I>(iterable)), n);
}

template<class I>
LZ_NODISCARD LZ_CONSTEXPR_CXX_17
    enable_if_t<!is_sized<I>::value || is_sentinel<iter_t<I>, sentinel_t<I>>::value || !is_bidi<iter_t<I>>::value, iter_t<I>>
    next_fast(I&& iterable, diff_iterable_t<I> n) {
    return std::next(detail::begin(std::forward<I>(iterable)), n);
}

template<class I>
LZ_NODISCARD LZ_CONSTEXPR_CXX_17
    enable_if_t<is_sized<I>::value && !is_sentinel<iter_t<I>, sentinel_t<I>>::value && is_bidi<iter_t<I>>::value, iter_t<I>>
    next_fast_safe(I&& iterable, const diff_iterable_t<I> n) {

    const auto size = lz::ssize(iterable);
    if (n >= size) {
        return detail::end(std::forward<I>(iterable));
    }
    return next_fast(std::forward<I>(iterable), n);
}

template<class I>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14
    enable_if_t<!is_sized<I>::value || is_sentinel<iter_t<I>, sentinel_t<I>>::value || !is_bidi<iter_t<I>>::value, iter_t<I>>
    next_fast_safe(I&& iterable, const diff_iterable_t<I> n) {

    using diff_type = diff_iterable_t<I>;

    auto begin = detail::begin(std::forward<I>(iterable));
    const auto end = detail::end(std::forward<I>(iterable));

    for (diff_type i = 0; i < n && begin != end; ++i, ++begin) {
    }
    return begin;
}

template<class Iterator, class S>
LZ_CONSTEXPR_CXX_14 enable_if_t<!is_ra<Iterator>::value, diff_type<Iterator>> distance_impl(Iterator begin, S end) {
    diff_type<Iterator> dist = 0;
    for (; begin != end; ++begin, ++dist) {
    }
    return dist;
}

template<class Iterator, class S>
constexpr enable_if_t<is_ra<Iterator>::value, diff_type<Iterator>> distance_impl(Iterator begin, S end) {
    return end - begin;
}

#endif

} // namespace detail
} // namespace lz

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Gets the distance of an iterator pair. If iterator is not random access, the operation will be O(n), otherwise O(1).
 *
 * @param begin The beginning of the iterator
 * @param end The end of the iterator
 * @return The length of the iterator
 */
template<class Iterator, class S>
LZ_NODISCARD constexpr diff_type<Iterator> distance(Iterator begin, S end) {
    return detail::distance_impl(std::move(begin), std::move(end));
}

/**
 * @brief Gets the distance of an iterable. If iterable is not random access, the operation will be O(n), otherwise O(1).
 *
 * @param iterable The iterable to get the distance of
 * @return The length of the iterable
 */
template<class Iterable>
LZ_NODISCARD constexpr diff_iterable_t<detail::decay_t<Iterable>> distance(const Iterable& iterable) {
    using std::distance;
    return distance(iterable.begin(), iterable.end());
}

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
[[nodiscard]] constexpr auto eager_size(Iterable&& c) {
    if constexpr (is_sized_v<Iterable>) {
        return lz::size(c);
    }
    else {
        using std::distance;
        return static_cast<size_t>(distance(c.begin(), c.end()));
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
LZ_NODISCARD constexpr auto eager_size(Iterable &&
                                       c) -> detail::enable_if_t<detail::is_sized<Iterable>::value, decltype(lz::size(c))> {
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
LZ_NODISCARD constexpr detail::enable_if_t<!detail::is_sized<Iterable>::value, size_t> eager_size(Iterable && c) {
    using std::distance;
    return static_cast<size_t>(distance(c.begin(), c.end()));
}

#endif

/**
 * @brief Gets the signed size of an iterable. If the iterable is not sized, it will calculate the size by iterating over the
 * iterable, which is O(n), unless it is random access. Example:
 * ```cpp
 * // Sized, making it O(1)
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * std::cout << lz::eager_ssize(vec) << '\n'; // prints 5
 *
 * // Not sized, not random access, making it O(n)
 * auto not_sized = lz::c_string("Hello");
 * std::cout << lz::eager_ssize(not_sized) << '\n'; // prints 5
 * ```
 *
 * @param c The iterable to get the size of.
 * @return The size of the iterable.
 */
template<class Iterable>
LZ_NODISCARD constexpr diff_iterable_t<Iterable> eager_ssize(Iterable && c) {
    return static_cast<diff_iterable_t<Iterable>>(eager_size(std::forward<Iterable>(c)));
}

} // namespace lz

#endif // LZ_DETAIL_PROCS_HPP
