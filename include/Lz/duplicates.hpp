#ifndef LZ_DUPLICATES_HPP
#define LZ_DUPLICATES_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/duplicates.hpp>

namespace lz {

/**
 * @brief This iterable returns a pair of each element in the iterable and the number of times it appears in the iterable.
 * `pair::first` is the element and `pair::second` is the count. The count is `size_t` and starts from 1. The input iterable
 * needs to be sorted first in order to count the duplicates. The iterator category is min(bidirectional, <input iterable
 * category>). Its end() function will return a sentinel if the input iterable is forward or has a sentinel. This iterable does
 * not contain a .size() method. Example:
 * ```cpp
 * std::vector<int> input{ 1, 2, 2, 3, 4, 4, 5 };
 * auto dupes = input | lz::duplicates; // { { 1, 1 }, { 2, 2 }, { 3, 1 }, { 4, 2 }, { 5, 1 } }
 * auto dupes = lz::duplicates(input); // same as above
 * ```
 */
LZ_INLINE_VAR constexpr detail::duplicates_adaptor duplicates{};

/**
 * @brief This is a type alias for the `duplicates` iterable.
 * @tparam Iterable The iterable to check the duplicates of.
 * @tparam BinaryPredicate The binary predicate to compare the elements of the iterable. Defaults to `std::less`.
 * ```cpp
 * std::forward_list<int> list = {1, 2, 3, 4, 5};
 * lz::duplicates_iterable<std::forward_list<int>> res = lz::duplicates(list);
 * ```
 */
template<class Iterable, class BinaryPredicate = MAKE_BIN_PRED(less)>
using duplicates_iterable = detail::duplicates_iterable<Iterable, BinaryPredicate>;

} // namespace lz

#endif
