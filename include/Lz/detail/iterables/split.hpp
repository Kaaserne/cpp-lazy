#pragma once

#ifndef LZ_SPLIT_ITERABLE_HPP
#define LZ_SPLIT_ITERABLE_HPP

#include <Lz/detail/iterators/split.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {
template<bool IsIterable, class ValueType, class Iterable, class Iterable2>
class split_iterable : public lazy_view {
    ref_or_view<Iterable> _iterable;
    ref_or_view<Iterable2> _delimiter;

public:
    using const_iterator =
        split_iterator<ValueType, iter_t<Iterable>, sentinel_t<Iterable>, iter_t<Iterable2>, sentinel_t<Iterable2>>;
    using iterator = const_iterator;
    using value_type = basic_iterable<iterator>;

    template<class I, class I2>
    constexpr split_iterable(I&& iterable, I2&& delimiter) :
        _iterable{ std::forward<I>(iterable) },
        _delimiter{ std::forward<I2>(delimiter) } {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const {
        return { std::begin(_iterable), std::end(_iterable), std::begin(_delimiter), std::end(_delimiter) };
    }

    LZ_NODISCARD constexpr default_sentinel end() const noexcept {
        return {};
    }
};

template<class ValueType, class Iterable, class T>
class split_iterable<false, ValueType, Iterable, T> : public lazy_view {
    ref_or_view<Iterable> _iterable;
    T _delimiter;

public:
    using const_iterator = split_single_iterator<ValueType, iter_t<Iterable>, sentinel_t<Iterable>, T>;
    using iterator = const_iterator;
    using value_type = basic_iterable<iterator>;

    template<class I>
    constexpr split_iterable(I&& iterable, T delimiter) :
        _iterable{ std::forward<I>(iterable) },
        _delimiter{ std::move(delimiter) } {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { std::begin(_iterable), std::end(_iterable), _delimiter };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return { std::begin(std::move(_iterable)), std::end(std::move(_iterable)), std::move(_delimiter) };
    }

    LZ_NODISCARD constexpr default_sentinel end() const noexcept {
        return {};
    }
};

} // namespace detail
} // namespace lz

#endif // LZ_SPLIT_ITERABLE_HPP
