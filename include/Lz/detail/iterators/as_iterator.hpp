#pragma once

#ifndef LZ_AS_ITERATOR_ITERATOR_HPP
#define LZ_AS_ITERATOR_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/iterator.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>

namespace lz {
namespace detail {

template<class T>
class as_iterator_sentinel {
    T value{};

    template<class, class, class>
    friend class as_iterator_iterator;

    template<class, class>
    friend class as_iterator_iterable;

    explicit constexpr as_iterator_sentinel(T v) noexcept(std::is_nothrow_move_constructible<T>::value) : value{ std::move(v) } {
    }

public:
#ifdef LZ_HAS_CXX_20
    constexpr as_iterator_sentinel()
        requires(std::default_initializable<T>)
    = default;
#else
    template<class I = T, class = enable_if_t<std::is_default_constructible<I>::value>>
    constexpr as_iterator_sentinel() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }
#endif
};

template<class Iterator, class S, class IterCat>
class as_iterator_iterator : public iterator<as_iterator_iterator<Iterator, S, IterCat>, Iterator, fake_ptr_proxy<Iterator>,
                                             diff_type<Iterator>, IterCat, as_iterator_sentinel<S>> {
    Iterator _iterator{};

public:
    using value_type = Iterator;
    using reference = Iterator;
    using pointer = fake_ptr_proxy<Iterator>;
    using difference_type = typename std::iterator_traits<Iterator>::difference_type;

    constexpr as_iterator_iterator(const as_iterator_iterator&) = default;
    LZ_CONSTEXPR_CXX_14 as_iterator_iterator& operator=(const as_iterator_iterator&) = default;

#ifdef LZ_HAS_CONCEPTS

    constexpr as_iterator_iterator()
        requires(std::default_initializable<Iterator>)
    = default;

#else

    template<class I = Iterator, class = enable_if_t<std::is_default_constructible<I>::value>>
    constexpr as_iterator_iterator() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    explicit constexpr as_iterator_iterator(Iterator it) : _iterator{ std::move(it) } {
    }

    LZ_CONSTEXPR_CXX_14 as_iterator_iterator& operator=(const as_iterator_sentinel<S>& other) {
        _iterator = other.value;
        return *this;
    }

    constexpr reference dereference() const {
        return _iterator;
    }

    constexpr pointer arrow() const {
        return fake_ptr_proxy<Iterator>{ _iterator };
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        ++_iterator;
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        --_iterator;
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const as_iterator_iterator& other) const {
        return _iterator - other._iterator;
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type& n) {
        _iterator += n;
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const as_iterator_iterator& other) const {
        return _iterator == other._iterator;
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const as_iterator_sentinel<S>& other) const {
        return _iterator == other.value;
    }
};

} // namespace detail
} // namespace lz

#endif
