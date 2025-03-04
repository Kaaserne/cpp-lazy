#pragma once

#ifndef LZ_MAP_ITERABLE
#define LZ_MAP_ITERABLE

#include <Lz/detail/func_container.hpp>
#include <Lz/detail/iterators/map.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {
template<class Iterable, class UnaryOp>
class map_iterable : public lazy_view {
    ref_or_view<Iterable> _iterable;
    func_container<UnaryOp> _unary_op;

public:
    using iterator = map_iterator<iter_t<Iterable>, sentinel_t<Iterable>, func_container<UnaryOp>>;
    using sentinel = typename iterator::sentinel;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    template<class I>
    constexpr map_iterable(I&& iterable, UnaryOp unary_op) : _iterable{ iterable }, _unary_op{ std::move(unary_op) } {
    }

    constexpr map_iterable() = default;

    template<class I = Iterable>
    LZ_NODISCARD constexpr enable_if<sized<I>::value, std::size_t> size() const noexcept {
        return static_cast<std::size_t>(lz::size(_iterable));
    }

    template<class I = iterator>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi<I>::value, iterator> begin() && {
        return { detail::begin(std::move(_iterable)), _unary_op };
    }

    template<class I = iterator>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi<I>::value, iterator> begin() && {
        return { detail::begin(std::move(_iterable)), std::move(_unary_op) };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { std::begin(_iterable), _unary_op };
    }

    template<class I = iterator>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi<I>::value, I> end() && {
        return { detail::end(std::move(_iterable)), _unary_op };
    }

    template<class I = iterator>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi<I>::value, I> end() const& {
        return { std::end(_iterable), _unary_op };
    }

    template<class I = iterator>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi<I>::value, sentinel> end() const& {
        return std::end(_iterable);
    }
};
} // namespace detail
} // namespace lz
#endif