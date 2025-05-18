#pragma once

#ifndef LZ_LOOP_ITERATOR_HPP
#define LZ_LOOP_ITERATOR_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/traits.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {
template<class Iterator, class S, bool /* is inf */>
class loop_iterator;

template<class Iterator, class S>
class loop_iterator<Iterator, S, false>
    : public iterator<loop_iterator<Iterator, S, false>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>, diff_type<Iterator>,
                      iter_cat_t<Iterator>, sentinel_selector<iter_cat_t<Iterator>, loop_iterator<Iterator, S, false>>> {

    using traits = std::iterator_traits<Iterator>;

    Iterator _begin;
    Iterator _iterator;
    S _end;
    std::size_t _rotations_left{};

public:
    using reference = typename traits::reference;
    using value_type = typename traits::value_type;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = typename traits::difference_type;
    using iterator_category = typename traits::iterator_category;

    constexpr loop_iterator(Iterator it, Iterator begin, S end, std::size_t amount) :
        _begin{ std::move(begin) },
        _iterator{ std::move(it) },
        _end{ std::move(end) },
        _rotations_left{ amount } {
    }

    LZ_CONSTEXPR_CXX_14 loop_iterator& operator=(default_sentinel) {
        _rotations_left = 0;
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
        if (_iterator == _end) {
            --_rotations_left;
            _iterator = _begin;
        }
        if (static_cast<difference_type>(_rotations_left) == -1) {
            _iterator = _end;
            _rotations_left = 0;
        }
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        --_iterator;
        if (_iterator == _begin) {
            _iterator = _end;
            ++_rotations_left;
        }
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type offset) {
        const auto iter_length = _end - _begin;
        const auto remainder = offset % iter_length;
        _iterator += offset % iter_length;
        _rotations_left -= offset / iter_length;
        if (_iterator == _begin && static_cast<difference_type>(_rotations_left) == -1) {
            // We are exactly at end (rotations left is unsigned)
            _iterator = _end;
            _rotations_left = 0;
        }
        else if (_iterator == _end && offset < 0 && remainder == 0) {
            _iterator = _begin;
            --_rotations_left;
        }
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const loop_iterator& other) const {
        LZ_ASSERT(_begin == other._begin && _end == other._end, "Incompatible iterators");
        const auto rotations_left_diff = static_cast<difference_type>(other._rotations_left - _rotations_left);
        return (_iterator - other._iterator) + rotations_left_diff * (_end - _begin);
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const loop_iterator& other) const {
        LZ_ASSERT(_begin == other._begin && _end == other._end, "Incompatible iterators");
        return _rotations_left == other._rotations_left && _iterator == other._iterator;
    }

    constexpr bool eq(default_sentinel) const {
        return _rotations_left == 0 && _iterator == _end;
    }
};

template<class Iterator, class S>
class loop_iterator<Iterator, S, true>
    : public iterator<loop_iterator<Iterator, S, true>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>, diff_type<Iterator>,
                      iter_cat_t<Iterator>, default_sentinel> {

    using traits = std::iterator_traits<Iterator>;

    Iterator _iterator;
    Iterator _begin;
    S _end;

public:
    using reference = typename traits::reference;
    using value_type = typename traits::value_type;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = typename traits::difference_type;
    using iterator_category = typename traits::iterator_category;

    constexpr loop_iterator(Iterator begin, S end) : _iterator{ begin }, _begin{ std::move(begin) }, _end{ std::move(end) } {
    }

    LZ_CONSTEXPR_CXX_14 loop_iterator& operator=(default_sentinel) noexcept {
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
        if (_iterator == _end) {
            _iterator = _begin;
        }
    }

    constexpr bool eq(const loop_iterator&) const {
        return eq(default_sentinel{});
    }

    constexpr bool eq(default_sentinel) const {
        return _begin == _end;
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_LOOP_ITERATOR_HPP