#pragma once

#ifndef LZ_INCLUSIVE_SCAN_HPP
#define LZ_INCLUSIVE_SCAN_HPP

#include <Lz/detail/adaptors/inclusive_scan.hpp>
#include <Lz/procs/chain.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Performs an inclusive scan on a container. The first element will be the result of the binary operation with init value
 * and the first element of the input iterable. The second element will be the previous result + the next value of the input
 * iterable, the third element will be previous result + the next value of the input iterable, etc. It contains a .size() method
 * if the input iterable also has a .size() method. Its end() function returns a sentinel rather than an iterator and its iterator
 * category is forward. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * // std::plus is used as the default binary operation
 * auto scan = lz::inclusive_scan(vec, 0); // scan = { 1, 3, 6, 10, 15 }
 * // so 0 + 1 (= 1), 1 + 2 (= 3), 3 + 3 (= 6), 6 + 4 (= 10), 10 + 5 (= 15)
 *
 * // or
 * auto scan = vec | lz::inclusive_scan(0); // scan = { 1, 3, 6, 10, 15 }
 * // you can also add a custom operator:
 * auto scan = vec | lz::inclusive_scan(0, std::plus<int>{}); // scan = { 1, 3, 6, 10, 15 }
 * // When working with pipe expressions, you always need to specify the init value. When working with 'regular' functions, you
 * // can omit the init value, in which case it will be a default constructed object of the type of the container.
 * // Example
 * auto scan = lz::inclusive_scan(vec);
 * auto scan = lz::inclusive_scan(vec, 0);
 * // auto scan = vec | lz::inclusive_scan; // uses 0 and std::plus
 * auto scan = vec | lz::inclusive_scan(0);
 * ```
 */
constexpr detail::inclusive_scan_adaptor inclusive_scan{};

/**
 * @brief Inclusive scan helper alias.
 * @tparam Iterable The input iterable
 * @tparam T The value type of the output (defaults to the value_type of `Iterable`)
 * @tparam BinaryOp The binary operation to use (defaults to `std::plus`)
 * ```
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * lz::inclusive_scan_iterable<std::vector<int>> scan = lz::inclusive_scan(vec, 0); // scan = { 1, 3, 6, 10, 15 }
 * ```
*/
template<class Iterable, class T = detail::val_iterable_t<Iterable>, class BinaryOp = detail::plus>
using inclusive_scan_iterable = detail::inclusive_scan_iterable<Iterable, T, BinaryOp>;

} // namespace lz

#endif
