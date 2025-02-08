#pragma once

#ifndef LZ_ENUMERATE_ITERABLE_HPP
#define LZ_ENUMERATE_ITERABLE_HPP

#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterators/enumerate.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {
template<LZ_CONCEPT_ITERABLE Iterable, LZ_CONCEPT_INTEGRAL IntType>
class enumerate_iterable : public lazy_view {
    ref_or_view<Iterable> _iterable;
    IntType _start;

public:
    using iterator = enumerate_iterator<iter_t<Iterable>, sentinel_t<Iterable>, IntType, sized<Iterable>::value>;
    using const_iterator = iterator;
    using sentinel = typename iterator::sentinel;
    using value_type = typename iterator::value_type;

    constexpr enumerate_iterable() = default;

    template<class I>
    constexpr enumerate_iterable(I&& iterable, const IntType start = 0) : _iterable{ iterable }, _start{ start } {
    }

    template<class I = Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<sized<I>::value, std::size_t> size() const {
        return lz::size(_iterable);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { std::begin(_iterable), _start };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return { detail::begin(std::move(_iterable)), _start };
    }

    // If iterable is sized, we don't have to return the sentinel because we know how to get the amounts of elements.
    // Of course, this should be the same as end() - begin(). This way, bidirectional iterators may be used
    template<class I = Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<sized<I>::value, iterator> end() const& {
        return { std::end(_iterable), _start + static_cast<IntType>(lz::size(_iterable)) };
    }

    template<class I = Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<sized<I>::value, iterator> end() && {
        return { detail::end(std::move(_iterable)), _start + static_cast<IntType>(lz::size(_iterable)) };
    }

    template<class I = Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!sized<I>::value, sentinel> end() const& {
        return std::end(_iterable);
    }
};
} // namespace detail
} // namespace lz
#endif