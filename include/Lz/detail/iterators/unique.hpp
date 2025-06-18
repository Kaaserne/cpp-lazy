#pragma once

#ifndef LZ_UNIQUE_ITERATOR_HPP
#define LZ_UNIQUE_ITERATOR_HPP

#include <Lz/detail/algorithm.hpp>
#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/iterator_base.hpp>
#include <algorithm>

namespace lz {
namespace detail {

template<class Iterable, class BinaryPredicate>
class unique_iterator
    : public iterator<unique_iterator<Iterable, BinaryPredicate>, ref_t<iter_t<Iterable>>,
                      fake_ptr_proxy<ref_t<iter_t<Iterable>>>, diff_type<iter_t<Iterable>>,
                      common_type<std::bidirectional_iterator_tag, iter_cat_t<iter_t<Iterable>>>, default_sentinel> {

    using iter = iter_t<Iterable>;
    using traits = std::iterator_traits<iter>;

    iter _iterator;
    Iterable _iterable;
    mutable BinaryPredicate _predicate;

public:
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

#ifdef LZ_HAS_CONCEPTS

    constexpr unique_iterator()
        requires std::default_initializable<iter> && std::default_initializable<Iterable> &&
                     std::default_initializable<BinaryPredicate>
    = default;

#else

    template<class I = iter,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<Iterable>::value &&
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

    LZ_CONSTEXPR_CXX_14 unique_iterator& operator=(default_sentinel) {
        _iterator = std::end(_iterable);
        return *this;
    }

    constexpr reference dereference() const {
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        using std::adjacent_find;
        _iterator = adjacent_find(std::move(_iterator), std::end(_iterable), _predicate);

        if (_iterator != std::end(_iterable)) {
            ++_iterator;
        }
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        --_iterator;
        if (_iterator == std::begin(_iterable)) {
            return;
        }

        auto next = _iterator;
        for (--next; next != std::begin(_iterable); --next, --_iterator) {
            if (!_predicate(*next, *_iterator)) {
                return;
            }
        }
        _iterator = std::move(next);
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const unique_iterator& b) const {
        LZ_ASSERT(std::end(_iterable) == std::end(b._iterable), "unique_iterators are not compatible");
        return _iterator == b._iterator;
    }

    constexpr bool eq(default_sentinel) const {
        return _iterator == std::end(_iterable);
    }
};

} // namespace detail
} // namespace lz

#endif