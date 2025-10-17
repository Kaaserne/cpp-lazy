#pragma once

#ifndef LZ_ROTATE_ITERATOR_HPP
#define LZ_ROTATE_ITERATOR_HPP

#include <Lz/detail/iterator.hpp>
#include <Lz/detail/procs/assert.hpp>
#include <Lz/detail/traits/enable_if.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>
#include <Lz/procs/eager_size.hpp>

#include <limits>

namespace lz {
namespace detail {
template<class Iterable>
class rotate_iterator : public iterator<rotate_iterator<Iterable>, ref_t<iter_t<Iterable>>, ptr_t<iter_t<Iterable>>,
                                        diff_type<iter_t<Iterable>>, iter_cat_t<iter_t<Iterable>>, iter_t<Iterable>> {

    using iter = iter_t<Iterable>;
    using traits = std::iterator_traits<iter>;

public:
    using reference = typename traits::reference;
    using value_type = typename traits::value_type;
    using pointer = typename traits::pointer;
    using difference_type = typename traits::difference_type;

private:
    iter _iterator{};
    Iterable _iterable{};
    size_t _offset{};

public:
    constexpr rotate_iterator(const rotate_iterator&) = default;
    LZ_CONSTEXPR_CXX_14 rotate_iterator& operator=(const rotate_iterator&) = default;

#ifdef LZ_HAS_CONCEPTS

    constexpr rotate_iterator()
        requires(std::default_initializable<iter> && std::default_initializable<Iterable>)
    = default;

#else

    template<class I = iter,
             class = enable_if_t<std::is_default_constructible<I>::value && std::is_default_constructible<Iterable>::value>>
    constexpr rotate_iterator() noexcept(std::is_nothrow_default_constructible<iter>::value &&
                                         std::is_nothrow_default_constructible<Iterable>::value) {
    }

#endif

    template<class I>
    constexpr rotate_iterator(I&& iterable, iter start, const size_t offset) :
        _iterator{ std::move(start) },
        _iterable{ std::forward<I>(iterable) },
        _offset{ offset } {
    }

#ifdef LZ_HAS_CXX_17

    constexpr rotate_iterator& operator=(const iter& end) {
        _iterator = end;
        if constexpr (is_bidi_v<iter>) {
            _offset = lz::eager_size(_iterable);
        }
        else {
            _offset = std::numeric_limits<size_t>::max();
        }
        return *this;
    }
#else

    template<class I = iter>
    LZ_CONSTEXPR_CXX_14 enable_if_t<is_bidi<I>::value, rotate_iterator&> operator=(const iter& end) {
        _iterator = end;
        _offset = lz::eager_size(_iterable);
        return *this;
    }

    template<class I = iter>
    LZ_CONSTEXPR_CXX_14 enable_if_t<!is_bidi<I>::value, rotate_iterator&> operator=(const iter& end) {
        _iterator = end;
        _offset = std::numeric_limits<size_t>::max();
        return *this;
    }

#endif

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        LZ_ASSERT_DEREFERENCABLE(_iterator != _iterable.end());
        return *_iterator;
    }

    constexpr pointer arrow() const {
        return _iterator.operator->();
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT_INCREMENTABLE(_iterator != _iterable.end());
        ++_iterator;
        ++_offset;
        if (_iterator == _iterable.end()) {
            _iterator = _iterable.begin();
        }
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        if (_iterator == _iterable.begin()) {
            _iterator = _iterable.end();
        }
        LZ_ASSERT_DECREMENTABLE(_offset != 0 && _iterator != _iterable.begin());
        --_iterator;
        --_offset;
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(difference_type n) {
        LZ_ASSERT_SUB_ADDABLE((n < 0 ? -n : n) <= (_iterable.end() - _iterable.begin()));
        if (n == 0) {
            return;
        }
        _offset += static_cast<size_t>(n);
        const auto size = _iterable.end() - _iterable.begin();
        auto pos = (_iterator - _iterable.begin() + n) % size;
        if (pos < 0) {
            pos += size;
        }
        _iterator = _iterable.begin() + pos;
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const rotate_iterator& other) const {
        LZ_ASSERT_COMPATIBLE(_iterable.begin() == other._iterable.begin() && _iterable.end() == other._iterable.end());
        return static_cast<difference_type>(_offset - other._offset);
    }

    constexpr difference_type difference(const iter&) const {
        return (_iterable.begin() - _iterable.end()) + static_cast<difference_type>(_offset);
    }

    template<class I = iter>
    LZ_CONSTEXPR_CXX_14 enable_if_t<!is_bidi<I>::value, bool> eq(const rotate_iterator& other) const {
        LZ_ASSERT_COMPATIBLE(_iterable.begin() == other._iterable.begin() && _iterable.end() == other._iterable.end());
        return _offset == other._offset || (_iterator == other._iterator && _offset != 0 && other._offset != 0);
    }

    template<class I = iter>
    LZ_CONSTEXPR_CXX_14 enable_if_t<is_bidi<I>::value, bool> eq(const rotate_iterator& other) const {
        LZ_ASSERT_COMPATIBLE(_iterable.begin() == other._iterable.begin() && _iterable.end() == other._iterable.end());
        return _offset == other._offset;
    }

    constexpr bool eq(const iter& other) const {
        return (_offset != 0 || _iterator == _iterable.end()) && _iterator == other;
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_ROTATE_ITERATOR_HPP
