#pragma once

#ifndef LZ_UNIQUE_ITERATOR_HPP
#define LZ_UNIQUE_ITERATOR_HPP

#include <Lz/algorithm/adjacent_find.hpp>
#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/iterator.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>
#include <Lz/util/default_sentinel.hpp>
#include <algorithm>

namespace lz {
namespace detail {

template<class Iterable, class BinaryPredicate>
class unique_iterator : public iterator<unique_iterator<Iterable, BinaryPredicate>, ref_t<iter_t<Iterable>>,
                                        fake_ptr_proxy<ref_t<iter_t<Iterable>>>, diff_type<iter_t<Iterable>>,
                                        bidi_strongest_cat<iter_cat_t<iter_t<Iterable>>>, default_sentinel_t> {

    using iter = iter_t<Iterable>;
    using traits = std::iterator_traits<iter>;

    iter _iterator{};
    Iterable _iterable{};
    mutable BinaryPredicate _predicate{};

public:
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

    constexpr unique_iterator(const unique_iterator&) = default;
    LZ_CONSTEXPR_CXX_14 unique_iterator& operator=(const unique_iterator&) = default;

#ifdef LZ_HAS_CONCEPTS

    constexpr unique_iterator()
        requires(std::default_initializable<iter> && std::default_initializable<Iterable> &&
                 std::default_initializable<BinaryPredicate>)
    = default;

#else

    template<class I = iter,
             class = enable_if_t<std::is_default_constructible<I>::value && std::is_default_constructible<Iterable>::value &&
                                 std::is_default_constructible<BinaryPredicate>::value>>
    constexpr unique_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                         std::is_nothrow_default_constructible<Iterable>::value &&
                                         std::is_nothrow_default_constructible<BinaryPredicate>::value) {
    }

#endif

    template<class I>
    constexpr unique_iterator(I&& iterable, iter it, BinaryPredicate binary_predicate) :
        _iterator{ std::move(it) },
        _iterable{ std::forward<I>(iterable) },
        _predicate{ std::move(binary_predicate) } {
    }

    LZ_CONSTEXPR_CXX_14 unique_iterator& operator=(default_sentinel_t) {
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

        using std::adjacent_find;
        _iterator = adjacent_find(std::move(_iterator), _iterable.end(), _predicate);

        if (_iterator != _iterable.end()) {
            ++_iterator;
        }
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        LZ_ASSERT_DECREMENTABLE(_iterator != _iterable.begin());
        --_iterator;
        if (_iterator == _iterable.begin()) {
            return;
        }

        auto next = _iterator;
        for (--next; next != _iterable.begin(); --next, --_iterator) {
            if (!_predicate(*next, *_iterator)) {
                return;
            }
        }
        _iterator = std::move(next);
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const unique_iterator& other) const {
        LZ_ASSERT_COMPATIBLE(_iterable.end() == other._iterable.end());
        return _iterator == other._iterator;
    }

    constexpr bool eq(default_sentinel_t) const {
        return _iterator == _iterable.end();
    }
};

} // namespace detail
} // namespace lz

#endif
