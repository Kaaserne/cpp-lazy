#pragma once

#ifndef LZ_REPEAT_ITERATOR_HPP
#define LZ_REPEAT_ITERATOR_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {

template<bool /* is infinite loop */, class>
class repeat_iterator;

template<class T>
class repeat_iterator<false, T>
    : public iterator<repeat_iterator<false, T>, T&, T*, std::ptrdiff_t, std::forward_iterator_tag, default_sentinel> {
    mutable T _to_repeat;
    std::size_t _amount{};

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

#ifdef LZ_HAS_CONCEPTS

    constexpr repeat_iterator()
        requires std::default_initializable<T>
    = default;

#else

    template<class U = T, class = enable_if<std::is_default_constructible<U>::value>>
    constexpr repeat_iterator() {
    }

#endif

    constexpr repeat_iterator(T to_repeat, const std::size_t start) : _to_repeat{ std::move(to_repeat) }, _amount{ start } {
    }

    LZ_CONSTEXPR_CXX_14 repeat_iterator& operator=(default_sentinel) {
        _to_repeat = T();
        _amount = 0;
        return *this;
    }

    constexpr reference dereference() const noexcept {
        return _to_repeat;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const noexcept {
        return std::addressof(_to_repeat);
    }

    LZ_CONSTEXPR_CXX_14 void increment() noexcept {
        --_amount;
    }

    constexpr bool eq(const repeat_iterator& b) const noexcept {
        return _amount == b._amount;
    }

    constexpr bool eq(default_sentinel) const noexcept {
        return _amount == 0;
    }
};

template<class T>
class repeat_iterator<true, T>
    : public iterator<repeat_iterator<true, T>, T&, T*, std::ptrdiff_t, std::forward_iterator_tag, default_sentinel> {
    mutable T _to_repeat;

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

#ifdef LZ_HAS_CONCEPTS

    constexpr repeat_iterator()
        requires std::default_initializable<T>
    = default;

#else

    template<class U = T, class = enable_if<std::is_default_constructible<U>::value>>
    constexpr repeat_iterator() {
    }

#endif

    constexpr repeat_iterator(T to_repeat) : _to_repeat{ std::move(to_repeat) } {
    }

    LZ_CONSTEXPR_CXX_14 repeat_iterator& operator=(default_sentinel) {
        return *this;
    }

    constexpr reference dereference() const noexcept {
        return _to_repeat;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const noexcept {
        return std::addressof(_to_repeat);
    }

    LZ_CONSTEXPR_CXX_14 void increment() const noexcept {
    }

    constexpr bool eq(const repeat_iterator&) const noexcept {
        return false;
    }

    constexpr bool eq(default_sentinel) const noexcept {
        return false;
    }
};

} // namespace detail
} // namespace lz

#endif