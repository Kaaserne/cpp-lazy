#pragma once

#ifndef LZ_DETAIL_TRAITS_ITERATOR_CATEGORIES_HPP
#define LZ_DETAIL_TRAITS_ITERATOR_CATEGORIES_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/procs/begin_end.hpp>
#include <Lz/traits/iter_type.hpp>
#include <iterator>

namespace lz {
namespace detail {

template<class IterTag>
using is_ra_tag = std::is_convertible<IterTag, std::random_access_iterator_tag>;

template<class IterTag>
using is_bidi_tag = std::is_convertible<IterTag, std::bidirectional_iterator_tag>;

template<class IterTag>
using is_fwd_tag = std::is_convertible<IterTag, std::forward_iterator_tag>;

template<class I>
using is_ra = std::is_convertible<typename std::iterator_traits<I>::iterator_category, std::random_access_iterator_tag>;

template<class I>
using is_bidi = std::is_convertible<typename std::iterator_traits<I>::iterator_category, std::bidirectional_iterator_tag>;

template<class I>
using is_fwd = std::is_convertible<typename std::iterator_traits<I>::iterator_category, std::forward_iterator_tag>;

template<class Cat, class Cat2>
using strongest_cat_t = typename std::common_type<Cat, Cat2>::type;

template<class Cat>
using bidi_strongest_cat = strongest_cat_t<Cat, std::bidirectional_iterator_tag>;

#ifdef LZ_HAS_CXX_17

template<class I>
constexpr bool is_ra_v = is_ra<I>::value;

template<class I>
constexpr bool is_bidi_v = is_bidi<I>::value;

template<class I>
constexpr bool is_fwd_v = is_fwd<I>::value;

template<class Tag>
constexpr bool is_ra_tag_v = is_ra_tag<Tag>::value;

template<class Tag>
constexpr bool is_bidi_tag_v = is_bidi_tag<Tag>::value;

template<class Tag>
constexpr bool is_fwd_tag_v = is_fwd_tag<Tag>::value;

#endif

template<class Iterator>
using iter_cat_t = typename std::iterator_traits<Iterator>::iterator_category;

template<class Iterable>
using iter_cat_iterable_t = typename std::iterator_traits<iter_t<Iterable>>::iterator_category;

template<class Iterable>
using is_sentinel_assignable = std::is_assignable<iter_t<Iterable>, sentinel_t<Iterable>>;

} // namespace detail
} // namespace lz

#endif
