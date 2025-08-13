#pragma once

#ifndef LZ_ROTATE_ITERATOR_HPP
#define LZ_ROTATE_ITERATOR_HPP

#include <Lz/detail/traits.hpp>
#include <Lz/iterator_base.hpp>
#include <iterator>
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
    iter _iterator;
    Iterable _iterable;
    std::size_t _offset{};

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr rotate_iterator()
        requires(std::default_initializable<iter> && std::default_initializable<Iterable>)
    = default;

#else

    template<class I = iter,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<Iterable>::value>>
    constexpr rotate_iterator() noexcept(std::is_nothrow_default_constructible<iter>::value &&
                                         std::is_nothrow_default_constructible<Iterable>::value) {
    }

#endif

    template<class I>
    constexpr rotate_iterator(I&& iterable, iter start, const std::size_t offset) :
        _iterator{ std::move(start) },
        _iterable{ std::forward<I>(iterable) },
        _offset{ offset } {
    }

    LZ_CONSTEXPR_CXX_14 rotate_iterator& operator=(const iter& end) {
        _iterator = end;
        _offset = std::numeric_limits<std::size_t>::max();
        return *this;
    }

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
        _offset += static_cast<std::size_t>(n);
        if (n < 0) {
            n = -n;
            if (n > _iterator - _iterable.begin()) {
                _iterator = _iterable.end() - (n - (_iterator - _iterable.begin()));
            }
            else {
                _iterator += -n;
            }
        }
        else if (n >= _iterable.end() - _iterator) {
            _iterator = _iterable.begin() + (n - (_iterable.end() - _iterator));
        }
        else {
            _iterator += n;
        }
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const rotate_iterator& other) const {
        LZ_ASSERT_COMPATIBLE(_iterable.begin() == other._iterable.begin() && _iterable.end() == other._iterable.end());
        return static_cast<difference_type>(_offset) - static_cast<difference_type>(other._offset);
    }

    constexpr difference_type difference(default_sentinel_t) const {
        return -static_cast<difference_type>(_offset);
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const rotate_iterator& other) const {
        LZ_ASSERT_COMPATIBLE(_iterable.begin() == other._iterable.begin() && _iterable.end() == other._iterable.end());
        return _offset == other._offset || (_iterator == _iterable.end() && other._iterator == other._iterable.end());
    }

    constexpr bool eq(const iter& other) const {
        return (_offset != 0 || _iterator == _iterable.end()) && _iterator == other;
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_ROTATE_ITERATOR_HPP
