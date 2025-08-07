#pragma once

#ifndef LZ_TAKE_WHILE_ITERATOR_HPP
#define LZ_TAKE_WHILE_ITERATOR_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/func_container.hpp>
#include <Lz/detail/iterators/common.hpp>
#include <Lz/detail/traits.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {

template<class Iterable, class UnaryPredicate>
class take_while_iterator
    : public iterator<take_while_iterator<Iterable, UnaryPredicate>, ref_t<iter_t<Iterable>>,
                      fake_ptr_proxy<ref_t<iter_t<Iterable>>>, diff_type<iter_t<Iterable>>,
                      common_type<iter_cat_t<iter_t<Iterable>>, std::bidirectional_iterator_tag>, default_sentinel_t> {

    using iter = iter_t<Iterable>;

    iter _iterator;
    Iterable _iterable;
    mutable UnaryPredicate _unary_predicate;

    using traits = std::iterator_traits<iter>;

    LZ_CONSTEXPR_CXX_14 void incremented_check() {
        if (_iterator != std::end(_iterable) && !_unary_predicate(*_iterator)) {
            _iterator = std::end(_iterable);
        }
    }

public:
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

#ifdef LZ_HAS_CONCEPTS

    constexpr take_while_iterator()
        requires std::default_initializable<iter> && std::default_initializable<UnaryPredicate>
    = default;

#else

    template<class I = iter,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<Iterable>::value &&
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
        if (_iterator == std::begin(_iterable)) {
            incremented_check();
        }
    }

    LZ_CONSTEXPR_CXX_14 take_while_iterator& operator=(default_sentinel_t) {
        _iterator = std::end(_iterable);
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT_INCREMENTABLE(!eq(lz::default_sentinel));
        ++_iterator;
        incremented_check();
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        LZ_ASSERT_DECREMENTABLE(_iterator != std::begin(_iterable));
        if (_iterator != std::end(_iterable)) {
            --_iterator;
            return;
        }
        do {
            --_iterator;
        } while (_iterator != std::begin(_iterable) && !_unary_predicate(*_iterator));
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        LZ_ASSERT_DEREFERENCABLE(!eq(lz::default_sentinel));
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const take_while_iterator& b) const noexcept {
        LZ_ASSERT_COMPTABLE(std::end(_iterable) == std::end(b._iterable) && std::begin(_iterable) == std::begin(b._iterable));
        return _iterator == b._iterator;
    }

    constexpr bool eq(default_sentinel_t) const noexcept {
        return _iterator == std::end(_iterable);
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_TAKE_WHILE_ITERATOR_HPP