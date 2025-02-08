#pragma once

#ifndef LZ_REPEAT_ITERATOR_HPP
#define LZ_REPEAT_ITERATOR_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/iterator_base.hpp>
#include <limits>

namespace lz {
namespace detail {
template<class T>
class repeat_iterator : public iter_base<repeat_iterator<T>, T&, T*, std::ptrdiff_t, std::forward_iterator_tag, default_sentinel> {
    mutable T _to_repeat{};
    std::size_t _amount{};

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    constexpr repeat_iterator(T to_repeat, const std::size_t start) : _to_repeat(std::move(to_repeat)), _amount(start) {
    }

    constexpr repeat_iterator() = default;

    constexpr reference dereference() const noexcept {
        return _to_repeat;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const noexcept {
        return std::addressof(_to_repeat);
    }

    LZ_CONSTEXPR_CXX_14 void increment() noexcept {
        if (_amount != (std::numeric_limits<std::size_t>::max)()) {
            --_amount;
        }
    }

    constexpr bool eq(const repeat_iterator& b) const noexcept {
        return _amount == b._amount;
    }

    constexpr bool eq(default_sentinel) const noexcept {
        return _amount == 0;
    }
};
} // namespace detail
} // namespace lz

#endif