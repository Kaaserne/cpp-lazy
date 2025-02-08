#pragma once

#ifndef LZ_TAKE_ITERABLE_HPP
#define LZ_TAKE_ITERABLE_HPP

#include <Lz/detail/iterators/take.hpp>

namespace lz {
namespace detail {
template<class Iterable>
class take_iterable : public lazy_view {
    ref_or_view<Iterable> _iterable;
    diff_iterable_t<Iterable> _n;

public:
    using iterator = take_iterator<iter_t<Iterable>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    template<class I>
    constexpr take_iterable(I&& iterable, typename iterator::difference_type n) noexcept :
        _iterable{ std::forward<I>(iterable) },
        _n{ n } {
    }

    constexpr take_iterable() = default;

    template<class I = Iterable>
    LZ_NODISCARD constexpr enable_if<sized<I>::value, std::size_t> size() const {
        return static_cast<std::size_t>(_n);
    }

    template<class I = iter_t<Iterable>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 enable_if<!is_ra<I>::value, iterator> begin() && {
        return { detail::begin(std::move(_iterable)), _n };
    }

    template<class I = iter_t<Iterable>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 enable_if<is_ra<I>::value, iterator> begin() && {
        return static_cast<const take_iterable&>(*this).begin();
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 iterator begin() const& {
        return { std::begin(_iterable), _n };
    }

    template<class I = iter_t<Iterable>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 enable_if<is_ra<I>::value, iterator> end() const& {
        return { std::begin(_iterable) + _n, 0 };
    }

    template<class I = iter_t<Iterable>>
    LZ_NODISCARD constexpr enable_if<!is_ra<I>::value, default_sentinel> end() const& noexcept {
        return {};
    }
};
} // namespace detail
} // namespace lz
#endif