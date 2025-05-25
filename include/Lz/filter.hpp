#pragma once

#ifndef LZ_FILTER_HPP
#define LZ_FILTER_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/filter.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief Drops elements based on a condition predicate. If it returns `false`, the element in question is dropped. If it returns
 * `true` then this item is will be yielded. If the input iterable is forward an end() sentinel is returned, otherwise the end()
 * iterator is the same as the begin() iterator. It is bidirectional if the input iterable is also at least bidirectional. It does
 * not contain a .size() method. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto filtered = lz::filter(vec, [](int i) { return i % 2 == 0; }); // { 2, 4 }
 * // or
 * auto filtered = vec | lz::filter([](int i) { return i % 2 == 0; }); // { 2, 4 }
 * ```
 */
static constexpr detail::filter_adaptor filter{};

#else

/**
 * @brief Drops elements based on a condition predicate. If it returns `false`, the element in question is dropped. If it returns
 * `true` then this item is will be yielded. If the input iterable is forward an end() sentinel is returned, otherwise the end()
 * iterator is the same as the begin() iterator. It is bidirectional if the input iterable is also at least bidirectional. It does
 * not contain a .size() method. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto filtered = lz::filter(vec, [](int i) { return i % 2 == 0; }); // { 2, 4 }
 * // or
 * auto filtered = vec | lz::filter([](int i) { return i % 2 == 0; }); // { 2, 4 }
 * ```
 */
LZ_INLINE_VAR constexpr detail::filter_adaptor filter{};

#endif

/**
 * @brief Filter iterable helper alias.
 * @tparam Iterable The type of the iterable to filter.
 * @tparam UnaryPredicate The type of the predicate function to use for filtering.
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * lz::filter_iterable<std::vector<int>, std::function<bool(int)>> filtered = lz::filter(vec, [](int i) { return i % 2 == 0; });
 * ```
 */
template<class Iterable, class UnaryPredicate>
using filter_iterable = detail::filter_iterable<Iterable, UnaryPredicate>;

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // end LZ_FILTER_HPP