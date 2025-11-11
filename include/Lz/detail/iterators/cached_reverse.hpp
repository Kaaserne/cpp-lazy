#pragma once

#ifndef LZ_CACHED_REVERSE_ITERATOR_HPP
#define LZ_CACHED_REVERSE_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/iterator.hpp>
#include <Lz/detail/procs/assert.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>
#include <Lz/util/default_sentinel.hpp>

namespace lz {
namespace detail {

template<class Iterator>
class cached_reverse_iterator
    : public iterator<cached_reverse_iterator<Iterator>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>, diff_type<Iterator>,
                      iter_cat_t<Iterator>, default_sentinel_t> {
    using traits = std::iterator_traits<Iterator>;

    Iterator _iterator{};
    Iterator _prev_it{};
    Iterator _begin{};

public:
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

    constexpr cached_reverse_iterator(const cached_reverse_iterator&) = default;
    LZ_CONSTEXPR_CXX_14 cached_reverse_iterator& operator=(const cached_reverse_iterator&) = default;

#ifdef LZ_HAS_CONCEPTS

    constexpr cached_reverse_iterator()
        requires(std::default_initializable<Iterator>)
    = default;

#else

    template<class I = Iterator, class = enable_if_t<std::is_default_constructible<I>::value>>
    constexpr cached_reverse_iterator() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    template<class S>
    LZ_CONSTEXPR_CXX_14 cached_reverse_iterator(Iterator it, Iterator begin, S end) :
        _iterator{ std::move(it) },
        _prev_it{ _iterator },
        _begin{ std::move(begin) } {
        if (_iterator == end && _begin != end) {
            --_prev_it;
        }
    }

    LZ_CONSTEXPR_CXX_14 cached_reverse_iterator& operator=(default_sentinel_t) {
        _iterator = _begin;
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        LZ_ASSERT_DEREFERENCABLE(!eq(lz::default_sentinel));
        return *_prev_it;
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT_DEREFERENCABLE(!eq(lz::default_sentinel));
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
        _iterator -= n;
        if (_iterator != _begin) {
            _prev_it = (_iterator - 1);
        }
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const cached_reverse_iterator& other) const {
        LZ_ASSERT_COMPATIBLE(other._begin == _begin);
        return static_cast<difference_type>(other._iterator - _iterator);
    }

    constexpr difference_type difference(default_sentinel_t) const {
        return static_cast<difference_type>(_begin - _iterator);
    }

    constexpr bool eq(const cached_reverse_iterator& other) const {
        return _iterator == other._iterator;
    }

    constexpr bool eq(default_sentinel_t) const {
        return _iterator == _begin;
    }
};

} // namespace detail
} // namespace lz

#endif // LZ_CACHED_REVERSE_ITERATOR_HPP
