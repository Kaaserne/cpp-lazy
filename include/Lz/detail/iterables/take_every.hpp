#pragma once

#ifndef LZ_TAKE_EVERY_ITERABLE_HPP
#define LZ_TAKE_EVERY_ITERABLE_HPP

#include <Lz/detail/iterators/take_every.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {
template<class Iterable>
class take_every_iterable : public lazy_view {
    using inner_iter = iter_t<Iterable>;
    using difference_type = diff_type<inner_iter>;

    ref_or_view<Iterable> _iterable;
    difference_type _offset;

public:
    using iterator = take_every_iterator<inner_iter, sentinel_t<Iterable>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr take_every_iterable() = default;

    template<class I>
    constexpr take_every_iterable(I&& iterable, const difference_type offset) :
        _iterable{ std::forward<I>(iterable) },
        _offset{ offset } {
    }

    template<class I = inner_iter>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi<I>::value, iterator> begin() const& {
        return { std::begin(_iterable), std::begin(_iterable), std::end(_iterable), _offset };
    }

    template<class I = inner_iter>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi<I>::value, iterator> begin() const& {
        return { std::begin(_iterable), std::end(_iterable), _offset };
    }

    template<class I = inner_iter>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi<I>::value, iterator> begin() && {
        return { detail::begin(std::forward<Iterable>(_iterable)), detail::end(std::forward<Iterable>(_iterable)), _offset };
    }

    template<class I = inner_iter>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi<I>::value, iterator> end() const {
        return { std::end(_iterable), std::begin(_iterable), std::end(_iterable), _offset };
    }

    template<class I = inner_iter>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi<I>::value, default_sentinel> end() const {
        return {};
    }
};

} // namespace detail
} // namespace lz

#endif