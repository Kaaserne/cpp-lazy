#pragma once

#ifndef LZ_SPLIT_ITERABLE_HPP
#define LZ_SPLIT_ITERABLE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/iterators/split.hpp>
#include <Lz/detail/maybe_owned.hpp>

namespace lz {
namespace detail {
template<class ValueType, class Iterable, class Iterable2, class = void>
class split_iterable;

template<class ValueType, class Iterable, class Iterable2>
class split_iterable<ValueType, Iterable, Iterable2, enable_if<is_iterable<Iterable2>::value>> : public lazy_view {
    maybe_owned<Iterable> _iterable;
    maybe_owned<Iterable2> _delimiter;

public:
    using const_iterator =
        split_iterator<ValueType, iter_t<Iterable>, sentinel_t<Iterable>, iter_t<Iterable2>, sentinel_t<Iterable2>>;
    using iterator = const_iterator;
    using value_type = basic_iterable<iterator>;

#ifdef LZ_HAS_CONCEPTS

    constexpr split_iterable()
        requires(std::default_initializable<maybe_owned<Iterable>> && std::default_initializable<maybe_owned<Iterable2>>)
    = default;

#else

    template<class I = decltype(_iterable), class = enable_if<std::is_default_constructible<I>::value &&
                                                              std::is_default_constructible<maybe_owned<Iterable2>>::value>>
    constexpr split_iterable() noexcept(std::is_nothrow_default_constructible<maybe_owned<Iterable>>::value &&
                                        std::is_nothrow_default_constructible<maybe_owned<Iterable2>>::value) {
    }

#endif

    template<class I, class I2>
    constexpr split_iterable(I&& iterable, I2&& delimiter) :
        _iterable{ std::forward<I>(iterable) },
        _delimiter{ std::forward<I2>(delimiter) } {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const {
        return { _iterable.begin(), _iterable.end(), _delimiter.begin(), _delimiter.end() };
    }

    LZ_NODISCARD constexpr default_sentinel_t end() const noexcept {
        return {};
    }
};

template<class ValueType, class Iterable, class T>
class split_iterable<ValueType, Iterable, T, enable_if<!is_iterable<T>::value>> : public lazy_view {
    maybe_owned<Iterable> _iterable;
    T _delimiter{};

public:
    using const_iterator = split_single_iterator<ValueType, iter_t<Iterable>, sentinel_t<Iterable>, T>;
    using iterator = const_iterator;
    using value_type = basic_iterable<iterator>;

#ifdef LZ_HAS_CONCEPTS

    constexpr split_iterable()
        requires(std::default_initializable<maybe_owned<Iterable>> && std::default_initializable<T>)
    = default;

#else

    template<class I = decltype(_iterable),
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<T>::value>>
    constexpr split_iterable() noexcept(std::is_nothrow_default_constructible<maybe_owned<Iterable>>::value &&
                                        std::is_nothrow_default_constructible<T>::value) {
    }

#endif

    template<class I>
    constexpr split_iterable(I&& iterable, T delimiter) :
        _iterable{ std::forward<I>(iterable) },
        _delimiter{ std::move(delimiter) } {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { _iterable.begin(), _iterable.end(), _delimiter };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return { detail::begin(std::move(_iterable)), detail::end(std::move(_iterable)), std::move(_delimiter) };
    }

    LZ_NODISCARD constexpr default_sentinel_t end() const noexcept {
        return {};
    }
};

} // namespace detail
} // namespace lz

#endif // LZ_SPLIT_ITERABLE_HPP
