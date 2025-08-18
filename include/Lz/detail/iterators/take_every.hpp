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
                      diff_type<iter_t<Iterable>>, iter_cat_t<iter_t<Iterable>>, default_sentinel_t> {
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
    size_t _offset;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr take_every_iterator()
        requires(std::default_initializable<it> && std::default_initializable<sent>)
    = default;

#else

    template<class I = it,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<sent>::value>>
    constexpr take_every_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                             std::is_nothrow_default_constructible<sent>::value) {
    }

#endif

    constexpr take_every_iterator(it iter, sent end, const size_t offset) :
        _iterator{ std::move(iter) },
        _end{ std::move(end) },
        _offset{ offset } {
    }

    LZ_CONSTEXPR_CXX_14 take_every_iterator& operator=(default_sentinel_t) {
        _iterator = _end;
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        LZ_ASSERT_DEREFERENCABLE(!eq(lz::default_sentinel));
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT_INCREMENTABLE(!eq(lz::default_sentinel));
        for (size_t count = 0; _iterator != _end && count < _offset; ++_iterator, ++count) {
        }
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const take_every_iterator& other) const {
        LZ_ASSERT_COMPATIBLE(_end == other._end && _offset == other._offset);
        return _iterator == other._iterator;
    }

    constexpr bool eq(default_sentinel_t) const {
        return _iterator == _end;
    }
};

template<class Iterable>
class take_every_iterator<Iterable, enable_if<is_bidi<iter_t<Iterable>>::value && !is_ra<iter_t<Iterable>>::value>>
    : public iterator<take_every_iterator<Iterable>, ref_t<iter_t<Iterable>>, fake_ptr_proxy<ref_t<iter_t<Iterable>>>,
                      diff_type<iter_t<Iterable>>, std::bidirectional_iterator_tag, default_sentinel_t> {
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
    size_t _offset{};
    size_t _distance{};

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr take_every_iterator()
        requires(std::default_initializable<it> && std::default_initializable<sent>)
    = default;

#else

    template<class I = it,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<sent>::value>>
    constexpr take_every_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                             std::is_nothrow_default_constructible<sent>::value) {
    }

#endif

    constexpr take_every_iterator(it iter, sent end, const size_t offset, const size_t distance) :
        _iterator{ std::move(iter) },
        _end{ std::move(end) },
        _offset{ offset },
        _distance{ distance } {
    }

    LZ_CONSTEXPR_CXX_14 take_every_iterator& operator=(default_sentinel_t) {
        _iterator = _end;
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        LZ_ASSERT_DEREFERENCABLE(!eq(lz::default_sentinel));
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT_INCREMENTABLE(!eq(lz::default_sentinel));
        for (size_t count = 0; _iterator != _end && count < _offset; ++_iterator, ++count, ++_distance) {
        }
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        LZ_ASSERT_DECREMENTABLE(_distance != 0);
        const auto start_pos = _distance % _offset;
        const auto adjusted_start_pos = start_pos == 0 ? _offset : start_pos;

        for (size_t count = 0; count < adjusted_start_pos; count++, --_iterator, --_distance) {
        }
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const take_every_iterator& other) const {
        LZ_ASSERT_COMPATIBLE(_end == other._end && _offset == other._offset);
        return _iterator == other._iterator;
    }

    constexpr bool eq(default_sentinel_t) const {
        return _iterator == _end;
    }
};

template<class Iterable>
class take_every_iterator<Iterable, enable_if<is_ra<iter_t<Iterable>>::value>>
    : public iterator<take_every_iterator<Iterable>, ref_t<iter_t<Iterable>>, fake_ptr_proxy<ref_t<iter_t<Iterable>>>,
                      diff_type<iter_t<Iterable>>, iter_cat_t<iter_t<Iterable>>, default_sentinel_t> {

    using it = iter_t<Iterable>;
    using sent = sentinel_t<Iterable>;
    using traits = std::iterator_traits<it>;

public:
    using value_type = typename traits::value_type;
    using iterator_category = typename traits::iterator_category;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

private:
    Iterable _iterable;
    it _iterator;
    size_t _offset{};

    LZ_CONSTEXPR_CXX_14 difference_type difference_impl(const sent& iter) const {
        const auto remaining = _iterator - iter;
        const auto quot = static_cast<std::ptrdiff_t>(remaining) / static_cast<std::ptrdiff_t>(_offset);
        const auto rem = static_cast<std::ptrdiff_t>(remaining) % static_cast<std::ptrdiff_t>(_offset);
        return rem == 0 ? quot : quot + (remaining < 0 ? -1 : 1);
    }

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr take_every_iterator()
        requires(std::default_initializable<it> && std::default_initializable<sent>)
    = default;

#else

    template<class I = it,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<sent>::value>>
    constexpr take_every_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                             std::is_nothrow_default_constructible<sent>::value) {
    }

#endif

    template<class I>
    constexpr take_every_iterator(I&& iterable, it iter, const size_t offset) :
        _iterable{ std::forward<I>(iterable) },
        _iterator{ std::move(iter) },
        _offset{ offset } {
    }

    LZ_CONSTEXPR_CXX_14 take_every_iterator& operator=(default_sentinel_t) {
        _iterator = _iterable.end();
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        LZ_ASSERT_DEREFERENCABLE(!eq(lz::default_sentinel));
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT_INCREMENTABLE(!eq(lz::default_sentinel));
        const auto distance = static_cast<size_t>(_iterable.end() - _iterator);
        if (_offset >= distance) {
            _iterator = _iterable.end();
        }
        else {
            _iterator += static_cast<difference_type>(_offset);
        }
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        LZ_ASSERT_DECREMENTABLE(_iterator != _iterable.begin());
        const auto remaining = static_cast<size_t>(_iterator - _iterable.begin());
        const auto rem = remaining % _offset;
        _iterator -= rem == 0 ? static_cast<difference_type>(_offset) : static_cast<difference_type>(rem);
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type offset) {
        const auto s_offset = static_cast<difference_type>(_offset);
        const auto to_add = offset * s_offset;

        if (to_add >= 0) {
            const auto current_distance = _iterable.end() - _iterator;
            if (to_add <= current_distance) {
                _iterator += to_add;
                return;
            }
            LZ_ASSERT_ADDABLE(to_add - current_distance < s_offset);
            _iterator = _iterable.end();
            return;
        }

        const auto current_distance = _iterator - _iterable.begin();
        const auto remainder = current_distance % s_offset;
        if (remainder == 0) {
            LZ_ASSERT_SUBTRACTABLE(-to_add <= current_distance);
            _iterator += to_add;
            return;
        }
        LZ_ASSERT_SUBTRACTABLE(-((offset + 1) * s_offset - remainder) <= current_distance);
        _iterator += (offset + 1) * s_offset - remainder;
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const take_every_iterator& other) const {
        LZ_ASSERT_COMPATIBLE(_iterable.end() == other._iterable.end() && _offset == other._offset &&
                             _iterable.begin() == other._iterable.begin());
        return difference_impl(other._iterator);
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(default_sentinel_t) const {
        return difference_impl(_iterable.end());
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const take_every_iterator& other) const {
        LZ_ASSERT_COMPATIBLE(_iterable.end() == other._iterable.end() && _offset == other._offset &&
                             _iterable.begin() == other._iterable.begin());
        return _iterator == other._iterator;
    }

    constexpr bool eq(default_sentinel_t) const {
        return _iterator == _iterable.end();
    }
};
} // namespace detail
} // namespace lz

#endif
