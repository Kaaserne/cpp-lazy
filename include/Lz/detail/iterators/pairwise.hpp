#pragma once

#ifndef LZ_PAIRWISE_ITERATOR_HPP
#define LZ_PAIRWISE_ITERATOR_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/procs.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {

template<class Iterable, class = void>
class pairwise_iterator;

template<class Iterable>
class pairwise_iterator<Iterable, enable_if<is_ra<iter_t<Iterable>>::value>>
    : public iterator<pairwise_iterator<Iterable>, basic_iterable<iter_t<Iterable>>,
                      fake_ptr_proxy<basic_iterable<iter_t<Iterable>>>, diff_iterable_t<Iterable>, iter_cat_t<iter_t<Iterable>>,
                      default_sentinel_t> {

    using iter = iter_t<Iterable>;
    using traits = std::iterator_traits<iter>;

    iter _iterator;
    Iterable _iterable;
    std::size_t _pair_size{};

public:
    using value_type = basic_iterable<iter>;
    using reference = value_type;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = typename traits::difference_type;

#ifdef LZ_HAS_CONCEPTS

    constexpr pairwise_iterator()
        requires(std::default_initializable<iter> && std::default_initializable<Iterable>)
    = default;

#else

    template<class I = iter,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<Iterable>::value>>
    constexpr pairwise_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                           std::is_nothrow_default_constructible<Iterable>::value) {
    }

#endif

    template<class I>
    LZ_CONSTEXPR_CXX_14 pairwise_iterator(I&& iterable, iter it, const std::size_t pair_size) :
        _iterator{ std::move(it) },
        _iterable{ std::forward<I>(iterable) },
        _pair_size{ pair_size } {
        LZ_ASSERT(_pair_size > 0, "Size must be greater than zero");
    }

    LZ_CONSTEXPR_CXX_14 pairwise_iterator operator=(default_sentinel_t) {
        _iterator = std::end(_iterable);
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        LZ_ASSERT_DEREFERENCABLE(!eq(lz::default_sentinel));
        return { _iterator, _iterator + static_cast<difference_type>(_pair_size) };
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT_INCREMENTABLE(!eq(lz::default_sentinel));
        ++_iterator;
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        LZ_ASSERT_DECREMENTABLE(_iterator != _iterable.begin());
        --_iterator;
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const pairwise_iterator& other) const {
        LZ_ASSERT_COMPATIBLE(_iterable.end() == other._iterable.end() && _iterable.begin() == other._iterable.begin() &&
                             _pair_size == other._pair_size);
        return _iterator == other._iterator;
    }

    constexpr bool eq(default_sentinel_t) const {
        return _iterator == std::end(_iterable);
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type offset) {
        LZ_ASSERT_SUB_ADDABLE(_iterator.end() - _iterator >= std::abs(offset));
        _iterator += offset;
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const pairwise_iterator& other) const {
        LZ_ASSERT_COMPATIBLE(_iterable.end() == other._iterable.end() && _pair_size == other._pair_size);
        const auto diff = (_iterator - other._iterator);
        return diff;
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(default_sentinel_t) const {
        return _iterable.end() - _iterator;
    }
};

template<class Iterable>
class pairwise_iterator<Iterable, enable_if<!is_ra<iter_t<Iterable>>::value>>
    : public iterator<pairwise_iterator<Iterable>, basic_iterable<iter_t<Iterable>>,
                      fake_ptr_proxy<basic_iterable<iter_t<Iterable>>>, diff_iterable_t<Iterable>, iter_cat_iterable_t<Iterable>,
                      default_sentinel_t> {
    using iter = iter_t<Iterable>;
    using traits = std::iterator_traits<iter>;

    iter _iterator;
    iter _last;
    Iterable _iterable;
    std::size_t _pair_size{};

public:
    using value_type = basic_iterable<iter>;
    using reference = value_type;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = typename traits::difference_type;

#ifdef LZ_HAS_CONCEPTS

    constexpr pairwise_iterator()
        requires(std::default_initializable<iter> && std::default_initializable<Iterable>)
    = default;

#else

    template<class I = iter,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<Iterable>::value>>
    constexpr pairwise_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                           std::is_nothrow_default_constructible<Iterable>::value) {
    }

#endif

    template<class I>
    LZ_CONSTEXPR_CXX_14 pairwise_iterator(I&& iterable, iter it, const std::size_t pair_size) :
        _iterator{ std::move(it) },
        _last{ _iterator },
        _iterable{ std::forward<I>(iterable) },
        _pair_size{ pair_size } {
        LZ_ASSERT(_pair_size > 0, "Size must be greater than zero");
    }

    LZ_CONSTEXPR_CXX_14 pairwise_iterator& operator=(default_sentinel_t) {
        _iterator = _iterable.end();
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        LZ_ASSERT_DEREFERENCABLE(!eq(lz::default_sentinel));
        return { _iterator, _last };
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT_INCREMENTABLE(!eq(lz::default_sentinel));
        if (_last == _iterable.end()) {
            _iterator = _iterable.end();
            return;
        }
        ++_iterator;
        ++_last;
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        LZ_ASSERT_DECREMENTABLE(_iterator != _iterable.begin());
        // TODO
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const pairwise_iterator& other) const {
        LZ_ASSERT_COMPATIBLE(_iterable.end() == other._iterable.end() && _pair_size == other._pair_size);
        return _iterator == other._iterator;
    }

    constexpr bool eq(default_sentinel_t) const {
        return _iterator == _iterable.end();
    }
};
} // namespace detail
} // namespace lz

#endif
