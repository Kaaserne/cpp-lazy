#pragma once

#ifndef LZ_CACHED_REVERSE_ITERATOR_HPP
#define LZ_CACHED_REVERSE_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {

template<class Iterator>
class cached_reverse_iterator
    : public iterator<cached_reverse_iterator<Iterator>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>, diff_type<Iterator>,
                      iter_cat_t<Iterator>, cached_reverse_iterator<Iterator>> {
    using traits = std::iterator_traits<Iterator>;

    Iterator _iterator;
    Iterator _prev_it;
    Iterator _begin;

public:
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

#ifdef LZ_HAS_CONCEPTS

    constexpr cached_reverse_iterator()
        requires std::default_initializable<Iterator>
    = default;

#else

    template<class I = Iterator, class = enable_if<std::is_default_constructible<I>::value>>
    constexpr cached_reverse_iterator() {
    }

#endif

    LZ_CONSTEXPR_CXX_14 cached_reverse_iterator(Iterator it, Iterator begin, Iterator end) :
        _iterator{ std::move(it) },
        _prev_it{ _iterator },
        _begin{ std::move(begin) } {
        if (_iterator == end && _begin != end) {
            --_prev_it;
        }
    }

    LZ_CONSTEXPR_CXX_14 cached_reverse_iterator& operator=(default_sentinel) {
        _iterator = default_sentinel{};
        return *this;
    }

    constexpr reference dereference() const {
        return *_prev_it;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        _iterator = _prev_it;
        if (_prev_it != _begin) {
            --_prev_it;
        }
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        _prev_it = _iterator;
        ++_iterator;
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type n) {
        if (n == 0) {
            return;
        }
        _iterator -= n;
        if (_iterator != _begin) {
            _prev_it = (_iterator - 1);
        }
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const cached_reverse_iterator& other) const {
        LZ_ASSERT(other._begin == _begin, "Cannot compare iterators from different ranges");
        return static_cast<difference_type>(other._iterator - _iterator);
    }

    constexpr bool eq(const cached_reverse_iterator& other) const {
        return _iterator == other._iterator;
    }
};

} // namespace detail
} // namespace lz

#endif // LZ_CACHED_REVERSE_ITERATOR_HPP
