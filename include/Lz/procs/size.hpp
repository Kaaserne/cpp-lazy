
#pragma once

#ifndef LZ_SIZE_HPP
#define LZ_SIZE_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <type_traits>

LZ_MODULE_EXPORT namespace lz {

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
LZ_NODISCARD constexpr size_t size(const T (&c)[N]) noexcept {
    return static_cast<void>(c), N;
}


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
    static_cast<typename std::common_type<std::ptrdiff_t, typename std::make_signed<decltype(lz::size(i))>::type>::type>(lz::size(i))))
    -> typename std::common_type<std::ptrdiff_t, typename std::make_signed<decltype(lz::size(i))>::type>::type {
    using T = typename std::common_type<std::ptrdiff_t, typename std::make_signed<decltype(lz::size(i))>::type>::type;
    return static_cast<T>(lz::size(i));
}

/**
 * @brief Gets the signed size of a container or iterable if it has a .size() method. Example:
 * ```cpp
 * int arr[] = { 1, 2, 3, 4, 5 };
 * std::cout << lz::ssize(arr) << '\n'; // prints 5
 * ```
 *
 * @return The size of the container.
 */
template<class T, size_t N>
LZ_NODISCARD constexpr std::ptrdiff_t ssize(const T (&)[N]) noexcept {
    return static_cast<std::ptrdiff_t>(N);
}

} // namespace lz

#endif
