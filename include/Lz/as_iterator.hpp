#pragma once

#ifndef LZ_AS_ITERATOR_HPP
#define LZ_AS_ITERATOR_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/as_iterator.hpp>

namespace lz {

/**
 * @brief Creates an `as_iterator_iterable` from an iterable. This means it will return its underling iterators instead of
 * using the `operator*`. This can be useful for when you need the iterator and the deref operator. The iterable has a size
 * method if the underlying iterable is sized. Returns the same sentinel as its underlying iterable. Example:
 * ```cpp
 * std::vector<int> vec = {1, 2, 3, 4, 5};
 * auto iter = lz::as_iterator(vec);
 * for (const auto vector_iterator : iter) {
 *     std::cout << *vector_iterator << " "; // prints: 1 2 3 4 5
 * }
 * ```
 */
LZ_INLINE_VAR constexpr detail::as_iterator_adaptor as_iterator{};

/**
 * @brief Helper alias for as_iterator_iterable.
 *
 * @tparam Iterable The iterable
 * @tparam IterCat (Optional) The iterator category to decay the iterable to. If not provided, it will be deduced from the
 * iterable.
 */
template<class Iterable, class IterCat = iter_cat_iterable_t<Iterable>>
using as_iterator_iterable = detail::as_iterator_iterable<Iterable, IterCat>;

} // namespace lz

#endif // LZ_AS_ITERATOR_HPP
