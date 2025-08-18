#pragma once

#ifndef LZ_INCLUSIVE_SCAN_ITERABLE_HPP
#define LZ_INCLUSIVE_SCAN_ITERABLE_HPP

#include <Lz/detail/func_container.hpp>
#include <Lz/detail/iterators/inclusive_scan.hpp>
#include <Lz/detail/maybe_owned.hpp>

namespace lz {
namespace detail {

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
        requires(std::default_initializable<maybe_owned<Iterable>> && std::default_initializable<T> &&
                 std::default_initializable<BinaryOp>)
    = default;

#else

    template<class I = decltype(_iterable),
             class = enable_if_t<std::is_default_constructible<I>::value && std::is_default_constructible<T>::value &&
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

#ifdef LZ_HAS_CONCEPTS

    LZ_NODISCARD constexpr size_t size() const
        requires(sized<Iterable>)
    {
        return static_cast<size_t>(lz::size(_iterable));
    }

#else

    template<class I = Iterable>
    LZ_NODISCARD constexpr enable_if_t<is_sized<I>::value, size_t> size() const {
        return static_cast<size_t>(lz::size(_iterable));
    }

#endif

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        auto begin = _iterable.begin();
        auto end = _iterable.end();

        if (begin != end) {
            auto new_init = _binary_op(_init, *begin);
            return { begin, end, new_init, _binary_op };
        }
        return { _iterable.begin(), _iterable.end(), _init, _binary_op };
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
