#pragma once

#ifndef LZ_EXCLUSIVE_SCAN_ITERATOR_HPP
#define LZ_EXCLUSIVE_SCAN_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {
template<class Iterator, class S, class T, class BinaryOp>
class exclusive_scan_iterator : public iterator<exclusive_scan_iterator<Iterator, S, T, BinaryOp>, T&, fake_ptr_proxy<T&>,
                                                diff_type<Iterator>, std::forward_iterator_tag, default_sentinel> {
    Iterator _iterator;
    mutable T _reducer;
    S _end;
    bool _reached_end{};
    BinaryOp _binary_op;

    using traits = std::iterator_traits<Iterator>;

public:
    using reference = T&;
    using value_type = decay_t<reference>;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = typename traits::difference_type;

    constexpr exclusive_scan_iterator() = default;

    constexpr exclusive_scan_iterator(Iterator iterator, S end, T init, BinaryOp binary_op) :
        _iterator{ std::move(iterator) },
        _reducer{ std::move(init) },
        _end{ std::move(end) },
        _reached_end{ _iterator == _end },
        _binary_op{ std::move(binary_op) } {
    }

    constexpr reference dereference() const {
        return _reducer;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        if (_iterator == _end) {
            _reached_end = true;
            return;
        }
        _reducer = _binary_op(std::move(_reducer), *_iterator);
        ++_iterator;
    }

    constexpr bool eq(const exclusive_scan_iterator& b) const {
        return _iterator == b._iterator && _reached_end == b._reached_end;
    }

    constexpr bool eq(default_sentinel) const {
        return _iterator == _end && _reached_end;
    }
};
} // namespace detail
} // namespace lz

#endif