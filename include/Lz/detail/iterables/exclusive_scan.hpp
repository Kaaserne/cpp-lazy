#pragma once

#ifndef LZ_EXCLUSIVE_SCAN_ITERABLE_HPP
#define LZ_EXCLUSIVE_SCAN_ITERABLE_HPP

#include <Lz/detail/iterators/exclusive_scan.hpp>
#include <Lz/detail/ref_or_view.hpp>
#include <Lz/detail/traits.hpp>

namespace lz {
namespace detail {
template<class Iterable, class T, class BinaryOp>
class exclusive_scan_iterable : public lazy_view {
    ref_or_view<Iterable> _iterable;
    T _init;
    BinaryOp _binary_op;

public:
    using iterator = exclusive_scan_iterator<iter_t<Iterable>, sentinel_t<Iterable>, T, BinaryOp>;
    using const_iterator = iterator;

    constexpr exclusive_scan_iterable() = default;

    template<class I>
    constexpr exclusive_scan_iterable(I&& iterable, T init, BinaryOp binary_op) :
        _iterable{ std::forward<I>(iterable) },
        _init{ std::move(init) },
        _binary_op{ std::move(binary_op) } {
    }

    template<class I = Iterable>
    LZ_NODISCARD constexpr enable_if<sized<I>::value, std::size_t> size() const noexcept {
        const auto size = static_cast<std::size_t>(lz::size(_iterable));
        return size == 0 ? 0 : size + 1;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { std::begin(_iterable), std::end(_iterable), _init, _binary_op };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return { detail::begin(std::move(_iterable)), detail::end(std::move(_iterable)), std::move(_init),
                 std::move(_binary_op) };
    }

    LZ_NODISCARD constexpr default_sentinel end() const noexcept {
        return {};
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_EXCLUSIVE_SCAN_ITERABLE_HPP
