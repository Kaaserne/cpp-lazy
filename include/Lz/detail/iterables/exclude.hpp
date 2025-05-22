#pragma once

#ifndef LZ_EXCLUDE_ITERABLE_HPP
#define LZ_EXCLUDE_ITERABLE_HPP

#include <Lz/detail/iterators/exclude.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {
template<class Iterable>
class exclude_iterable : public lazy_view {
    ref_or_view<Iterable> _iterable;
    diff_iterable_t<Iterable> _from;
    diff_iterable_t<Iterable> _to;

public:
    using iterator = exclude_iterator<iter_t<Iterable>, sentinel_t<Iterable>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;
    using sentinel = typename iterator::sentinel;

    template<class I>
    constexpr exclude_iterable(I&& iterable, const diff_iterable_t<Iterable> from, const diff_iterable_t<Iterable> to) :
        _iterable{ std::forward<I>(iterable) },
        _from{ from },
        _to{ to } {
    }

    template<class I = Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<sized<I>::value, std::size_t> size() const {
        return static_cast<std::size_t>(lz::size(_iterable) - static_cast<std::size_t>(_to - _from));
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { std::begin(_iterable), std::end(_iterable), _from, _to, 0 };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return { detail::begin(std::move(_iterable)), std::end(_iterable), _from, _to, 0 };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, iterator> end() const {
        return { std::end(_iterable), std::end(_iterable), _from, _to,
                 static_cast<typename iterator::difference_type>(lz::eager_size(_iterable)) };
    }
    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, sentinel> end() const {
        return std::end(_iterable);
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_EXCLUDE_ITERABLE_HPP
