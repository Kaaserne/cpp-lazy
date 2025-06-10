#pragma once

#ifndef LZ_AS_ITERATOR_ITERATOR_HPP
#define LZ_AS_ITERATOR_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {

template<class Iterator, class S, class IterCat>
class as_iterator_iterator : public iterator<as_iterator_iterator<Iterator, S, IterCat>, Iterator, fake_ptr_proxy<Iterator>,
                                             diff_type<Iterator>, IterCat, S> {
    Iterator _iterator;

public:
    using value_type = Iterator;
    using reference = Iterator;
    using pointer = fake_ptr_proxy<Iterator>;
    using difference_type = typename std::iterator_traits<Iterator>::difference_type;

    template<class I = Iterator, class = enable_if<std::is_default_constructible<I>::value>>
    constexpr as_iterator_iterator() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

    constexpr as_iterator_iterator(Iterator it) : _iterator{ std::move(it) } {
    }

    LZ_CONSTEXPR_CXX_14 as_iterator_iterator& operator=(const S& other) {
        _iterator = other;
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        return _iterator;
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
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

    LZ_CONSTEXPR_CXX_14 bool eq(const S& other) const {
        return _iterator == other;
    }
};

} // namespace detail
} // namespace lz

#endif
