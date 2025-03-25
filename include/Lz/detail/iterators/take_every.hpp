#pragma once

#ifndef LZ_TAKE_EVERY_ITERATOR_HPP
#define LZ_TAKE_EVERY_ITERATOR_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/traits.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {
template<class Iterator, class S, class = void>
class take_every_iterator;

template<class Iterator, class S>
class take_every_iterator<Iterator, S, enable_if<!is_bidi<Iterator>::value>>
    : public iterator<take_every_iterator<Iterator, S>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>, diff_type<Iterator>,
                      iter_cat_t<Iterator>, default_sentinel> {
    using traits = std::iterator_traits<Iterator>;

public:
    using value_type = typename traits::value_type;
    using iterator_category = iter_cat_t<Iterator>;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

    Iterator _iterator;
    S _end;
    std::size_t _offset;

public:
    LZ_CONSTEXPR_CXX_14 take_every_iterator(Iterator it, S end, const std::size_t offset) :
        _iterator{ std::move(it) },
        _end{ std::move(end) },
        _offset{ offset } {
        LZ_ASSERT(_offset != 0, "Can't increment by 0");
    }

    constexpr take_every_iterator() = default;

    constexpr reference dereference() const {
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT(_iterator != _end, "Out of bounds");
        for (std::size_t count = 0; _iterator != _end && count < _offset; ++_iterator, ++count) {
        }
    }

    constexpr bool eq(const take_every_iterator& b) const {
        LZ_ASSERT(_end == b._end && _offset == b._offset, "Incompatible iterators");
        return _iterator == b._iterator;
    }

    constexpr bool eq(default_sentinel) const {
        return _iterator == _end;
    }
};

template<class Iterator, class S>
class take_every_iterator<Iterator, S, enable_if<is_bidi<Iterator>::value && !is_ra<Iterator>::value>>
    : public iterator<take_every_iterator<Iterator, S>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>, diff_type<Iterator>,
                      iter_cat_t<Iterator>> {
    using traits = std::iterator_traits<Iterator>;

public:
    using value_type = typename traits::value_type;
    using iterator_category = typename traits::iterator_category;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

    Iterator _iterator;
    S _end;
    std::size_t _offset{};
    std::size_t _distance{};

public:
    LZ_CONSTEXPR_CXX_14 take_every_iterator(Iterator it, S end, const std::size_t offset, const std::size_t distance) :
        _iterator{ std::move(it) },
        _end{ std::move(end) },
        _offset{ offset },
        _distance{ distance } {
        LZ_ASSERT(_offset != 0, "Can't increment by 0");
    }

    constexpr take_every_iterator() = default;

    constexpr reference dereference() const {
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    template<class I = Iterator>
    LZ_CONSTEXPR_CXX_14 enable_if<!is_ra<I>::value> increment() {
        LZ_ASSERT(_iterator != _end, "Out of bounds");
        for (std::size_t count = 0; _iterator != _end && count < _offset; ++_iterator, ++count, ++_distance) {
        }
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        LZ_ASSERT(_distance != 0, "Out of bounds");
        const auto start_pos = _distance % _offset;
        const auto adjusted_start_pos = start_pos == 0 ? _offset : start_pos;

        for (std::size_t count = 0; count < adjusted_start_pos; count++, --_iterator, --_distance) {
        }
    }

    constexpr bool eq(const take_every_iterator& b) const {
        LZ_ASSERT(_end == b._end && _offset == b._offset, "Incompatible iterators");
        return _iterator == b._iterator;
    }

    constexpr bool eq(default_sentinel) const {
        return _iterator == _end;
    }
};

template<class Iterator, class S>
class take_every_iterator<Iterator, S, enable_if<is_ra<Iterator>::value>>
    : public iterator<take_every_iterator<Iterator, S>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>, diff_type<Iterator>,
                      iter_cat_t<Iterator>> {
    using traits = std::iterator_traits<Iterator>;

public:
    using value_type = typename traits::value_type;
    using iterator_category = typename traits::iterator_category;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

    Iterator _begin;
    Iterator _iterator;
    S _end;
    std::size_t _offset{};

public:
    LZ_CONSTEXPR_CXX_14 take_every_iterator(Iterator it, Iterator begin, S end, const std::size_t offset) :
        _begin{ std::move(begin) },
        _iterator{ std::move(it) },
        _end{ std::move(end) },
        _offset{ offset } {
        LZ_ASSERT(_offset != 0, "Can't increment by 0");
    }

    constexpr take_every_iterator() = default;

    constexpr reference dereference() const {
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        const auto distance = static_cast<std::size_t>(_end - _iterator);
        if (_offset >= distance) {
            LZ_ASSERT(_iterator != _end, "Out of bounds");
            _iterator = _end;
        }
        else {
            _iterator += _offset;
        }
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        const auto remaining = static_cast<std::size_t>(_iterator - _begin);
        const auto lldiv = std::lldiv(remaining, _offset);
        if (lldiv.rem == 0) {
            _iterator -= _offset;
        }
        else {
            _iterator -= lldiv.rem;
        }
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type offset) {
        const auto s_offset = static_cast<difference_type>(_offset);
        const auto to_add = offset * s_offset;

        if (to_add >= 0) {
            const auto current_distance = _end - _iterator;
            if (to_add <= current_distance) {
                _iterator += to_add;
                return;
            }
            LZ_ASSERT(to_add - current_distance < s_offset, "Out of bounds");
            _iterator = _end;
            return;
        }

        const auto current_distance = _iterator - _begin;
        const auto remainder = current_distance % s_offset;
        if (remainder == 0) {
            _iterator += to_add;
            return;
        }
        _iterator -= remainder;
        _iterator += (offset + 1) * s_offset;
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const take_every_iterator& other) const {
        LZ_ASSERT(_end == other._end && _offset == other._offset && _begin == other._begin, "Incompatible iterators");
        const auto remaining = _iterator - other._iterator;
        const auto lldiv = std::lldiv(static_cast<std::ptrdiff_t>(remaining), static_cast<std::ptrdiff_t>(_offset));
        return lldiv.rem == 0 ? lldiv.quot : lldiv.quot + (remaining < 0 ? -1 : 1);
    }

    constexpr bool eq(const take_every_iterator& b) const {
        LZ_ASSERT(_end == b._end && _offset == b._offset && _begin == b._begin, "Incompatible iterators");
        return _iterator == b._iterator;
    }

    constexpr bool eq(default_sentinel) const {
        return _iterator == _end;
    }
};
} // namespace detail
} // namespace lz

#endif