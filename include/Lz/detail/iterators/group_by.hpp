#pragma once

#ifndef LZ_GROUP_BY_ITERATOR_HPP
#define LZ_GROUP_BY_ITERATOR_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/traits.hpp>
#include <Lz/iterator_base.hpp>
#include <algorithm>

namespace lz {
namespace detail {
template<class Iterator, class S, class BinaryPredicate, class = void>
class group_by_iterator;

template<class Iterator, class S, class BinaryPredicate>
class group_by_iterator<Iterator, S, BinaryPredicate, enable_if<!is_bidi<Iterator>::value>>
    : public iterator<group_by_iterator<Iterator, S, BinaryPredicate>, std::pair<ref_t<Iterator>, basic_iterable<Iterator>>,
                      fake_ptr_proxy<std::pair<ref_t<Iterator>, basic_iterable<Iterator>>>, std::ptrdiff_t, iter_cat_t<Iterator>,
                      default_sentinel> {

    Iterator _sub_range_end;
    Iterator _sub_range_begin;
    S _end;
    mutable BinaryPredicate _comparer;

    using ref_type = ref_t<Iterator>;

    LZ_CONSTEXPR_CXX_14 void find_next(ref_type next) {
        using detail::find_if;
        using std::find_if;

        using rt = ref_t<Iterator>;
        _sub_range_end = find_if(std::move(_sub_range_end), _end, [this, &next](rt v) { return !_comparer(v, next); });
    }

    LZ_CONSTEXPR_CXX_14 void advance() {
        if (_sub_range_end == _end) {
            return;
        }
        ref_type next = *_sub_range_end;
        ++_sub_range_end;
        find_next(next);
    }

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::pair<decay_t<ref_type>, basic_iterable<Iterator>>;
    using reference = std::pair<ref_type, basic_iterable<Iterator>>;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = std::ptrdiff_t;

    constexpr group_by_iterator() = default;

    LZ_CONSTEXPR_CXX_14 group_by_iterator(Iterator begin, S end, BinaryPredicate binary_predicate) :
        _sub_range_end{ begin },
        _sub_range_begin{ std::move(begin) },
        _end{ std::move(end) },
        _comparer{ std::move(binary_predicate) } {
        if (_sub_range_begin == _end) {
            return;
        }
        advance();
    }

    constexpr reference dereference() const {
        return { *_sub_range_begin, { _sub_range_begin, _sub_range_end } };
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        _sub_range_begin = _sub_range_end;
        advance();
    }

    constexpr bool eq(const group_by_iterator& rhs) const {
        LZ_ASSERT(_end == rhs._end, "Incompatible iterators");
        return _sub_range_begin == rhs._sub_range_begin;
    }

    constexpr bool eq(default_sentinel) const {
        return _sub_range_begin == _end;
    }
};

template<class Iterator, class S, class BinaryPredicate>
class group_by_iterator<Iterator, S, BinaryPredicate, enable_if<is_bidi<Iterator>::value>>
    : public iterator<group_by_iterator<Iterator, S, BinaryPredicate>, std::pair<ref_t<Iterator>, basic_iterable<Iterator>>,
                      fake_ptr_proxy<std::pair<ref_t<Iterator>, basic_iterable<Iterator>>>, std::ptrdiff_t,
                      std::bidirectional_iterator_tag, group_by_iterator<Iterator, S, BinaryPredicate>> {

    Iterator _begin;
    Iterator _sub_range_end;
    Iterator _sub_range_begin;
    S _end;
    mutable BinaryPredicate _comparer;

    using ref_type = ref_t<Iterator>;

    LZ_CONSTEXPR_CXX_14 void find_next(ref_type last_seen) {
        using detail::find_if_not;
        using std::find_if;
        _sub_range_end =
            find_if(std::move(_sub_range_end), _end, [this, &last_seen](ref_type v) { return !_comparer(v, last_seen); });
    }

    LZ_CONSTEXPR_CXX_14 void advance() {
        if (_sub_range_end == _end) {
            return;
        }
        ref_type last_seen = *_sub_range_end;
        ++_sub_range_end;
        find_next(last_seen);
    }

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::pair<decay_t<ref_type>, basic_iterable<Iterator>>;
    using reference = std::pair<ref_type, basic_iterable<Iterator>>;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = std::ptrdiff_t;

    constexpr group_by_iterator() = default;

    LZ_CONSTEXPR_CXX_14 group_by_iterator(Iterator iterator, Iterator begin, S end, BinaryPredicate binary_predicate) :
        _begin{ std::move(begin) },
        _sub_range_end{ iterator },
        _sub_range_begin{ std::move(iterator) },
        _end{ std::move(end) },
        _comparer{ std::move(binary_predicate) } {
        if (_sub_range_begin == _end) {
            return;
        }
        advance();
    }

    constexpr reference dereference() const {
        return { *_sub_range_begin, { _sub_range_begin, _sub_range_end } };
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        _sub_range_begin = _sub_range_end;
        advance();
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        _sub_range_end = _sub_range_begin;
        auto prev = --_sub_range_begin;

        ref_type last_seen = *_sub_range_begin;
        while (_sub_range_begin != _begin && _comparer(*_sub_range_begin, last_seen)) {
            prev = _sub_range_begin;
            --_sub_range_begin;
        }

        if (_sub_range_begin == _begin && _comparer(*_sub_range_begin, last_seen)) {
            return;
        }
        _sub_range_begin = std::move(prev);
    }

    constexpr bool eq(const group_by_iterator& rhs) const {
        LZ_ASSERT(_end == rhs._end && _begin == rhs._begin, "Incompatible iterators");
        return _sub_range_begin == rhs._sub_range_begin;
    }

    constexpr bool eq(default_sentinel) const {
        return _sub_range_begin == _end;
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_GROUP_BY_ITERATOR_HPP
