#pragma once

#ifndef LZ_ENUMERATE_ITERABLE_HPP
#define LZ_ENUMERATE_ITERABLE_HPP

#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterators/enumerate.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {
template<class Iterable, class IntType>
class enumerate_iterable : public lazy_view {
    ref_or_view<Iterable> _iterable;
    IntType _start;

    IntType get_last_index() const {
        return _start + static_cast<IntType>(lz::eager_size(_iterable));
    }

public:
    using iterator = enumerate_iterator<iter_t<Iterable>, sentinel_t<Iterable>, IntType>;
    using const_iterator = iterator;
    using sentinel = typename iterator::sentinel;
    using value_type = typename iterator::value_type;

    template<class I>
    constexpr enumerate_iterable(I&& iterable, const IntType start = 0) : _iterable{ iterable }, _start{ start } {
    }

    template<class I = Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<sized<I>::value, std::size_t> size() const {
        return static_cast<std::size_t>(lz::size(_iterable));
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { std::begin(_iterable), _start };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return { detail::begin(std::move(_iterable)), _start };
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const& {
        if constexpr (is_bidi_tag<typename iterator::iterator_category>::value) {
            return iterator{ std::end(_iterable), get_last_index() };
        }
        else {
            return std::end(_iterable);
        }
    }

#else

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, sentinel> end() const& {
        return { std::end(_iterable), get_last_index() };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, sentinel> end() const& {
        return std::end(_iterable);
    }

#endif

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, sentinel> end() && {
        return { detail::end(std::move(_iterable)), get_last_index() };
    }
};
} // namespace detail
} // namespace lz
#endif