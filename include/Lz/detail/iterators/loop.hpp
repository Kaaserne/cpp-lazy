#pragma once

#ifndef LZ_LOOP_ITERATOR_HPP
#define LZ_LOOP_ITERATOR_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/traits.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {
template<class Iterator, class S, bool IsInfiniteLoop>
class loop_iterator;

template<class Iterator, class S>
class loop_iterator<Iterator, S, false>
    : public iterator<loop_iterator<Iterator, S, false>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>, diff_type<Iterator>,
                      iter_cat_t<Iterator>, sentinel_selector<iter_cat_t<Iterator>, loop_iterator<Iterator, S, false>>> {

    using traits = std::iterator_traits<Iterator>;

    Iterator _begin;
    Iterator _iterator;
    S _end;
    std::ptrdiff_t _amount{};

public:
    using reference = typename traits::reference;
    using value_type = typename traits::value_type;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = typename traits::difference_type;
    using iterator_category = typename traits::iterator_category;

    constexpr loop_iterator(Iterator iterator, Iterator begin, S end, std::size_t amount) :
        _begin{ std::move(begin) },
        _iterator{ std::move(iterator) },
        _end{ std::move(end) },
        _amount{ static_cast<std::ptrdiff_t>(amount) } {
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
            --_amount;
            if (_amount == 0) {
                return;
            }
            else {
                _iterator = _begin;
            }
        }
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        --_iterator;
        if (_iterator == _begin) {
            _iterator = _end;
            ++_amount;
        }
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(difference_type offset) {
        const auto iterable_length = _end - _begin;
        _amount -= offset / iterable_length;
        _iterator += offset % iterable_length;

        if (_amount == 0 && _iterator == _begin) {
            _iterator = _end;
        }
        else if (-offset == iterable_length * _amount) {
            _iterator = _begin;
        }
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const loop_iterator& other) const {
        const auto iterable_length = _end - _begin;
        if (_iterator > other._iterator) {
            const auto total_diff = iterable_length * (other._amount - _amount - 1);
            return total_diff + (_iterator - other._iterator);
        }
        const auto total_diff = iterable_length * (other._amount - _amount);
        return total_diff + ((_iterator - other._iterator) + iterable_length);
    }

    constexpr bool eq(const loop_iterator& other) const {
        return _amount == other._amount && _iterator == other._iterator;
    }

    constexpr bool eq(default_sentinel) const {
        return _amount == 0;
    }
};

template<class Iterator, class S>
class loop_iterator<Iterator, S, true>
    : public iterator<loop_iterator<Iterator, S, true>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>, diff_type<Iterator>,
                      iter_cat_t<Iterator>, default_sentinel> {

    using traits = std::iterator_traits<Iterator>;

    Iterator _begin;
    Iterator _iterator;
    S _end;

public:
    using reference = typename traits::reference;
    using value_type = typename traits::value_type;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = typename traits::difference_type;
    using iterator_category = typename traits::iterator_category;

    constexpr loop_iterator(Iterator begin, S end) : _iterator{ begin }, _begin{ std::move(begin) }, _end{ std::move(end) } {
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
        if (_begin == _end) {
            return true;
        }
        return false;
    }

    constexpr bool eq(default_sentinel) const {
        if (_begin == _end) {
            return true;
        }
        return false;
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_LOOP_ITERATOR_HPP