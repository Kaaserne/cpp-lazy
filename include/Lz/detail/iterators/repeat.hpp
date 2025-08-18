#pragma once

#ifndef LZ_REPEAT_ITERATOR_HPP
#define LZ_REPEAT_ITERATOR_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/iterator.hpp>
#include <Lz/detail/procs.hpp>

namespace lz {
namespace detail {

template<bool /* is infinite loop */, class>
class repeat_iterator;

template<class T>
class repeat_iterator<false, T> : public iterator<repeat_iterator<false, T>, T, fake_ptr_proxy<T>, std::ptrdiff_t,
                                                  std::random_access_iterator_tag, default_sentinel_t> {
    T _to_repeat;
    size_t _amount{};

public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = remove_cvref<T>;
    using difference_type = std::ptrdiff_t;
    using pointer = fake_ptr_proxy<T>;
    using reference = T;

#ifdef LZ_HAS_CONCEPTS

    constexpr repeat_iterator()
        requires(std::default_initializable<T>)
    = default;

#else

    template<class U = T, class = enable_if_t<std::is_default_constructible<U>::value>>
    constexpr repeat_iterator() noexcept(std::is_nothrow_default_constructible<T>::value) {
    }

#endif

    constexpr repeat_iterator(T to_repeat, const size_t start) : _to_repeat{ std::forward<T>(to_repeat) }, _amount{ start } {
    }

    LZ_CONSTEXPR_CXX_14 repeat_iterator& operator=(default_sentinel_t) {
        _amount = 0;
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const noexcept {
        LZ_ASSERT_DEREFERENCABLE(_amount != 0);
        return _to_repeat;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const noexcept {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() noexcept {
        LZ_ASSERT_INCREMENTABLE(_amount != 0);
        --_amount;
    }

    LZ_CONSTEXPR_CXX_14 void decrement() noexcept {
        ++_amount;
    }

    constexpr bool eq(const repeat_iterator& other) const noexcept {
        return _amount == other._amount;
    }

    constexpr bool eq(default_sentinel_t) const noexcept {
        return _amount == 0;
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type value) noexcept {
        LZ_ASSERT_ADDABLE(value > 0 ? _amount >= static_cast<size_t>(value) : true);
        _amount -= static_cast<size_t>(value);
    }

    constexpr difference_type difference(const repeat_iterator& other) const noexcept {
        return static_cast<difference_type>(other._amount) - static_cast<difference_type>(_amount);
    }

    constexpr difference_type difference(default_sentinel_t) const noexcept {
        return -static_cast<difference_type>(_amount);
    }
};

template<class T>
class repeat_iterator<true, T> : public iterator<repeat_iterator<true, T>, T, fake_ptr_proxy<T>, std::ptrdiff_t,
                                                 std::forward_iterator_tag, default_sentinel_t> {
    T _to_repeat;

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = remove_cvref<T>;
    using difference_type = std::ptrdiff_t;
    using pointer = fake_ptr_proxy<T>;
    using reference = T;

#ifdef LZ_HAS_CONCEPTS

    constexpr repeat_iterator()
        requires(std::default_initializable<T>)
    = default;

#else

    template<class U = T, class = enable_if_t<std::is_default_constructible<U>::value>>
    constexpr repeat_iterator() {
    }

#endif

    explicit constexpr repeat_iterator(const T& to_repeat) : _to_repeat{ to_repeat } {
    }

    LZ_CONSTEXPR_CXX_14 repeat_iterator& operator=(default_sentinel_t) {
        return *this;
    }

    constexpr reference dereference() const noexcept {
        return _to_repeat;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const noexcept {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() const noexcept {
    }

    constexpr bool eq(const repeat_iterator&) const noexcept {
        return false;
    }

    constexpr bool eq(default_sentinel_t) const noexcept {
        return false;
    }
};

} // namespace detail
} // namespace lz

#endif
