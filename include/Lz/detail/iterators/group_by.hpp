#pragma once

#ifndef LZ_GROUP_BY_ITERATOR_HPP
#define LZ_GROUP_BY_ITERATOR_HPP

#include <Lz/detail/basic_iterable.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/func_container.hpp>
#include <Lz/detail/traits.hpp>
#include <Lz/iterator_base.hpp>
#include <algorithm>

namespace lz {
namespace detail {

template<class Iterator, class S, class BinaryPredicate>
class group_by_iterator
    : public iter_base<group_by_iterator<Iterator, S, BinaryPredicate>, std::pair<ref_t<Iterator>, basic_iterable<Iterator>>,
                       fake_ptr_proxy<std::pair<ref_t<Iterator>, basic_iterable<Iterator>>>, std::ptrdiff_t,
                       std::forward_iterator_tag, default_sentinel> {

    Iterator _sub_range_end;
    Iterator _sub_range_begin;
    S _end;
    mutable func_container<BinaryPredicate> _comparer;

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
        _sub_range_end(begin),
        _sub_range_begin(std::move(begin)),
        _end(std::move(end)),
        _comparer(std::move(binary_predicate)) {
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

    constexpr bool eq(const group_by_iterator& rhs) const noexcept {
        return _sub_range_begin == rhs._sub_range_begin;
    }

    constexpr bool eq(default_sentinel) const noexcept {
        return _sub_range_begin == _end;
    }
};
} // namespace detail
} // namespace lz
#endif // LZ_GROUP_BY_ITERATOR_HPP
