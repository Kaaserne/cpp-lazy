#pragma once

#ifndef LZ_INCLUSIVE_SCAN_HPP
#define LZ_INCLUSIVE_SCAN_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/inclusive_scan.hpp>

namespace lz {
LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief Performs an inclusive scan on a container. The first element will be the result of the binary operation with init value
 * and the first element of the input iterable. The second element will be the previous result + the next value of the input
 * iterable, the third element will be previous result + the next value of the input iterable, etc. It contains a .size() method
 * if the input iterable also has a .size() method. Its end() function returns a sentinel rather than an iterator. Example:
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
 * auto scan = lz::inclusive_scan(vec); // OK
 * auto scan = lz::inclusive_scan(vec, 0); // OK
 * // auto scan = vec | lz::inclusive_scan; // Error, unable to determine the type of the init value
 * auto scan = vec | lz::inclusive_scan(0); // OK
 * ```
 */
static const detail::inclusive_scan_adaptor inclusive_scan{};

#else

/**
 * @brief Performs an inclusive scan on a container. The first element will be the result of the binary operation with init value
 * and the first element of the input iterable. The second element will be the previous result + the next value of the input
 * iterable, the third element will be previous result + the next value of the input iterable, etc. It contains a .size() method
 * if the input iterable also has a .size() method. Its end() function returns a sentinel rather than an iterator. Example:
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
 * auto scan = lz::inclusive_scan(vec); // OK
 * auto scan = lz::inclusive_scan(vec, 0); // OK
 * // auto scan = vec | lz::inclusive_scan; // Error, unable to determine the type of the init value
 * auto scan = vec | lz::inclusive_scan(0); // OK
 * ```
 */
LZ_INLINE_VAR constexpr detail::inclusive_scan_adaptor inclusive_scan{};

#endif

LZ_MODULE_EXPORT_SCOPE_END
} // namespace lz

#endif