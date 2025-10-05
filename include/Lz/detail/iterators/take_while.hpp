#pragma once

#ifndef LZ_TAKE_WHILE_ITERATOR_HPP
#define LZ_TAKE_WHILE_ITERATOR_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/func_container.hpp>
#include <Lz/detail/iterator.hpp>
#include <Lz/detail/iterators/common.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>

namespace lz {
namespace detail {

template<class Iterable, class UnaryPredicate>
class take_while_iterator : public iterator<take_while_iterator<Iterable, UnaryPredicate>, ref_t<iter_t<Iterable>>,
                                            fake_ptr_proxy<ref_t<iter_t<Iterable>>>, diff_type<iter_t<Iterable>>,
                                            bidi_strongest_cat<iter_cat_t<iter_t<Iterable>>>, default_sentinel_t> {

    using iter = iter_t<Iterable>;

    iter _iterator{};
    Iterable _iterable{};
    mutable UnaryPredicate _unary_predicate{};

    using traits = std::iterator_traits<iter>;

    LZ_CONSTEXPR_CXX_14 void incremented_check() {
        if (_iterator != _iterable.end() && !_unary_predicate(*_iterator)) {
            _iterator = _iterable.end();
        }
    }

public:
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

    constexpr take_while_iterator(const take_while_iterator&)  = default;
    LZ_CONSTEXPR_CXX_14 take_while_iterator& operator=(const take_while_iterator&) = default;

#ifdef LZ_HAS_CONCEPTS

    constexpr take_while_iterator()
        requires(std::default_initializable<iter> && std::default_initializable<UnaryPredicate>)
    = default;

#else

    template<class I = iter,
             class = enable_if_t<std::is_default_constructible<I>::value && std::is_default_constructible<Iterable>::value &&
                                 std::is_default_constructible<UnaryPredicate>::value>>
    constexpr take_while_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                             std::is_nothrow_default_constructible<Iterable>::value &&
                                             std::is_nothrow_default_constructible<UnaryPredicate>::value) {
    }

#endif

    template<class I>
    LZ_CONSTEXPR_CXX_14 take_while_iterator(I&& iterable, iter it, UnaryPredicate unary_predicate) :
        _iterator{ std::move(it) },
        _iterable{ std::forward<I>(iterable) },
        _unary_predicate{ std::move(unary_predicate) } {
        if (_iterator == _iterable.begin()) {
            incremented_check();
        }
    }

    LZ_CONSTEXPR_CXX_14 take_while_iterator& operator=(default_sentinel_t) {
        _iterator = _iterable.end();
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT_INCREMENTABLE(!eq(lz::default_sentinel));
        ++_iterator;
        incremented_check();
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        LZ_ASSERT_DECREMENTABLE(_iterator != _iterable.begin());
        if (_iterator != _iterable.end()) {
            --_iterator;
            return;
        }
        do {
            --_iterator;
        } while (_iterator != _iterable.begin() && !_unary_predicate(*_iterator));
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        LZ_ASSERT_DEREFERENCABLE(!eq(lz::default_sentinel));
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const take_while_iterator& other) const noexcept {
        LZ_ASSERT_COMPATIBLE(_iterable.end() == other._iterable.end() && _iterable.begin() == other._iterable.begin());
        return _iterator == other._iterator;
    }

    constexpr bool eq(default_sentinel_t) const noexcept {
        return _iterator == _iterable.end();
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_TAKE_WHILE_ITERATOR_HPP
