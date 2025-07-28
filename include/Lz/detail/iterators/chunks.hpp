#pragma once

#ifndef LZ_CHUNKS_ITERATOR_HPP
#define LZ_CHUNKS_ITERATOR_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {
template<class Iterator, class S, class = void>
class chunks_iterator;

template<class Iterator, class S>
class chunks_iterator<Iterator, S, enable_if<is_fwd<Iterator>::value && !is_bidi<Iterator>::value>>
    : public iterator<chunks_iterator<Iterator, S>, basic_iterable<Iterator>, fake_ptr_proxy<basic_iterable<Iterator>>,
                      diff_type<Iterator>, iter_cat_t<Iterator>, default_sentinel_t> {

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
#ifdef LZ_HAS_CONCEPTS

    constexpr chunks_iterator()
        requires std::default_initializable<Iterator> && std::default_initializable<S>
    = default;

#else

    template<class I = Iterator,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<S>::value>>
    constexpr chunks_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                         std::is_nothrow_default_constructible<S>::value) {
    }

#endif

    LZ_CONSTEXPR_CXX_14 chunks_iterator(Iterator it, S end, const std::size_t chunk_size) :
        _sub_range_begin{ it },
        _sub_range_end{ std::move(it) },
        _end{ std::move(end) },
        _chunk_size{ chunk_size } {
        next_chunk();
    }

    LZ_CONSTEXPR_CXX_14 chunks_iterator& operator=(default_sentinel_t) {
        _sub_range_begin = _end;
    }

    constexpr reference dereference() const {
        LZ_ASSERT(!eq(lz::default_sentinel), "Cannot dereference end iterator");
        return { _sub_range_begin, _sub_range_end };
    }

    constexpr pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT(!eq(lz::default_sentinel), "Cannot increment end iterator");
        _sub_range_begin = _sub_range_end;
        next_chunk();
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const chunks_iterator& rhs) const {
        LZ_ASSERT(_chunk_size == rhs._chunk_size, "Incompatible iterators");
        return _sub_range_begin == rhs._sub_range_begin;
    }

    constexpr bool eq(default_sentinel_t) const {
        return _sub_range_begin == _end;
    }
};

template<class Iterator, class S>
class chunks_iterator<Iterator, S, enable_if<is_bidi<Iterator>::value && !is_ra<Iterator>::value>>
    : public iterator<chunks_iterator<Iterator, S>, basic_iterable<Iterator>, fake_ptr_proxy<basic_iterable<Iterator>>,
                      diff_type<Iterator>, iter_cat_t<Iterator>, default_sentinel_t> {

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
    std::size_t _distance{};

    LZ_CONSTEXPR_CXX_14 void next_chunk() {
        for (std::size_t count = 0; count < _chunk_size && _sub_range_end != _end; count++, ++_sub_range_end, ++_distance) {
        }
    }

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr chunks_iterator()
        requires std::default_initializable<Iterator> && std::default_initializable<S>
    = default;

#else

    template<class I = Iterator,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<S>::value>>
    constexpr chunks_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                         std::is_nothrow_default_constructible<S>::value) {
    }

#endif

    LZ_CONSTEXPR_CXX_14
    chunks_iterator(Iterator begin, S end, const std::size_t chunk_size, const std::size_t cur_distance) :
        _sub_range_begin{ begin },
        _sub_range_end{ std::move(begin) },
        _end{ std::move(end) },
        _chunk_size{ chunk_size },
        _distance{ cur_distance } {
        next_chunk();
    }

    LZ_CONSTEXPR_CXX_14 chunks_iterator& operator=(default_sentinel_t) {
        _sub_range_begin = _end;
    }

    constexpr reference dereference() const {
        return { _sub_range_begin, _sub_range_end };
    }

    constexpr pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT(_sub_range_begin != _end, "Cannot increment end iterator");
        _sub_range_begin = _sub_range_end;
        next_chunk();
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        LZ_ASSERT(_distance != 0, "Cannot decrement begin iterator");
        _sub_range_end = _sub_range_begin;

        auto start_pos = _distance % _chunk_size;
        const auto adjusted_start_pos = start_pos == 0 ? _chunk_size : start_pos;

        for (std::size_t count = 0; count < adjusted_start_pos; count++, --_sub_range_begin, --_distance) {
        }
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const chunks_iterator& rhs) const {
        LZ_ASSERT(_chunk_size == rhs._chunk_size && _end == rhs._end, "Incompatible iterators");
        return _sub_range_begin == rhs._sub_range_begin;
    }

    constexpr bool eq(default_sentinel_t) const {
        return _sub_range_begin == _end;
    }
};

template<class Iterable, class Iterator>
class chunks_iterator<Iterable, Iterator, enable_if<is_ra<Iterator>::value>>
    : public iterator<chunks_iterator<Iterable, Iterator>, basic_iterable<Iterator>, fake_ptr_proxy<basic_iterable<Iterator>>,
                      diff_type<Iterator>, iter_cat_t<Iterator>, default_sentinel_t> {

    using iter_traits = std::iterator_traits<Iterator>;

public:
    using value_type = basic_iterable<Iterator>;
    using reference = value_type;
    using pointer = fake_ptr_proxy<value_type>;
    using difference_type = typename iter_traits::difference_type;

private:
    Iterator _sub_range_begin;
    Iterable _iterable;
    std::size_t _chunk_size{};

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr chunks_iterator()
        requires std::default_initializable<Iterator> && std::default_initializable<Iterable>
    = default;

#else

    template<class I = Iterator,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<Iterable>::value>>
    constexpr chunks_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                         std::is_nothrow_default_constructible<Iterable>::value) {
    }

#endif

    template<class I>
    LZ_CONSTEXPR_CXX_14 chunks_iterator(I&& iterable, Iterator it, const std::size_t chunk_size) :
        _sub_range_begin{ std::move(it) },
        _iterable{ std::forward<I>(iterable) },
        _chunk_size{ chunk_size } {
        LZ_ASSERT(_chunk_size != 0, "Cannot increment by 0");
    }

    LZ_CONSTEXPR_CXX_14 chunks_iterator& operator=(default_sentinel_t) {
        _sub_range_begin = std::end(_iterable);
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        using s = typename std::make_signed<std::size_t>::type;
        LZ_ASSERT(_sub_range_begin != std::end(_iterable), "Cannot dereference end iterator");
        auto sub_range_end =
            _sub_range_begin +
            static_cast<s>(std::min(_chunk_size, static_cast<std::size_t>(std::end(_iterable) - _sub_range_begin)));
        return { _sub_range_begin, sub_range_end };
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT(_sub_range_begin != std::end(_iterable), "Cannot increment end iterator");
        using s = typename std::make_signed<std::size_t>::type;
        _sub_range_begin +=
            static_cast<s>(std::min(_chunk_size, static_cast<std::size_t>(std::end(_iterable) - _sub_range_begin)));
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        LZ_ASSERT(_sub_range_begin != std::begin(_iterable), "Cannot decrement begin iterator");
        const auto remaining = static_cast<std::size_t>(_sub_range_begin - std::begin(_iterable));
        const auto offset = remaining % _chunk_size;

        if (offset == 0) {
            _sub_range_begin -= static_cast<difference_type>(_chunk_size);
        }
        else {
            _sub_range_begin -= static_cast<difference_type>(offset);
        }
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const chunks_iterator& rhs) const {
        LZ_ASSERT(_chunk_size == rhs._chunk_size, "Incompatible iterators");
        return _sub_range_begin == rhs._sub_range_begin;
    }

    constexpr bool eq(default_sentinel_t) const {
        return _sub_range_begin == std::end(_iterable);
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type offset) {
        const auto s_chunk_size = static_cast<difference_type>(_chunk_size);
        const auto to_add = offset * s_chunk_size;

        if (to_add >= 0) {
            const auto current_distance = std::end(_iterable) - _sub_range_begin;
            if (to_add <= current_distance) {
                _sub_range_begin += to_add;
                return;
            }
            LZ_ASSERT(to_add - current_distance < s_chunk_size, "Cannot add after end");
            _sub_range_begin = std::end(_iterable);
            return;
        }

        const auto current_distance = _sub_range_begin - std::begin(_iterable);
        const auto remainder = current_distance % s_chunk_size;
        if (remainder == 0) {
            _sub_range_begin += to_add;
            return;
        }
        const auto to_subtract = remainder + (-(offset + 1) * s_chunk_size);
        LZ_ASSERT(current_distance >= to_subtract, "Cannot subtract before begin");
        _sub_range_begin -= to_subtract;
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const chunks_iterator& rhs) const {
        LZ_ASSERT(_chunk_size == rhs._chunk_size && std::begin(_iterable) == std::begin(rhs._iterable) &&
                      std::end(_iterable) == std::end(rhs._iterable),
                  "Incompatible iterators");
        const auto left = _sub_range_begin - rhs._sub_range_begin;
        const auto remainder = left % static_cast<difference_type>(_chunk_size);
        const auto quotient = left / static_cast<difference_type>(_chunk_size);
        return remainder == 0 ? quotient : quotient + (left < 0 ? -1 : 1);
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(default_sentinel_t) const {
        const auto total_distance = std::end(_iterable) - _sub_range_begin;
        const auto remainder = total_distance % static_cast<difference_type>(_chunk_size);
        const auto quotient = total_distance / static_cast<difference_type>(_chunk_size);
        return -(remainder == 0 ? quotient : quotient + (total_distance < 0 ? -1 : 1));
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_CHUNKS_ITERATOR_HPP
