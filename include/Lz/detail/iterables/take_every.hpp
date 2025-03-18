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

    ref_or_view<Iterable> _iterable;
    std::size_t _offset;
    std::size_t _start;

    template<class I = inner_iter>
    LZ_CONSTEXPR_CXX_14 enable_if<is_ra<I>::value, inner_iter> get_begin() const {
        if (_start >= static_cast<std::size_t>(std::end(_iterable) - std::begin(_iterable))) {
            return std::end(_iterable);
        }
        return std::next(std::begin(_iterable), _start);
    }

public:
    using iterator = take_every_iterator<inner_iter, sentinel_t<Iterable>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr take_every_iterable() = default;

    template<class I>
    constexpr take_every_iterable(I&& iterable, const std::size_t offset, const std::size_t start) :
        _iterable{ std::forward<I>(iterable) },
        _offset{ offset },
        _start{ start } {
    }

    template<class I = Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<sized<I>::value, std::size_t> size() const {
        const auto cur_size = static_cast<std::size_t>(lz::size(_iterable));
        if (cur_size == 0) {
            return 0;
        }
        const auto actual_size = std::max(std::ptrdiff_t{ 1 }, static_cast<std::ptrdiff_t>(cur_size - _start));
        const auto div_rem = std::lldiv(actual_size, static_cast<std::ptrdiff_t>(_offset));
        return static_cast<std::size_t>(div_rem.quot) + static_cast<std::size_t>(div_rem.rem == 0 ? 0 : 1);
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, iterator> begin() const& {
        auto begin = std::begin(_iterable);
        for (std::size_t i = 0; i < _start && begin != std::end(_iterable); ++i) {
            ++begin;
        }
        return { begin, std::end(_iterable), _offset };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, iterator> begin() && {
        auto begin = detail::begin(std::move(_iterable));
        for (std::size_t i = 0; i < _start && begin != std::end(_iterable); ++i) {
            ++begin;
        }
        return { begin, detail::end(std::move(_iterable)), _offset };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value && !is_ra_tag<I>::value, iterator> begin() const& {
        auto begin = std::begin(_iterable);
        for (std::size_t i = 0; i < _start && begin != std::end(_iterable); ++i) {
            ++begin;
        }
        return { begin, std::end(_iterable), _offset, _start };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value && !is_ra_tag<I>::value, iterator> end() const {
        return { std::end(_iterable), std::end(_iterable), _offset, lz::eager_size(_iterable) - _start };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_ra_tag<I>::value, iterator> begin() const& {
        auto start_pos = get_begin();
        return { start_pos, start_pos, std::end(_iterable), _offset };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_ra_tag<I>::value, iterator> end() const {
        auto start_pos = get_begin();
        return { std::end(_iterable), start_pos, std::end(_iterable), _offset };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, default_sentinel> end() const {
        return {};
    }
};

} // namespace detail
} // namespace lz

#endif