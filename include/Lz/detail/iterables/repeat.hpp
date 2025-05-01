#pragma once

#ifndef LZ_REPEAT_ITERABLE_HPP
#define LZ_REPEAT_ITERABLE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/iterators/repeat.hpp>

namespace lz {
namespace detail {

template<bool /* is infinite loop */, class>
class repeat_iterable;

template<class T>
class repeat_iterable<false, T> : public lazy_view {
    T _value;
    std::size_t _amount;

public:
    using iterator = repeat_iterator<false, T>;
    using const_iterator = iterator;
    using value_type = T;

    constexpr repeat_iterable(T value, const std::size_t amount) : _value{ std::move(value) }, _amount{ amount } {
    }

    constexpr repeat_iterable() = default;

    LZ_NODISCARD constexpr std::size_t size() const noexcept {
        return _amount;
    }

    LZ_NODISCARD constexpr iterator begin() const& {
        return { _value, _amount };
    }

    LZ_NODISCARD constexpr iterator begin() && {
        return { std::move(_value), _amount };
    }

    LZ_NODISCARD constexpr default_sentinel end() const noexcept {
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

    constexpr repeat_iterable(T value) : _value{ std::move(value) } {
    }

    constexpr repeat_iterable() = default;

    LZ_NODISCARD constexpr iterator begin() const& {
        return { _value };
    }

    LZ_NODISCARD constexpr iterator begin() && {
        return { std::move(_value) };
    }

    LZ_NODISCARD constexpr default_sentinel end() const noexcept {
        return {};
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_REPEAT_ITERABLE_HPP
