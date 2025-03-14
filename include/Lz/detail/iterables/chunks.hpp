#pragma once

#ifndef LZ_CHUNKS_ITERABLE_HPP
#define LZ_CHUNKS_ITERABLE_HPP

#include <Lz/detail/iterators/chunks.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {
template<class Iterable>
class chunks_iterable : public lazy_view {
    ref_or_view<Iterable> _iterable;
    std::size_t _chunk_size;

    using inner_iter = iter_t<Iterable>;

public:
    using iterator = chunks_iterator<inner_iter, sentinel_t<Iterable>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;
    using sentinel = typename iterator::sentinel;

    constexpr chunks_iterable() = default;

    template<class I>
    constexpr chunks_iterable(I&& iterable, const std::size_t chunk_size) :
        _iterable{ std::forward<I>(iterable) },
        _chunk_size{ chunk_size } {
    }

    template<class I = Iterable>
    LZ_NODISCARD enable_if<sized<I>::value, std::size_t> size() const {
        return static_cast<std::size_t>(std::ceil(static_cast<double>(lz::size(_iterable)) / _chunk_size));
    }

    template<class I = inner_iter>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_fwd<I>::value && !is_bidi<I>::value, iterator> begin() const& {
        return { std::begin(_iterable), std::end(_iterable), _chunk_size };
    }

    template<class I = inner_iter>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_fwd<I>::value && !is_bidi<I>::value, iterator> begin() && {
        return { detail::begin(std::forward<Iterable>(_iterable)), detail::end(std::forward<Iterable>(_iterable)), _chunk_size };
    }

    template<class I = inner_iter>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi<I>::value && !is_ra<I>::value, iterator> begin() const& {
        return { std::begin(_iterable), std::end(_iterable), _chunk_size, 0 };
    }

    template<class I = inner_iter>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_ra<I>::value, iterator> begin() const& {
        return { std::begin(_iterable), std::begin(_iterable), std::end(_iterable), _chunk_size };
    }

    template<class I = inner_iter>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_ra<I>::value, iterator> end() const& {
        return { std::end(_iterable), std::begin(_iterable), std::end(_iterable), _chunk_size };
    }

    template<class I = inner_iter>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi<I>::value && !is_ra<I>::value, iterator> end() const& {
        return { std::end(_iterable), std::end(_iterable), _chunk_size, lz::eager_size(_iterable) };
    }

    template<class I = inner_iter>
    LZ_NODISCARD constexpr enable_if<is_fwd<I>::value && !is_bidi<I>::value, default_sentinel> end() const& {
        return {};
    }
};
} // namespace detail
} // namespace lz

#endif