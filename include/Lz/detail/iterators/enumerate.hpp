#pragma once

#ifndef LZ_ENUMERATE_ITERATOR_HPP
#define LZ_ENUMERATE_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/iterator.hpp>
#include <Lz/detail/traits.hpp>

namespace lz {
namespace detail {

template<class Iterable, class IntType, class = void>
class enumerate_iterator;

// TODO for all operator= which ar enot sentinel_, we want to make an immutable ojbect
template<class Iterable, class IntType>
class enumerate_iterator<Iterable, IntType, enable_if<is_bidi<iter_t<Iterable>>::value>>
    : public iterator<enumerate_iterator<Iterable, IntType>, std::pair<IntType, ref_t<iter_t<Iterable>>>,
                      fake_ptr_proxy<std::pair<IntType, ref_t<iter_t<Iterable>>>>, diff_type<iter_t<Iterable>>,
                      iter_cat_t<iter_t<Iterable>>, sentinel_with<IntType>> {

    using iter = iter_t<Iterable>;
    Iterable _iterable{};
    iter _iterator{};
    IntType _index{};

    using traits = std::iterator_traits<iter>;

public:
    using value_type = std::pair<IntType, typename traits::value_type>;
    using reference = std::pair<IntType, typename traits::reference>;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = typename traits::difference_type;

    constexpr enumerate_iterator(const enumerate_iterator&) = default;
    LZ_CONSTEXPR_CXX_14 enumerate_iterator& operator=(const enumerate_iterator&) = default;

#ifdef LZ_HAS_CONCEPTS

    constexpr enumerate_iterator()
        requires(std::default_initializable<Iterator> && std::default_initializable<Iterable>)
    = default;

#else

    template<class I = iter,
             class = enable_if_t<std::is_default_constructible<I>::value && std::is_default_constructible<Iterable>::value>>
    constexpr enumerate_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                            std::is_nothrow_default_constructible<Iterable>::value) {
    }

#endif

    template<class I>
    constexpr enumerate_iterator(I&& iterable, iter it, const IntType start) :
        _iterable{ std::forward<I>(iterable) },
        _iterator{ std::move(it) },
        _index{ start } {
    }

    LZ_CONSTEXPR_CXX_14 enumerate_iterator& operator=(sentinel_with<IntType> s) {
        _iterator = _iterable.end();
        // TODO only store iterable if bidirectional. forward don't need. do this for all eager_size operator=
        _index = s.value + static_cast<IntType>(lz::eager_size(_iterable));
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        ++_index;
        ++_iterator;
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        --_index;
        --_iterator;
    }

    constexpr reference dereference() const {
        return { _index, *_iterator };
    }

    constexpr pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    constexpr bool eq(const enumerate_iterator& other) const {
        return _iterator == other._iterator;
    }

    constexpr bool eq(sentinel_with<IntType>) const {
        return _iterator == _iterable.end();
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type n) {
        _index += static_cast<IntType>(n);
        _iterator += n;
    }

    constexpr difference_type difference(const enumerate_iterator& other) const {
        return _iterator - other._iterator;
    }

    constexpr difference_type difference(sentinel_with<IntType>) const {
        return _iterator - _iterable.end();
    }
};

template<class Iterable, class IntType>
class enumerate_iterator<Iterable, IntType, enable_if<!is_bidi<iter_t<Iterable>>::value>>
    : public iterator<enumerate_iterator<Iterable, IntType>, std::pair<IntType, ref_t<iter_t<Iterable>>>,
                      fake_ptr_proxy<std::pair<IntType, ref_t<iter_t<Iterable>>>>, diff_type<iter_t<Iterable>>,
                      iter_cat_t<iter_t<Iterable>>, sentinel_with<sentinel_t<Iterable>>> {

    using iter = iter_t<Iterable>;
    iter _iterator{};
    IntType _index{};

    using traits = std::iterator_traits<iter>;

public:
    using value_type = std::pair<IntType, typename traits::value_type>;
    using reference = std::pair<IntType, typename traits::reference>;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = typename traits::difference_type;

    constexpr enumerate_iterator(const enumerate_iterator&) = default;
    LZ_CONSTEXPR_CXX_14 enumerate_iterator& operator=(const enumerate_iterator&) = default;

#ifdef LZ_HAS_CONCEPTS

    constexpr enumerate_iterator()
        requires(std::default_initializable<iter>)
    = default;

#else

    template<class I = iter, class = enable_if_t<std::is_default_constructible<I>::value>>
    constexpr enumerate_iterator() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    constexpr enumerate_iterator(iter it, const IntType start) : _iterator{ std::move(it) }, _index{ start } {
    }

    LZ_CONSTEXPR_CXX_14 enumerate_iterator& operator=(sentinel_with<sentinel_t<Iterable>> s) {
        _iterator = std::move(s.value);
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        ++_index;
        ++_iterator;
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        --_index;
        --_iterator;
    }

    constexpr reference dereference() const {
        return { _index, *_iterator };
    }

    constexpr pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    constexpr bool eq(const enumerate_iterator& other) const {
        return _iterator == other._iterator;
    }

    constexpr bool eq(sentinel_with<iter> other) const {
        return _iterator == other.value;
    }
};
} // namespace detail
} // namespace lz

#endif
