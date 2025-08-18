#pragma once

#ifndef LZ_REPEAT_ITERABLE_HPP
#define LZ_REPEAT_ITERABLE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/iterators/repeat.hpp>
// todo add extra tests for references
namespace lz {
namespace detail {

template<bool /* is infinite loop */, class>
class repeat_iterable;

template<class T>
class repeat_iterable<false, T> : public lazy_view {
    T _value{};
    size_t _amount{};

public:
    using iterator = repeat_iterator<false, T>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

#ifdef LZ_HAS_CONCEPTS

    constexpr repeat_iterable()
        requires(std::default_initializable<T>)
    = default;

#else

    template<class U = T, class = enable_if_t<std::is_default_constructible<U>::value>>
    constexpr repeat_iterable() noexcept(std::is_nothrow_default_constructible<U>::value) {
    }

#endif

    constexpr repeat_iterable(T value, const size_t amount) : _value{ std::forward<T>(value) }, _amount{ amount } {
    }

    LZ_NODISCARD constexpr size_t size() const noexcept {
        return _amount;
    }

    LZ_NODISCARD constexpr iterator begin() const& {
        return { _value, _amount };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return { std::move(_value), _amount };
    }

    LZ_NODISCARD constexpr default_sentinel_t end() const noexcept {
        return {};
    }
};

template<class T>
class repeat_iterable<true, T> : public lazy_view {
    T _value;

public:
    using iterator = repeat_iterator<true, T>;
    using const_iterator = iterator;
    using value_type = T;

#ifdef LZ_HAS_CONCEPTS

    constexpr repeat_iterable()
        requires(std::default_initializable<T>)
    = default;

#else

    template<class U = T, class = enable_if_t<std::is_default_constructible<U>::value>>
    constexpr repeat_iterable() noexcept(std::is_nothrow_default_constructible<U>::value) {
    }

#endif

    explicit constexpr repeat_iterable(T value) : _value{ std::forward<T>(value) } {
    }

    LZ_NODISCARD constexpr iterator begin() const& {
        return iterator{ _value };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return iterator{ std::move(_value) };
    }

    LZ_NODISCARD constexpr default_sentinel_t end() const noexcept {
        return {};
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_REPEAT_ITERABLE_HPP
