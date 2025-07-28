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
        requires std::default_initializable<iter> && std::default_initializable<Iterable>
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
        LZ_ASSERT_DEREFERENCABLE(_iterator != std::end(_iterable));
        return *_iterator;
    }

    constexpr pointer arrow() const {
        return _iterator.operator->();
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT_INCREMENTABLE(_iterator != std::end(_iterable));
        ++_iterator;
        ++_offset;
        if (_iterator == std::end(_iterable)) {
            _iterator = std::begin(_iterable);
        }
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        if (_iterator == std::begin(_iterable)) {
            _iterator = std::end(_iterable);
        }
        LZ_ASSERT(_offset != 0 && _iterator != std::begin(_iterable), "Cannot decrement begin iterator");
        --_iterator;
        --_offset;
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(difference_type n) {
        LZ_ASSERT_SUB_ADDABLE((n < 0 ? -n : n) <= (std::end(_iterable) - std::begin(_iterable)));
        _offset += static_cast<std::size_t>(n);
        if (n < 0) {
            n = -n;
            if (n > _iterator - std::begin(_iterable)) {
                _iterator = std::end(_iterable) - (n - (_iterator - std::begin(_iterable)));
            }
            else {
                _iterator += -n;
            }
        }
        else if (n >= std::end(_iterable) - _iterator) {
            _iterator = std::begin(_iterable) + (n - (std::end(_iterable) - _iterator));
        }
        else {
            _iterator += n;
        }
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const rotate_iterator& other) const {
        LZ_ASSERT(std::begin(_iterable) == std::begin(other._iterable) && std::end(_iterable) == std::end(other._iterable),
                  "Incompatible iterators");
        return static_cast<difference_type>(_offset) - static_cast<difference_type>(other._offset);
    }

    constexpr difference_type difference(default_sentinel_t) const {
        return -static_cast<difference_type>(_offset);
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const rotate_iterator& b) const {
        LZ_ASSERT(std::begin(_iterable) == std::begin(b._iterable) && std::end(_iterable) == std::end(b._iterable),
                  "Incompatible iterators");
        return _offset == b._offset || (_iterator == std::end(_iterable) && b._iterator == std::end(b._iterable));
    }

    constexpr bool eq(const iter& b) const {
        return (_offset != 0 || _iterator == std::end(_iterable)) && _iterator == b;
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_ROTATE_ITERATOR_HPP
