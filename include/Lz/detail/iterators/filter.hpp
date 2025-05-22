#pragma once

#ifndef LZ_FILTER_ITERATOR_HPP
#define LZ_FILTER_ITERATOR_HPP

#include <Lz/detail/algorithm.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/iterator_base.hpp>
#include <algorithm>

namespace lz {
namespace detail {
template<class Iterator, class S, class UnaryPredicate, class = void>
class filter_iterator;

template<class Iterator, class S, class UnaryPredicate>
class filter_iterator<Iterator, S, UnaryPredicate, enable_if<!is_bidi<Iterator>::value>>
    : public iterator<filter_iterator<Iterator, S, UnaryPredicate>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>,
                      diff_type<Iterator>, iter_cat_t<Iterator>, default_sentinel> {

    using traits = std::iterator_traits<Iterator>;

public:
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

    LZ_CONSTEXPR_CXX_14 Iterator find(Iterator first, S last) {
        using detail::find_if;
        using std::find_if;
        return find_if(std::move(first), std::move(last), _predicate);
    }

private:
    Iterator _iterator;
    S _end;
    mutable UnaryPredicate _predicate;

public:
    LZ_CONSTEXPR_CXX_14 filter_iterator(Iterator it, Iterator begin, S end, UnaryPredicate up) :
        _iterator{ std::move(it) },
        _end{ std::move(end) },
        _predicate{ std::move(up) } {
        if (_iterator == begin) {
            _iterator = find(std::move(_iterator), _end);
        }
    }

    LZ_CONSTEXPR_CXX_14 filter_iterator& operator=(default_sentinel) {
        _iterator = _end;
        return *this;
    }

    constexpr reference dereference() const {
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        ++_iterator;
        _iterator = find(std::move(_iterator), _end);
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const filter_iterator& b) const {
        LZ_ASSERT(_end == b._end, "Incompatible iterators");
        return _iterator == b._iterator;
    }

    constexpr bool eq(default_sentinel) const {
        return _iterator == _end;
    }
};

template<class Iterator, class S, class UnaryPredicate>
class filter_iterator<Iterator, S, UnaryPredicate, enable_if<is_bidi<Iterator>::value>>
    : public iterator<filter_iterator<Iterator, S, UnaryPredicate>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>,
                      diff_type<Iterator>, std::bidirectional_iterator_tag, filter_iterator<Iterator, S, UnaryPredicate>> {

    using traits = std::iterator_traits<Iterator>;

public:
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

    LZ_CONSTEXPR_CXX_14 Iterator find(Iterator first, S last) {
        using detail::find_if;
        using std::find_if;
        return find_if(std::move(first), std::move(last), _predicate);
    }

private:
    Iterator _begin;
    Iterator _iterator;
    S _end;
    mutable UnaryPredicate _predicate;

public:
    LZ_CONSTEXPR_CXX_14 filter_iterator(Iterator it, Iterator begin, S end, UnaryPredicate up) :
        _begin{ std::move(begin) },
        _iterator{ std::move(it) },
        _end{ std::move(end) },
        _predicate{ std::move(up) } {
        if (_iterator == _begin) {
            _iterator = find(std::move(_iterator), _end);
        }
    }

    LZ_CONSTEXPR_CXX_14 filter_iterator& operator=(default_sentinel) {
        _iterator = _end;
        return *this;
    }

    constexpr reference dereference() const {
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        ++_iterator;
        _iterator = find(std::move(_iterator), _end);
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        do {
            --_iterator;
        } while (!_predicate(*_iterator) && _iterator != _begin);
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const filter_iterator& b) const {
        LZ_ASSERT(_end == b._end && _begin == b._begin, "Incompatible iterators");
        return _iterator == b._iterator;
    }

    constexpr bool eq(default_sentinel) const {
        return _iterator == _end;
    }
};
} // namespace detail
} // namespace lz

#endif