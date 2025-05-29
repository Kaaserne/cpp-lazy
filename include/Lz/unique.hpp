#pragma once

#ifndef LZ_UNIQUE_HPP
#define LZ_UNIQUE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/unique.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

/**
 * @brief Makes the input iterable unique. Every element therefore only occurs once. The input iterable must be sorted beforehand.
 * This iterator will 'decay' into a bidirectional one if the input iterator is higher than bidirectional. If the input iterable is
 * not bidirectional or higher, then the output iterator will be forward, and will also return a sentinel, rather than an
 * iterator. This method does not contain a .size() method. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 1, 2, 3, 3, 3, 4, 5, 5 };
 * std::sort(vec.begin(), vec.end());
 * auto unique = lz::unique(vec);  // second argument is custom comparer (optional)
 * // or
 * auto unique = vec | lz::unique;
 * // custom comparer can be passed as argument as well:
 * auto unique = vec | lz::unique(std::less<>{});
 * ```
 */
LZ_INLINE_VAR constexpr detail::unique_adaptor unique{};

/**
 * @brief Unique iterable helper alias
 * @tparam Iterable Type of the iterable to make unique
 * @tparam BinaryPredicate Type of the binary predicate to use for uniqueness
 * ```cpp
 * std::vector<int> vec = { 1, 1, 2, 3, 3, 3, 4, 5, 5 };
 * std::sort(vec.begin(), vec.end());
 * lz::unique_iterable<std::vector> vec_unique = lz::unique(vec);
 * ```
 */
template<class Iterable, class BinaryPredicate>
using unique_iterable = detail::unique_iterable<Iterable, BinaryPredicate>;

LZ_MODULE_EXPORT_SCOPE_END

} // end namespace lz

#endif // end LZ_UNIQUE_HPP