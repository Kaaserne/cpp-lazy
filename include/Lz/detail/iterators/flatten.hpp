#pragma once

#ifndef LZ_FLATTEN_ITERATOR_HPP
#define LZ_FLATTEN_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {
template<bool>
struct count_dims_helper;

template<>
struct count_dims_helper<false> {
#ifdef LZ_HAS_CXX_11

    template<class>
    using type = std::integral_constant<std::size_t, 0>;

#else

    template<class>
    static constexpr std::size_t value = 0;

#endif
};

template<>
struct count_dims_helper<true> {
#ifdef LZ_HAS_CXX_11

    template<class T>
    using type =
        std::integral_constant<std::size_t, 1 + count_dims_helper<is_iterable<decltype(*std::begin(std::declval<T>()))>::value>::
                                                    template type<decltype(*std::begin(std::declval<T>()))>::value>;

#else

    template<class T>
    static constexpr std::size_t value = 1 + count_dims_helper<
        is_iterable<decltype(*std::begin(std::declval<T>()))>::value>::template value<decltype(*std::begin(std::declval<T>()))>;

#endif
};

#ifdef LZ_HAS_CXX_11

template<class T>
using count_dims = typename count_dims_helper<is_iterable<T>::value>::template type<T>;

#else

template<class T>
using count_dims = std::integral_constant<std::size_t, count_dims_helper<is_iterable<T>::value>::template value<T>>;

#endif

// Improvement of https://stackoverflow.com/a/21076724/8729023
template<class Iterator, class S>
class flatten_wrapper
    : public iterator<
          flatten_wrapper<Iterator, S>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>, diff_type<Iterator>,
          common_type<std::bidirectional_iterator_tag, iter_cat_t<Iterator>>,
          sentinel_selector<common_type<std::bidirectional_iterator_tag, iter_cat_t<Iterator>>, flatten_wrapper<Iterator, S>>> {

    Iterator _begin;
    Iterator _current;
    S _end;

    using traits = std::iterator_traits<Iterator>;

public:
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;

    constexpr flatten_wrapper() = default;

    constexpr flatten_wrapper(Iterator current, Iterator begin, S end) :
        _begin{ std::move(begin) },
        _current{ std::move(current) },
        _end{ std::move(end) } {
    }

    constexpr bool has_some() const {
        return _current != _end;
    }

    constexpr bool has_prev() const {
        return _current != _begin;
    }

    constexpr bool eq(const flatten_wrapper& b) const {
        LZ_ASSERT(_begin == b._begin && _end == b._end, "Incompatible iterators");
        return _current == b._current;
    }

    constexpr bool eq(default_sentinel) const {
        return _current == _end;
    }

    constexpr reference dereference() const {
        return *_current;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        ++_current;
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        --_current;
    }
};

template<class, class, std::size_t>
class flatten_iterator;

template<class Iterator, std::size_t N>
using inner =
    flatten_iterator<decltype(std::begin(*std::declval<Iterator>())), decltype(std::end(*std::declval<Iterator>())), N - 1>;

template<class Iterator, class S, std::size_t N>
class flatten_iterator
    : public iterator<flatten_iterator<Iterator, S, N>, ref_t<inner<Iterator, N>>, fake_ptr_proxy<ref_t<inner<Iterator, N>>>,
                      diff_type<inner<Iterator, N>>, common_type<std::bidirectional_iterator_tag, iter_cat_t<inner<Iterator, N>>>,
                      sentinel_selector<iter_cat_t<inner<Iterator, N>>, flatten_iterator<Iterator, S, N>>> {

    using this_inner = inner<Iterator, N>;

public:
    using reference = typename this_inner::reference;
    using pointer = typename this_inner::pointer;
    using value_type = typename this_inner::value_type;
    using difference_type = typename this_inner::difference_type;

private:
    LZ_CONSTEXPR_CXX_14 void advance() {
        if (_inner_iter.has_some()) {
            return;
        }
        for (++_outer_iter; _outer_iter.has_some(); ++_outer_iter) {
            auto begin = std::begin(*_outer_iter);
            _inner_iter = this_inner(begin, begin, std::end(*_outer_iter));
            if (_inner_iter.has_some()) {
                return;
            }
        }
        _inner_iter = {};
    }

    flatten_wrapper<Iterator, S> _outer_iter;
    this_inner _inner_iter;

public:
    constexpr flatten_iterator() = default;

    LZ_CONSTEXPR_CXX_14 flatten_iterator(Iterator it, Iterator begin, S end) :
        _outer_iter{ std::move(it), std::move(begin), std::move(end) } {
        if (_outer_iter.has_some()) {
            auto beg = std::begin(*_outer_iter);
            _inner_iter = this_inner(beg, beg, std::end(*_outer_iter));
            this->advance();
        }
    }

    constexpr bool has_some() const {
        return _outer_iter.has_some();
    }

    constexpr bool has_prev() const {
        return _inner_iter.has_prev() || _outer_iter.has_prev();
    }

    constexpr bool eq(const flatten_iterator& b) const noexcept {
        return _outer_iter == b._outer_iter && _inner_iter == b._inner_iter;
    }

    constexpr bool eq(default_sentinel) const noexcept {
        return !has_some();
    }

    constexpr reference dereference() const {
        return *_inner_iter;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        ++_inner_iter;
        this->advance();
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        if (_inner_iter.has_prev()) {
            --_inner_iter;
            return;
        }
        while (_outer_iter.has_prev()) {
            --_outer_iter;
            const auto end = std::end(*_outer_iter);
            _inner_iter = this_inner(end, std::begin(*_outer_iter), end);
            if (_inner_iter.has_prev()) {
                --_inner_iter;
                return;
            }
        }
    }
};

template<class Iterator, class S>
class flatten_iterator<Iterator, S, 0>
    : public iterator<flatten_iterator<Iterator, S, 0>, ref_t<flatten_wrapper<Iterator, S>>,
                      fake_ptr_proxy<ref_t<flatten_wrapper<Iterator, S>>>, diff_type<flatten_wrapper<Iterator, S>>,
                      common_type<std::bidirectional_iterator_tag, iter_cat_t<flatten_wrapper<Iterator, S>>>,
                      sentinel_selector<common_type<iter_cat_t<flatten_wrapper<Iterator, S>>, std::bidirectional_iterator_tag>,
                                        flatten_iterator<Iterator, S, 0>>> {

    flatten_wrapper<Iterator, S> _range;
    using traits = std::iterator_traits<Iterator>;

public:
    using pointer = typename traits::pointer;
    using reference = typename traits::reference;
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;

    constexpr flatten_iterator() = default;

    constexpr flatten_iterator(Iterator it, Iterator begin, S end) : _range{ std::move(it), std::move(begin), std::move(end) } {
    }

    constexpr bool has_prev() const noexcept {
        return _range.has_prev();
    }

    constexpr bool has_some() const noexcept {
        return _range.has_some();
    }

    constexpr reference dereference() const {
        return *_range;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    constexpr bool eq(const flatten_iterator& b) const noexcept {
        return _range == b._range;
    }

    constexpr bool eq(default_sentinel) const noexcept {
        return _range.eq(default_sentinel{});
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        ++_range;
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        --_range;
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_FLATTEN_ITERATOR_HPP
