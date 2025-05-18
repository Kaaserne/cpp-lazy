#pragma once

#ifndef LZ_ROTATE_ITERABLE_HPP
#define LZ_ROTATE_ITERABLE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/iterators/rotate.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {
template<class Iterable>
class rotate_iterable : public lazy_view {
    using inner_iter = iter_t<Iterable>;

    ref_or_view<Iterable> _iterable;
    inner_iter _start;

    static constexpr bool is_sized = sized<Iterable>::value;

public:
    using iterator = rotate_iterator<iter_t<Iterable>, sentinel_t<Iterable>, is_sized>;
    using sentinel = typename iterator::sentinel;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr rotate_iterable() = default;

    template<class I>
    constexpr rotate_iterable(I&& iterable, const diff_iterable_t<Iterable> start) :
        _iterable{ std::forward<I>(iterable) },
        _start{ next_fast(_iterable, start) } {
    }

    template<class I = Iterable>
    LZ_NODISCARD constexpr enable_if<sized<I>::value, std::size_t> size() const {
        return static_cast<std::size_t>(lz::size(_iterable));
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { std::begin(_iterable), std::end(_iterable), _start, 0 };
    }

    template<bool Sized = is_sized>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<Sized, iterator> begin() && {
        return static_cast<const rotate_iterable&>(*this).begin();
    }

    template<bool Sized = is_sized>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!Sized, iterator> begin() && {
        return { detail::begin(std::move(_iterable)), detail::end(std::move(_iterable)), std::move(_start), 0 };
    }

    template<bool Sized = is_sized>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<Sized, iterator> end() const {
        return { std::begin(_iterable), std::end(_iterable), _start, size() };
    }

    template<bool Sized = is_sized>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!Sized, inner_iter> end() const {
        return _start;
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_ROTATE_ITERABLE_HPP
