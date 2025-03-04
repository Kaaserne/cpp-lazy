#pragma once

#ifndef LZ_TAKE_EVERY_ITERABLE_HPP
#define LZ_TAKE_EVERY_ITERABLE_HPP

#include <Lz/detail/iterators/take_every.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {
template<class Iterable, class = void>
class take_every_iterable;

template<class Iterable>
class take_every_iterable<Iterable, enable_if<!is_ra<iter_t<Iterable>>::value>> : public lazy_view {
    using inner_iter = iter_t<Iterable>;
    using difference_type = diff_type<inner_iter>;

    ref_or_view<Iterable> _iterable;
    iter_t<Iterable> _begin;
    difference_type _offset;
    difference_type _start;

public:
    using iterator = take_every_iterator<inner_iter, sentinel_t<Iterable>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr take_every_iterable() = default;

    template<class I>
    constexpr take_every_iterable(I&& iterable, const difference_type offset, difference_type start) :
        _iterable{ std::forward<I>(iterable) },
        _begin{ std::begin(_iterable) },
        _offset{ offset },
        _start{ start } {
        while (_begin != std::end(_iterable) && start > 0) {
            ++_begin;
            --start;
        }
    }

    template<class I = Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<sized<I>::value, std::size_t> size() const {
        const auto cur_size = lz::size(_iterable);
        if (cur_size == 0) {
            return 0;
        }
        const auto actual_size = std::max(std::ptrdiff_t{ 1 }, static_cast<std::ptrdiff_t>(cur_size - _start));
        const auto div_rem = std::lldiv(actual_size, _offset);
        return static_cast<std::size_t>(div_rem.quot) + (div_rem.rem == 0 ? 0 : 1);
    }

    template<class I = inner_iter>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi<I>::value, iterator> begin() const& {
        return { _begin, _begin, std::end(_iterable), _offset };
    }

    template<class I = inner_iter>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi<I>::value, iterator> begin() const& {
        return { _begin, std::end(_iterable), _offset };
    }

    template<class I = inner_iter>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi<I>::value, iterator> begin() && {
        return { std::move(_begin), detail::end(std::move(_iterable)), _offset };
    }

    template<class I = inner_iter>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi<I>::value, iterator> end() const {
        return { std::end(_iterable), _begin, std::end(_iterable), _offset };
    }

    template<class I = inner_iter>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi<I>::value, default_sentinel> end() const {
        return {};
    }
};

template<class Iterable>
class take_every_iterable<Iterable, enable_if<is_ra<iter_t<Iterable>>::value>> : public lazy_view {
    using inner_iter = iter_t<Iterable>;
    using difference_type = diff_type<inner_iter>;

    ref_or_view<Iterable> _iterable;
    difference_type _offset;
    difference_type _start;

public:
    using iterator = take_every_iterator<inner_iter, sentinel_t<Iterable>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr take_every_iterable() = default;

    template<class I>
    constexpr take_every_iterable(I&& iterable, const difference_type offset, const difference_type start) :
        _iterable{ std::forward<I>(iterable) },
        _offset{ offset },
        _start{ start } {
    }

    template<class I = Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<sized<I>::value, std::size_t> size() const {
        const auto cur_size = lz::size(_iterable);
        if (cur_size == 0) {
            return 0;
        }
        const auto actual_size = std::max(std::ptrdiff_t{ 1 }, static_cast<std::ptrdiff_t>(cur_size - _start));
        const auto div_rem = std::div(actual_size, _offset);
        return static_cast<std::size_t>(div_rem.quot) + (div_rem.rem == 0 ? 0 : 1);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const {
        if (_start >= std::end(_iterable) - std::begin(_iterable)) {
            return { std::end(_iterable), std::end(_iterable), std::end(_iterable), _offset };
        }
        return { std::next(std::begin(_iterable), _start), std::next(std::begin(_iterable), _start), std::end(_iterable),
                 _offset };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator end() const {
        if (_start >= std::end(_iterable) - std::begin(_iterable)) {
            return { std::end(_iterable), std::end(_iterable), std::end(_iterable), _offset };
        }
        return { std::end(_iterable), std::next(std::begin(_iterable), _start), std::end(_iterable), _offset };
    }
};

} // namespace detail
} // namespace lz

#endif