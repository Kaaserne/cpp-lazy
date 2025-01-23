#pragma once

#ifndef LZ_MAP_ITERABLE
#define LZ_MAP_ITERABLE

#include <Lz/detail/iterators/map.hpp>

namespace lz {
namespace detail {
template<class Iterable, class UnaryOp>
class map_iterable {
    iterable_ref<Iterable> _iterable;
    UnaryOp _unary_op;

public:
    using iterator = map_iterator<iter_t<Iterable>, sentinel_t<Iterable>, UnaryOp>;
    using sentinel = typename iterator::sentinel;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    template<class I>
    constexpr map_iterable(I&& iterable, UnaryOp unary_op) :
        _iterable{ iterable },
        _unary_op{ std::move(unary_op) } {
    }

    constexpr map_iterable() = default;

    template<class I = Iterable>
    LZ_NODISCARD constexpr enable_if<sized<I>::value, std::size_t> size() const noexcept {
        return static_cast<std::size_t>(_iterable.get().size());
    }

    // Iterator is bidirectional or higher, meaning an actual iterator is returned in end().
    // We cannot move _unary_op here, but can move inner begin()
    template<class I = iterator>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi<I>::value, iterator> begin() && {
        return { std::move(_iterable.get()).begin(), _unary_op };
    }

    // Iterator is not bidirectional, meaning it is forward or lower. We can safely move the function
    // because a sentinel is returned, which does not rely on any state of the iterable
    template<class I = iterator>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi<I>::value, iterator> begin() && {
        return { std::move(_iterable.get()).begin(), std::move(_unary_op) };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { std::begin(_iterable.get()), _unary_op };
    }

    template<class I = iterator>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi<I>::value, I> end() && {
        return { std::move(_iterable.get()).end(), _unary_op };
    }

    template<class I = iterator>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi<I>::value, I> end() const& {
        return { std::end(_iterable.get()), _unary_op };
    }

    template<class I = iterator>
    LZ_NODISCARD constexpr enable_if<!is_bidi<I>::value, sentinel> end() const& {
        return {};
    }
};
} // namespace detail
} // namespace lz
#endif