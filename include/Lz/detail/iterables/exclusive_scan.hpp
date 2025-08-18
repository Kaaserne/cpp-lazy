#pragma once

#ifndef LZ_EXCLUSIVE_SCAN_ITERABLE_HPP
#define LZ_EXCLUSIVE_SCAN_ITERABLE_HPP

#include <Lz/detail/func_container.hpp>
#include <Lz/detail/iterators/exclusive_scan.hpp>
#include <Lz/detail/maybe_owned.hpp>
#include <Lz/detail/traits.hpp>

namespace lz {
namespace detail {
template<class Iterable, class T, class BinaryOp>
class exclusive_scan_iterable : public lazy_view {
    maybe_owned<Iterable> _iterable;
    T _init{};
    func_container<BinaryOp> _binary_op;

public:
    using iterator = exclusive_scan_iterator<iter_t<Iterable>, sentinel_t<Iterable>, T, func_container<BinaryOp>>;
    using const_iterator = iterator;

#ifdef LZ_HAS_CONCEPTS

    constexpr exclusive_scan_iterable()
        requires(std::default_initializable<maybe_owned<Iterable>> && std::default_initializable<T> &&
                 std::default_initializable<BinaryOp>)
    = default;

#else

    template<class I = decltype(_iterable),
             class = enable_if_t<std::is_default_constructible<I>::value && std::is_default_constructible<T>::value &&
                                 std::is_default_constructible<BinaryOp>::value>>
    constexpr exclusive_scan_iterable() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                                 std::is_nothrow_default_constructible<T>::value &&
                                                 std::is_nothrow_default_constructible<BinaryOp>::value) {
    }

#endif

    template<class I>
    constexpr exclusive_scan_iterable(I&& iterable, T init, BinaryOp binary_op) :
        _iterable{ std::forward<I>(iterable) },
        _init{ std::move(init) },
        _binary_op{ std::move(binary_op) } {
    }

#ifdef LZ_HAS_CONCEPTS

    [[nodiscard]] constexpr size_t size() const
        requires(sized<Iterable>)
    {
        const auto size = static_cast<size_t>(lz::size(_iterable));
        return size == 0 ? 0 : size + 1;
    }

#else

    template<class I = Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<is_sized<I>::value, size_t> size() const {
        const auto size = static_cast<size_t>(lz::size(_iterable));
        return size == 0 ? 0 : size + 1;
    }

#endif

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { _iterable.begin(), _iterable.end(), _init, _binary_op };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return { detail::begin(std::move(_iterable)), detail::end(std::move(_iterable)), std::move(_init),
                 std::move(_binary_op) };
    }

    LZ_NODISCARD constexpr default_sentinel_t end() const noexcept {
        return {};
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_EXCLUSIVE_SCAN_ITERABLE_HPP
