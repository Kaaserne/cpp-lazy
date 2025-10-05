#pragma once

#ifndef LZ_EXCLUSIVE_SCAN_HPP
#define LZ_EXCLUSIVE_SCAN_HPP

#include <Lz/detail/adaptors/exclusive_scan.hpp>
#include <Lz/procs/chain.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Performs an exclusive scan on a container. The first element will be the init value, the second element will be the
 * first element of the container + the init value, the third element will be the second element + last returned element
 * previously, etc. It contains a .size() method if the input iterable also has a .size() method. Its end() function returns a
 * sentinel rather than an iterator. Its iterator category is forward. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * // std::plus is used as the default binary operation
 *
 * // you can also add a custom operator:
 * auto scan = vec | lz::exclusive_scan(0, std::plus<int>{}); // scan = { 0, 1, 3, 6, 10, 15 }
 * // When working with pipe expressions, you always need to specify the init value. When working with 'regular' functions,
 * you
 * // can omit the init value, in which case it will be a default constructed object of the type of the container.
 * // Example
 * auto scan = lz::exclusive_scan(vec);
 * auto scan = lz::exclusive_scan(vec, 0);
 * auto scan = vec | lz::exclusive_scan; // uses 0 and std::plus
 * auto scan = vec | lz::exclusive_scan(0);
 * ```
 */
LZ_INLINE_VAR constexpr detail::exclusive_scan_adaptor exclusive_scan{};

/**
 * @brief A type alias for the exclusive scan iterable.
 * @tparam Iterable The iterable type.
 * @tparam T The type of the elements in the iterable. Defaults to the value type of the iterable.
 * @tparam BinaryOp The binary operation used for the scan. Defaults to `std::plus`.
 * Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * lz::exclusive_scan<std::vector<int>> scan = lz::exclusive_scan_iterable(vec, 0);
 * ```
 */
template<class Iterable, class T = detail::val_iterable_t<Iterable>, class BinaryOp = detail::plus>
using exclusive_scan_iterable = detail::exclusive_scan_iterable<Iterable, T, BinaryOp>;

} // namespace lz

#endif
