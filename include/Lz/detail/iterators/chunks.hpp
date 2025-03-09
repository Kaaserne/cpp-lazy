#pragma once

#ifndef LZ_CHUNKS_ITERATOR_HPP
#define LZ_CHUNKS_ITERATOR_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/iterator_base.hpp>
#include <cmath>

namespace lz {
namespace detail {
template<class Iterator, class S, class = void>
class chunks_iterator;

template<class Iterator, class S>
class chunks_iterator<Iterator, S, enable_if<is_fwd<Iterator>::value && !is_bidi<Iterator>::value>>
    : public iterator<chunks_iterator<Iterator, S>, basic_iterable<Iterator>, fake_ptr_proxy<basic_iterable<Iterator>>,
                      diff_type<Iterator>, iter_cat_t<Iterator>, default_sentinel> {

    using iter_traits = std::iterator_traits<Iterator>;

public:
    using value_type = basic_iterable<Iterator>;
    using reference = value_type;
    using pointer = fake_ptr_proxy<value_type>;
    using difference_type = typename iter_traits::difference_type;

private:
    Iterator _sub_range_begin;
    Iterator _sub_range_end;
    S _end;
    std::size_t _chunk_size{};

    LZ_CONSTEXPR_CXX_14 void next_chunk() {
        for (std::size_t count = 0; count < _chunk_size && _sub_range_end != _end; count++, ++_sub_range_end) {
        }
    }

public:
    LZ_CONSTEXPR_CXX_14 chunks_iterator(Iterator iterator, S end, const std::size_t chunk_size) :
        _sub_range_begin{ iterator },
        _sub_range_end{ std::move(iterator) },
        _end{ std::move(end) },
        _chunk_size{ chunk_size } {
        next_chunk();
    }

    constexpr chunks_iterator() = default;

    constexpr reference dereference() const {
        return { _sub_range_begin, _sub_range_end };
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        _sub_range_begin = _sub_range_end;
        next_chunk();
    }

    constexpr bool eq(const chunks_iterator& rhs) const {
        LZ_ASSERT(_chunk_size == rhs._chunk_size, "Incompatible iterators");
        return _sub_range_begin == rhs._sub_range_begin;
    }

    constexpr bool eq(default_sentinel) const {
        return _sub_range_begin == _end;
    }
};

template<class Iterator, class S>
class chunks_iterator<Iterator, S, enable_if<is_bidi<Iterator>::value && !is_ra<Iterator>::value>>
    : public iterator<chunks_iterator<Iterator, S>, basic_iterable<Iterator>, fake_ptr_proxy<basic_iterable<Iterator>>,
                      diff_type<Iterator>, iter_cat_t<Iterator>> {

    using iter_traits = std::iterator_traits<Iterator>;

public:
    using value_type = basic_iterable<Iterator>;
    using reference = value_type;
    using pointer = fake_ptr_proxy<value_type>;
    using difference_type = typename iter_traits::difference_type;

private:
    Iterator _begin;
    Iterator _sub_range_begin;
    Iterator _sub_range_end;
    S _end;
    std::size_t _chunk_size{};

    LZ_CONSTEXPR_CXX_14 void next_chunk() {
        for (std::size_t count = 0; count < _chunk_size && _sub_range_end != _end; count++, ++_sub_range_end) {
        }
    }

public:
    LZ_CONSTEXPR_CXX_20 chunks_iterator(Iterator iterator, Iterator begin, Iterator end, const std::size_t chunk_size) :
        _begin{ std::move(begin) },
        _sub_range_begin{ iterator },
        _end{ std::move(end) },
        _chunk_size{ chunk_size } {
        next_chunk();
    }

    constexpr chunks_iterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return { _sub_range_begin, _sub_range_end };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        _sub_range_begin = _sub_range_end;
        next_chunk();
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        _sub_range_end = _sub_range_begin;
        for (std::size_t count = 0; count < _chunk_size && _sub_range_begin != _begin; count++, --_sub_range_begin) {
        }
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const chunks_iterator& rhs) const {
        LZ_ASSERT(_chunk_size == rhs._chunk_size, "Incompatible iterators");
        return _sub_range_begin == rhs._sub_range_begin;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(default_sentinel) const {
        return _sub_range_begin == _end;
    }
};

template<class Iterator, class S>
class chunks_iterator<Iterator, S, enable_if<is_ra<Iterator>::value>>
    : public iterator<chunks_iterator<Iterator, S>, basic_iterable<Iterator>, fake_ptr_proxy<basic_iterable<Iterator>>,
                      diff_type<Iterator>, iter_cat_t<Iterator>> {

    using iter_traits = std::iterator_traits<Iterator>;

public:
    using value_type = basic_iterable<Iterator>;
    using reference = value_type;
    using pointer = fake_ptr_proxy<value_type>;
    using difference_type = typename iter_traits::difference_type;

private:
    Iterator _begin;
    Iterator _sub_range_begin;
    S _end;
    std::size_t _chunk_size{};

public:
    LZ_CONSTEXPR_CXX_20 chunks_iterator(Iterator iterator, Iterator begin, Iterator end, const std::size_t chunk_size) :
        _begin{ std::move(begin) },
        _sub_range_begin{ iterator },
        _end{ std::move(end) },
        _chunk_size{ chunk_size } {
    }

    constexpr chunks_iterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        auto sub_range_end = _sub_range_begin + std::min(static_cast<difference_type>(_chunk_size), _end - _sub_range_begin);
        return { _sub_range_begin, sub_range_end };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        _sub_range_begin += std::min(static_cast<difference_type>(_chunk_size), _end - _sub_range_begin);
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        const auto remaining = _sub_range_begin - _begin;
        const auto lldiv = std::lldiv(remaining, static_cast<difference_type>(_chunk_size));
        if (lldiv.rem == 0) {
            _sub_range_begin -= _chunk_size;
        }
        else {
            _sub_range_begin -= lldiv.rem;
        }
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const chunks_iterator& rhs) const {
        LZ_ASSERT(_chunk_size == rhs._chunk_size, "Incompatible iterators");
        return _sub_range_begin == rhs._sub_range_begin;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(default_sentinel) const {
        return _sub_range_begin == _end;
    }

    LZ_CONSTEXPR_CXX_20 void plus_is(const difference_type offset) {
        auto to_add = offset * static_cast<difference_type>(_chunk_size);

        if (to_add > 0) {
            const auto current_distance = _end - _sub_range_begin;
            if (to_add <= current_distance) {
                _sub_range_begin += to_add;
                return;
            }
            LZ_ASSERT(to_add - current_distance < static_cast<difference_type>(_chunk_size), "Out of bounds");
            _sub_range_begin = _end;
            return;
        }

        const auto current_distance = _sub_range_begin - _begin;
        const auto remainder = current_distance % static_cast<difference_type>(_chunk_size);
        if (remainder == 0) {
            _sub_range_begin += to_add;
            return;
        }
        _sub_range_begin -= remainder;
        to_add = (offset + 1) * static_cast<difference_type>(_chunk_size);
        _sub_range_begin += to_add;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 difference_type difference(const chunks_iterator& rhs) const {
        LZ_ASSERT(_chunk_size == rhs._chunk_size && _begin == rhs._begin && _end == rhs._end, "Incompatible iterators");
        const auto remaining = _sub_range_begin - rhs._sub_range_begin;
        const auto lldiv = std::lldiv(remaining, static_cast<difference_type>(_chunk_size));
        return lldiv.rem == 0 ? lldiv.quot : lldiv.quot + 1;
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_CHUNKS_ITERATOR_HPP
