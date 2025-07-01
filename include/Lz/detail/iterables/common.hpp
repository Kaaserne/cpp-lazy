#pragma once

#ifndef LZ_COMMON_ITERABLE_HPP
#define LZ_COMMON_ITERABLE_HPP

#include <Lz/detail/iterators/common.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {

template<class Iterable>
class common_iterable : public lazy_view {
    ref_or_view<Iterable> _iterable;

public:
    using iterator = common_iterator<iter_t<Iterable>, sentinel_t<Iterable>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

#ifdef LZ_HAS_CONCEPTS

    constexpr common_iterable()
        requires std::default_initializable<ref_or_view<Iterable>>
    = default;

#else

    template<class I = decltype(_iterable), class = enable_if<std::is_default_constructible<I>::value>>
    constexpr common_iterable() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    template<class I>
    constexpr common_iterable(I&& iterable) : _iterable{ std::forward<I>(iterable) } {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return { detail::begin(std::move(_iterable)) };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { std::begin(_iterable) };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator end() && {
        return { detail::end(std::move(_iterable)) };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator end() const& {
        return { std::end(_iterable) };
    }
};
} // namespace detail
} // namespace lz

#endif