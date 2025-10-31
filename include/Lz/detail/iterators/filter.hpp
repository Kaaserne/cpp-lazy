#pragma once

#ifndef LZ_FILTER_ITERATOR_HPP
#define LZ_FILTER_ITERATOR_HPP

#include <Lz/algorithm/find_if.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/iterator.hpp>
#include <Lz/detail/procs/assert.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>
#include <Lz/util/default_sentinel.hpp>
#include <algorithm>

namespace lz {
namespace detail {

template<class Iterable, class UnaryPredicate>
class filter_iterator
    : public iterator<filter_iterator<Iterable, UnaryPredicate>, ref_t<iter_t<Iterable>>, fake_ptr_proxy<ref_t<iter_t<Iterable>>>,
                      diff_type<iter_t<Iterable>>, bidi_strongest_cat<iter_cat_t<iter_t<Iterable>>>, default_sentinel_t> {

    using it = iter_t<Iterable>;
    using traits = std::iterator_traits<it>;

public:
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

    constexpr filter_iterator(const filter_iterator&) = default;
    LZ_CONSTEXPR_CXX_14 filter_iterator& operator=(const filter_iterator&) = default;

    LZ_CONSTEXPR_CXX_14 void find() {
        _iterator = detail::find_if(std::move(_iterator), _iterable.end(), _predicate);
    }

private:
    it _iterator{};
    Iterable _iterable{};
    mutable UnaryPredicate _predicate{};

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr filter_iterator()
        requires(std::default_initializable<Iterable> && std::default_initializable<it> &&
                 std::default_initializable<UnaryPredicate>)
    = default;

#else

    template<class I = it,
             class = enable_if_t<std::is_default_constructible<I>::value && std::is_default_constructible<Iterable>::value &&
                                 std::is_default_constructible<UnaryPredicate>::value>>
    constexpr filter_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                         std::is_nothrow_default_constructible<Iterable>::value &&
                                         std::is_nothrow_default_constructible<UnaryPredicate>::value) {
    }

#endif

    template<class I>
    LZ_CONSTEXPR_CXX_14 filter_iterator(I&& iterable, it iter, UnaryPredicate up) :
        _iterator{ std::move(iter) },
        _iterable{ std::forward<I>(iterable) },
        _predicate{ std::move(up) } {
        find();
    }

    LZ_CONSTEXPR_CXX_14 filter_iterator& operator=(default_sentinel_t) {
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
        ++_iterator;
        find();
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        LZ_ASSERT_DECREMENTABLE(_iterator != _iterable.begin());
        do {
            --_iterator;
        } while (!_predicate(*_iterator) && _iterator != _iterable.begin());
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const filter_iterator& other) const {
        LZ_ASSERT_COMPATIBLE(_iterable.end() == other._iterable.end() && _iterable.begin() == other._iterable.begin());
        return _iterator == other._iterator;
    }

    constexpr bool eq(default_sentinel_t) const {
        return _iterator == _iterable.end();
    }
};
} // namespace detail
} // namespace lz

#endif
