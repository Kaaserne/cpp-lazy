#pragma once

#ifndef LZ_DROP_ITERABLE_HPP
#define LZ_DROP_ITERABLE_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/ref_or_view.hpp>
#include <Lz/detail/traits.hpp>

namespace lz {
namespace detail {
template<class Iterable>
class drop_iterable : public lazy_view {
    ref_or_view<Iterable> _iterable;
    diff_iterable_t<Iterable> _n;

public:
    using iterator = iter_t<Iterable>;
    using sentinel = sentinel_t<Iterable>;
    using const_iterator = iterator;
    using value_type = val_iterable_t<Iterable>;

    constexpr drop_iterable() = default;

    template<class I>
    constexpr drop_iterable(I&& iterable, diff_iterable_t<Iterable> n) : _iterable{ std::forward<I>(iterable) }, _n{ n } {
    }

    template<class I = Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<sized<I>::value, std::size_t> size() const {
        return lz::size(_iterable) - static_cast<std::size_t>(_n);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return std::next(detail::begin(std::move(_iterable)), _n);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return std::next(std::begin(_iterable), _n);
    }

    LZ_NODISCARD constexpr sentinel end() const& {
        return std::end(_iterable);
    }

    LZ_NODISCARD constexpr sentinel end() && {
        return detail::end(std::move(_iterable));
    }
};
} // namespace detail
} // namespace lz

#endif