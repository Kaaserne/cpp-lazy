#pragma once

#ifndef LZ_REPEAT_ITERATOR_HPP
#define LZ_REPEAT_ITERATOR_HPP

#include <Lz/detail/compiler_config.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/iterator.hpp>
#include <Lz/detail/procs/assert.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <Lz/detail/traits/remove_ref.hpp>
#include <Lz/util/default_sentinel.hpp>

namespace lz {
namespace detail {

template<class, bool /* is inf */>
class repeat_iterator;

template<class T>
class repeat_iterator<T, false> : public iterator<repeat_iterator<T, false>, T, fake_ptr_proxy<T>, ptrdiff_t,
                                                  std::random_access_iterator_tag, default_sentinel_t> {
    T _to_repeat{};
    ptrdiff_t _amount{};

public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = remove_cvref_t<T>;
    using difference_type = ptrdiff_t;
    using pointer = fake_ptr_proxy<T>;
    using reference = T;

    constexpr repeat_iterator(const repeat_iterator&) = default;
    LZ_CONSTEXPR_CXX_14 repeat_iterator& operator=(const repeat_iterator&) = default;

#ifdef LZ_HAS_CONCEPTS

    constexpr repeat_iterator()
        requires(std::default_initializable<T>)
    = default;

#else

    template<class U = T, class = enable_if_t<std::is_default_constructible<U>::value>>
    constexpr repeat_iterator() noexcept(std::is_nothrow_default_constructible<T>::value) {
    }

#endif

    constexpr repeat_iterator(T to_repeat, const ptrdiff_t start) : _to_repeat{ std::forward<T>(to_repeat) }, _amount{ start } {
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
        LZ_ASSERT_ADDABLE(value < 0 ? true : _amount >= value);
        _amount -= value;
    }

    constexpr difference_type difference(const repeat_iterator& other) const noexcept {
        return other._amount - _amount;
    }

    constexpr difference_type difference(default_sentinel_t) const noexcept {
        return -_amount;
    }
};

template<class T>
class repeat_iterator<T, true> : public iterator<repeat_iterator<T, true>, T, fake_ptr_proxy<T>, std::ptrdiff_t,
                                                 std::forward_iterator_tag, default_sentinel_t> {
    T _to_repeat{};

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = remove_cvref_t<T>;
    using difference_type = std::ptrdiff_t;
    using pointer = fake_ptr_proxy<T>;
    using reference = T;

    constexpr repeat_iterator(const repeat_iterator&) = default;
    LZ_CONSTEXPR_CXX_14 repeat_iterator& operator=(const repeat_iterator&) = default;

#ifdef LZ_HAS_CONCEPTS

    constexpr repeat_iterator()
        requires(std::default_initializable<T>)
    = default;

#else

    template<class U = T, class = enable_if_t<std::is_default_constructible<U>::value>>
    constexpr repeat_iterator() {
    }

#endif

    explicit constexpr repeat_iterator(T to_repeat) : _to_repeat{ std::forward<T>(to_repeat) } {
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
