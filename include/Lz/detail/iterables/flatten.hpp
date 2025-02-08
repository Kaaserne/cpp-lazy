#pragma once

#ifndef LZ_FLATTEN_ITERABLE_HPP
#define LZ_FLATTEN_ITERABLE_HPP

#include <Lz/detail/iterators/flatten.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {
template<class Iterable, int Dims>
class flatten_iterable : public lazy_view {
    using inner_iter = iter_t<Iterable>;
    using inner_sentinel = sentinel_t<Iterable>;

    ref_or_view<Iterable> _iterable;

public:
    using iterator = flatten_iterator<inner_iter, inner_sentinel, Dims>;
    using const_iterator = iterator;
    using value_type = typename flatten_iterator<inner_iter, inner_sentinel, 0>::value_type;

    constexpr flatten_iterable() = default;

    template<class I>
    LZ_CONSTEXPR_CXX_14 flatten_iterable(I&& iterable) : _iterable{ std::forward<I>(iterable) } {
    }

    // TODO add size method

    LZ_CONSTEXPR_CXX_14 iterator begin() const {
        return { std::begin(_iterable), std::begin(_iterable), std::end(_iterable) };
    }

    template<class I = inner_iter>
    LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi<I>::value, iterator> begin() && {
        return { std::begin(_iterable), std::begin(_iterable), detail::end(std::move(_iterable)) };
    }

    template<class I = inner_iter>
    LZ_NODISCARD constexpr enable_if<is_bidi<I>::value, iterator> end() const noexcept {
        return { std::end(_iterable), std::begin(_iterable), std::end(_iterable) };
    }

    template<class I = inner_iter>
    LZ_NODISCARD constexpr enable_if<!is_bidi<I>::value, default_sentinel> end() const noexcept {
        return {};
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_FLATTEN_ITERABLE_HPP
