#pragma once

#ifndef LZ_CARTESIAN_PRODUCT_HPP
#define LZ_CARTESIAN_PRODUCT_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/cartesian_product.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

/**
 * @brief Performs a cartesian product on the given iterables. This means that it will return all possible combinations
 * of the elements of the given iterables. Contains a .size() function if all of the iterables have a .size() function.
 * Returns a sentinel if one of the iterables has a forward iterator or a sentinel. Returns the same iterator category of the
 * 'weakest' input iterables. Example:
 * ```cpp
 * std::vector<int> a = {1, 2};
 * std::vector<int> b = {3, 4};
 * auto product = lz::cartesian_product(a, b); // product = {{1, 3}, {1, 4}, {2, 3}, {2, 4}}
 * // or
 * auto product = a | lz::cartesian_product(b); // product = {{1, 3}, {1, 4}, {2, 3}, {2, 4}}
 * ```
 */
LZ_INLINE_VAR constexpr detail::cartesian_product_adaptor cartesian_product{};

/**
 * @brief Helper alias for the cartesian_product_iterable.
 * @tparam Iterables The types of the iterables.
 * Example:
 * ```cpp
 * std::vector<int> a = {1, 2};
 * std::vector<int> b = {3, 4};
 * lz::cartesian_product_iterable<std::vector<int>, std::vector<int>> product = lz::cartesian_product(a, b);
 * ```
 */
template<class... Iterables>
using cartesian_product_iterable = detail::cartesian_product_iterable<Iterables...>;

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_CARTESIAN_PRODUCT_HPP
