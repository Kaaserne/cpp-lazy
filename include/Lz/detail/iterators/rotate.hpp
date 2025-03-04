#pragma once

#ifndef LZ_ROTATE_ITERATOR_HPP
#define LZ_ROTATE_ITERATOR_HPP

#include <Lz/detail/traits.hpp>
#include <Lz/iterator_base.hpp>
#include <iterator>

namespace lz {
namespace detail {
template<class Iterator, class S, bool IsSized>
class rotate_iterator
    : public iterator<rotate_iterator<Iterator, S, IsSized>, ref_t<Iterator>, ptr_t<Iterator>, diff_type<Iterator>,
                      conditional<IsSized, iter_cat_t<Iterator>, std::forward_iterator_tag>,
                      conditional<IsSized, rotate_iterator<Iterator, S, IsSized>, Iterator>> {

    using traits = std::iterator_traits<Iterator>;

public:
    using reference = typename traits::reference;
    using value_type = typename traits::value_type;
    using pointer = typename traits::pointer;
    using difference_type = typename traits::difference_type;

private:
    Iterator _iterator;
    Iterator _begin;
    S _end;
    std::size_t _offset{};

public:
    constexpr rotate_iterator() = default;

    LZ_CONSTEXPR_CXX_14 rotate_iterator(Iterator begin, S end, Iterator start, const std::size_t offset) :
        _iterator{ std::move(start) },
        _begin{ std::move(begin) },
        _end{ std::move(end) },
        _offset{ offset } {
    }

    constexpr reference dereference() const {
        return *_iterator;
    }

    constexpr pointer arrow() const {
        return _iterator.operator->();
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        ++_iterator;
        ++_offset;
        if (_iterator == _end) {
            _iterator = _begin;
        }
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        if (_iterator == _begin) {
            _iterator = _end;
        }
        --_iterator;
        --_offset;
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(difference_type n) {
        _offset += n;
        if (n < 0) {
            n = -n;
            if (n > _iterator - _begin) {
                _iterator = _end - (n - (_iterator - _begin));
            }
            else {
                _iterator += -n;
            }
        }
        else {
            if (n > _end - _iterator) {
                _iterator = _begin + (n - (_end - _iterator));
            }
            else {
                _iterator += n;
            }
        }
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const rotate_iterator& other) const {
        return static_cast<difference_type>(_offset - other._offset);
    }

    constexpr bool eq(const rotate_iterator& b) const {
        return _offset == b._offset;
    }

    constexpr bool eq(const Iterator& b) const {
        if (_offset == 0 && _iterator != _end) {
            return false;
        }
        return _iterator == b;
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_ROTATE_ITERATOR_HPP
