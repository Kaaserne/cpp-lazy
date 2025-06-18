#pragma once

#ifndef LZ_TAKE_EVERY_ITERATOR_HPP
#define LZ_TAKE_EVERY_ITERATOR_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/procs.hpp>
#include <Lz/detail/traits.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {
template<class Iterable, class = void>
class take_every_iterator;

template<class Iterable>
class take_every_iterator<Iterable, enable_if<!is_bidi<iter_t<Iterable>>::value>>
    : public iterator<take_every_iterator<Iterable>, ref_t<iter_t<Iterable>>, fake_ptr_proxy<ref_t<iter_t<Iterable>>>,
                      diff_type<iter_t<Iterable>>, iter_cat_t<iter_t<Iterable>>, default_sentinel> {
    using it = iter_t<Iterable>;
    using sent = sentinel_t<Iterable>;
    using traits = std::iterator_traits<it>;

public:
    using value_type = typename traits::value_type;
    using iterator_category = iter_cat_t<it>;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

    it _iterator;
    sent _end;
    std::size_t _offset;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr take_every_iterator()
        requires std::default_initializable<it> && std::default_initializable<sent>
    = default;

#else

    template<class I = it,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<sent>::value>>
    constexpr take_every_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                             std::is_nothrow_default_constructible<sent>::value) {
    }

#endif

    LZ_CONSTEXPR_CXX_14 take_every_iterator(it iter, sent end, const std::size_t offset) :
        _iterator{ std::move(iter) },
        _end{ std::move(end) },
        _offset{ offset } {
        LZ_ASSERT(_offset != 0, "Can't increment by 0");
    }

    LZ_CONSTEXPR_CXX_14 take_every_iterator& operator=(default_sentinel) {
        _iterator = _end;
        return *this;
    }

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

    LZ_CONSTEXPR_CXX_14 bool eq(const take_every_iterator& b) const {
        LZ_ASSERT(_end == b._end && _offset == b._offset, "Incompatible iterators");
        return _iterator == b._iterator;
    }

    constexpr bool eq(default_sentinel) const {
        return _iterator == _end;
    }
};

template<class Iterable>
class take_every_iterator<Iterable, enable_if<is_bidi<iter_t<Iterable>>::value && !is_ra<iter_t<Iterable>>::value>>
    : public iterator<take_every_iterator<Iterable>, ref_t<iter_t<Iterable>>, fake_ptr_proxy<ref_t<iter_t<Iterable>>>,
                      diff_type<iter_t<Iterable>>, std::bidirectional_iterator_tag, default_sentinel> {
    using it = iter_t<Iterable>;
    using sent = sentinel_t<Iterable>;
    using traits = std::iterator_traits<it>;

public:
    using value_type = typename traits::value_type;
    using iterator_category = typename traits::iterator_category;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

    it _iterator;
    sent _end;
    std::size_t _offset{};
    std::size_t _distance{};

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr take_every_iterator()
        requires std::default_initializable<it> && std::default_initializable<sent>
    = default;

#else

    template<class I = it,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<sent>::value>>
    constexpr take_every_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                             std::is_nothrow_default_constructible<sent>::value) {
    }

#endif

    LZ_CONSTEXPR_CXX_14 take_every_iterator(it iter, sent end, const std::size_t offset, const std::size_t distance) :
        _iterator{ std::move(iter) },
        _end{ std::move(end) },
        _offset{ offset },
        _distance{ distance } {
        LZ_ASSERT(_offset != 0, "Can't increment by 0");
    }

    LZ_CONSTEXPR_CXX_14 take_every_iterator& operator=(default_sentinel) {
        _iterator = _end;
        return *this;
    }

    constexpr reference dereference() const {
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
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

    LZ_CONSTEXPR_CXX_14 bool eq(const take_every_iterator& b) const {
        LZ_ASSERT(_end == b._end && _offset == b._offset, "Incompatible iterators");
        return _iterator == b._iterator;
    }

    constexpr bool eq(default_sentinel) const {
        return _iterator == _end;
    }
};

template<class Iterable>
class take_every_iterator<Iterable, enable_if<is_ra<iter_t<Iterable>>::value>>
    : public iterator<take_every_iterator<Iterable>, ref_t<iter_t<Iterable>>, fake_ptr_proxy<ref_t<iter_t<Iterable>>>,
                      diff_type<iter_t<Iterable>>, iter_cat_t<iter_t<Iterable>>, default_sentinel> {

    using it = iter_t<Iterable>;
    using sent = sentinel_t<Iterable>;
    using traits = std::iterator_traits<it>;

public:
    using value_type = typename traits::value_type;
    using iterator_category = typename traits::iterator_category;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

    it _iterator;
    it _begin;
    sent _end;
    std::size_t _offset{};

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr take_every_iterator()
        requires std::default_initializable<it> && std::default_initializable<sent>
    = default;

#else

    template<class I = it,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<sent>::value>>
    constexpr take_every_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                             std::is_nothrow_default_constructible<sent>::value) {
    }

#endif

    LZ_CONSTEXPR_CXX_14 take_every_iterator(it iter, it begin, sent end, const std::size_t offset) :
        _iterator{ std::move(iter) },
        _begin{ std::move(begin) },
        _end{ std::move(end) },
        _offset{ offset } {
        LZ_ASSERT(_offset != 0, "Can't increment by 0");
    }

    LZ_CONSTEXPR_CXX_14 take_every_iterator& operator=(default_sentinel) {
        _iterator = _end;
        return *this;
    }

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
            _iterator += static_cast<difference_type>(_offset);
        }
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        const auto remaining = static_cast<std::size_t>(_iterator - _begin);
        const auto rem = remaining % _offset;
        if (rem == 0) {
            _iterator -= static_cast<difference_type>(_offset);
        }
        else {
            _iterator -= static_cast<difference_type>(rem);
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
        const auto quot = static_cast<std::ptrdiff_t>(remaining) / static_cast<std::ptrdiff_t>(_offset);
        const auto rem = static_cast<std::ptrdiff_t>(remaining) % static_cast<std::ptrdiff_t>(_offset);
        return rem == 0 ? quot : quot + (remaining < 0 ? -1 : 1);
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const take_every_iterator& b) const {
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