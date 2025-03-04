#pragma once

#ifndef LZ_INCLUSIVE_SCAN_ITERATOR_HPP
#define LZ_INCLUSIVE_SCAN_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {
template<class Iterator, class S, class T, class BinaryOp>
class inclusive_scan_iterator : public iterator<inclusive_scan_iterator<Iterator, S, T, BinaryOp>, T&, fake_ptr_proxy<T&>,
                                                diff_type<Iterator>, std::forward_iterator_tag, default_sentinel> {
    Iterator _iterator;
    mutable T _reducer;
    S _end;
    BinaryOp _binary_op;

    using traits = std::iterator_traits<Iterator>;

public:
    using reference = T&;
    using value_type = T;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = typename traits::difference_type;
    using iterator_category = std::forward_iterator_tag;

    constexpr inclusive_scan_iterator() = default;

    constexpr inclusive_scan_iterator(Iterator iterator, S end, T init, BinaryOp bin_op) :
        _iterator{ std::move(iterator) },
        _reducer{ std::move(init) },
        _end{ std::move(end) },
        _binary_op{ std::move(bin_op) } {
    }

    constexpr reference dereference() const {
        return _reducer;
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        ++_iterator;
        if (_iterator != _end) {
            _reducer = _binary_op(std::move(_reducer), *_iterator);
        }
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    constexpr bool eq(const inclusive_scan_iterator& b) const {
        return _iterator == b._iterator;
    }

    constexpr bool eq(default_sentinel) const {
        return _iterator == _end;
    }
};
} // namespace detail
} // namespace lz

#endif