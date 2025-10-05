#pragma once

#ifndef LZ_HEAGER_SIZE_HPP
#define LZ_HEAGER_SIZE_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/traits/is_sized.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>
#include <Lz/procs/distance.hpp>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

LZ_MODULE_EXPORT namespace lz {

#ifdef LZ_HAS_CXX_17

/**
 * @brief Gets the size of an iterable. If the iterable is not sized, it will calculate the size by iterating over the
 * iterable, which is O(n). Example:
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
    if constexpr (detail::is_sized_v<Iterable>) {
        return lz::size(c);
    }
    else {
        using std::distance;
        return static_cast<size_t>(distance(c.begin(), c.end()));
    }
}

#else

/**
 * @brief Gets the size of an iterable. If the iterable is not sized, it will calculate the size by iterating over the
 * iterable, which is O(n). Example:
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
LZ_NODISCARD constexpr auto
eager_size(Iterable&& c) -> detail::enable_if_t<detail::is_sized<Iterable>::value, decltype(lz::size(c))> {
    return lz::size(c);
}

/**
 * @brief Gets the size of an iterable. If the iterable is not sized, it will calculate the size by iterating over the
 * iterable, which is O(n), unless it is random access. Example:
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
LZ_NODISCARD constexpr detail::enable_if_t<!detail::is_sized<Iterable>::value, size_t> eager_size(Iterable&& c) {
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
LZ_NODISCARD constexpr detail::diff_iterable_t<Iterable> eager_ssize(Iterable&& c) {
    return static_cast<detail::diff_iterable_t<Iterable>>(eager_size(std::forward<Iterable>(c)));
}

} // namespace lz

#endif
