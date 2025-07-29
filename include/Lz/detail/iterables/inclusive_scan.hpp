#pragma once

#ifndef LZ_INCLUSIVE_SCAN_ITERABLE_HPP
#define LZ_INCLUSIVE_SCAN_ITERABLE_HPP

#include <Lz/detail/func_container.hpp>
#include <Lz/detail/iterators/inclusive_scan.hpp>
#include <Lz/detail/maybe_owned.hpp>

namespace lz {
namespace detail {
LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterable, class T, class BinaryOp>
class inclusive_scan_iterable : public lazy_view {
    maybe_owned<Iterable> _iterable;
    T _init{};
    func_container<BinaryOp> _binary_op;

public:
    using iterator = inclusive_scan_iterator<iter_t<Iterable>, sentinel_t<Iterable>, T, func_container<BinaryOp>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

#ifdef LZ_HAS_CONCEPTS

    constexpr inclusive_scan_iterable()
        requires std::default_initializable<maybe_owned<Iterable>> && std::default_initializable<T> &&
                     std::default_initializable<BinaryOp>
    = default;

#else

    template<class I = decltype(_iterable),
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<T>::value &&
                               std::is_default_constructible<BinaryOp>::value>>
    constexpr inclusive_scan_iterable() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                                 std::is_nothrow_default_constructible<T>::value &&
                                                 std::is_nothrow_default_constructible<BinaryOp>::value) {
    }

#endif

    template<class I>
    constexpr inclusive_scan_iterable(I&& iterable, T init, BinaryOp binary_op) :
        _iterable{ std::forward<I>(iterable) },
        _init{ std::move(init) },
        _binary_op{ std::move(binary_op) } {
    }

    template<class I = Iterable>
    LZ_NODISCARD constexpr enable_if<sized<I>::value, std::size_t> size() const {
        return static_cast<std::size_t>(lz::size(_iterable));
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        auto begin = std::begin(_iterable);
        auto end = std::end(_iterable);

        if (begin != end) {
            auto new_init = _binary_op(_init, *begin);
            return { begin, end, new_init, _binary_op };
        }
        return { std::begin(_iterable), std::end(_iterable), _init, _binary_op };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        auto begin = detail::begin(std::move(_iterable));
        auto end = detail::end(std::move(_iterable));

        if (begin != end) {
            return { begin, end, _binary_op(std::move(_init), *begin), std::move(_binary_op) };
        }
        return { begin, end, std::move(_init), std::move(_binary_op) };
    }

    LZ_NODISCARD constexpr default_sentinel_t end() const noexcept {
        return {};
    }
};

} // namespace detail
} // namespace lz

#endif // LZ_INCLUSIVE_SCAN_ITERABLE_HPP
