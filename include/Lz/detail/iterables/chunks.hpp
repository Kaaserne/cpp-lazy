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
    std::size_t _chunk_size{};

public:
    using iterator = chunks_iterator<iter_t<Iterable>, sentinel_t<Iterable>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;
    using sentinel = typename iterator::sentinel;

#ifdef LZ_HAS_CONCEPTS

    constexpr chunks_iterable()
        requires std::default_initializable<Iterable>
    = default;

#else

    template<class I = decltype(_iterable), class = enable_if<std::is_default_constructible<I>::value>>
    constexpr chunks_iterable() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    template<class I>
    constexpr chunks_iterable(I&& iterable, const std::size_t chunk_size) :
        _iterable{ std::forward<I>(iterable) },
        _chunk_size{ chunk_size } {
    }

    template<class I = Iterable>
    LZ_NODISCARD constexpr enable_if<sized<I>::value, std::size_t> size() const {
        return static_cast<std::size_t>(lz::size(_iterable) + (_chunk_size - 1)) / _chunk_size;
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr iterator begin() const& {
        if constexpr (is_ra_tag<typename iterator::iterator_category>::value) {
            return iterator{ std::begin(_iterable), std::begin(_iterable), std::end(_iterable), _chunk_size };
        }
        else if constexpr (is_bidi_tag<typename iterator::iterator_category>::value) {
            return iterator{ std::begin(_iterable), std::end(_iterable), _chunk_size, 0 };
        }
        else {
            return iterator{ std::begin(_iterable), std::end(_iterable), _chunk_size };
        }
    }

#else

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_fwd_tag<I>::value && !is_bidi_tag<I>::value, iterator> begin() const& {
        return { std::begin(_iterable), std::end(_iterable), _chunk_size };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value && !is_ra_tag<I>::value, iterator> begin() const& {
        return { std::begin(_iterable), std::end(_iterable), _chunk_size, 0 };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_ra_tag<I>::value, iterator> begin() const& {
        return { std::begin(_iterable), std::begin(_iterable), std::end(_iterable), _chunk_size };
    }

#endif

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_fwd_tag<I>::value && !is_bidi_tag<I>::value, iterator> begin() && {
        return { detail::begin(std::forward<Iterable>(_iterable)), detail::end(std::forward<Iterable>(_iterable)), _chunk_size };
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const& {
        if constexpr (is_ra_tag<typename iterator::iterator_category>::value) {
            return iterator{ std::end(_iterable), std::begin(_iterable), std::end(_iterable), _chunk_size };
        }
        else if constexpr (is_bidi_tag<typename iterator::iterator_category>::value) {
            return iterator{ std::end(_iterable), std::end(_iterable), _chunk_size, lz::eager_size(_iterable) };
        }
        else {
            return default_sentinel{};
        }
    }

#else

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_ra_tag<I>::value, iterator> end() const& {
        return { std::end(_iterable), std::begin(_iterable), std::end(_iterable), _chunk_size };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value && !is_ra_tag<I>::value, iterator> end() const& {
        return { std::end(_iterable), std::end(_iterable), _chunk_size, lz::eager_size(_iterable) };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD constexpr enable_if<is_fwd_tag<I>::value && !is_bidi_tag<I>::value, default_sentinel> end() const& {
        return {};
    }

#endif
};
} // namespace detail
} // namespace lz

#endif