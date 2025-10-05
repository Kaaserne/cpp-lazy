#pragma once

#ifndef LZ_DETAIL_TRAITS_STRICT_ITERATOR_TRAITS_HPP
#define LZ_DETAIL_TRAITS_STRICT_ITERATOR_TRAITS_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/traits/iter_type.hpp>

namespace lz {
namespace detail {

template<class T>
struct strict_iterator_traits {
    using value_type = typename T::value_type;
    using reference = typename T::reference;
    using pointer = typename T::pointer;
    using difference_type = typename T::difference_type;
};

template<class T>
struct strict_iterator_traits<T*> {
    using value_type = typename std::remove_cv<T>::type;
    using reference = T&;
    using pointer = T*;
    using difference_type = std::ptrdiff_t;
};


template<class Iterator>
using val_t = typename strict_iterator_traits<Iterator>::value_type;

template<class Iterator>
using ref_t = typename strict_iterator_traits<Iterator>::reference;

template<class Iterator>
using ptr_t = typename strict_iterator_traits<Iterator>::pointer;

template<class Iterator>
using diff_type = typename strict_iterator_traits<Iterator>::difference_type;

template<class Iterable>
using val_iterable_t = typename strict_iterator_traits<iter_t<Iterable>>::value_type;

template<class Iterable>
using diff_iterable_t = typename strict_iterator_traits<iter_t<Iterable>>::difference_type;

template<class Iterable>
using ref_iterable_t = typename strict_iterator_traits<iter_t<Iterable>>::reference;

template<class Iterable>
using ptr_iterable_t = typename strict_iterator_traits<iter_t<Iterable>>::pointer;

} // namespace detail
} // namespace lz

#endif
