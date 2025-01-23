#pragma once

#ifndef LZ_CHUNKS_ITERABLE_HPP
#define LZ_CHUNKS_ITERABLE_HPP

#include <Lz/detail/iterators/chunks.hpp>

namespace lz {
namespace detail {
template<class Iterable>
class chunks_iterable {
    iterable_ref<Iterable> _iterable;
    std::size_t _chunk_size;

public:
    using iterator = chunks_iterator<iter_t<Iterable>, sentinel_t<Iterable>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;
    using sentinel = typename iterator::sentinel;

    constexpr chunks_iterable() = default;

    template<class I>
    LZ_CONSTEXPR_CXX_14 chunks_iterable(I&& iterable, const std::size_t chunk_size) :
        _iterable{ iterable },
        _chunk_size{ chunk_size } {
    }

    template<class I = Iterable>
    LZ_NODISCARD enable_if<sized<I>::value, std::size_t> size() const {
        return static_cast<std::size_t>(std::ceil(static_cast<double>(_iterable.get().size()) / _chunk_size));
    }

    template<class I = iterator>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi<I>::value, I> begin() const {
        return { std::begin(_iterable.get()), std::end(_iterable.get()), _chunk_size };
    }

    template<class I = iterator>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi<I>::value, I> begin() const {
        return { std::begin(_iterable.get()), std::begin(_iterable.get()), std::end(_iterable.get()), _chunk_size };
    }

    template<class I = iterator>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi<I>::value, I> end() const {
        return { std::end(_iterable.get()), std::begin(_iterable.get()), std::end(_iterable.get()), _chunk_size };
    }

    template<class I = iterator>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi<I>::value, sentinel> end() const {
        return {};
    }
};
} // namespace detail
} // namespace lz

#endif