#pragma once

#ifndef LZ_AS_ITERATOR_HPP
#define LZ_AS_ITERATOR_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/as_iterator.hpp>

namespace lz {
// TODO docs
LZ_INLINE_VAR constexpr detail::as_iterator_adaptor as_iterator{};

template<class Iterable, class IterCat = iter_cat_iterable_t<Iterable>>
using as_iterator_iterable = detail::as_iterator_iterable<Iterable, IterCat>;

} // namespace lz

#endif // LZ_AS_ITERATOR_HPP
