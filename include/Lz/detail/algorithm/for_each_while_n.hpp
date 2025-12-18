#pragma once

#ifndef LZ_DETAIL_ALGORITHM_FOR_EACH_WHILE_N_HPP
#define LZ_DETAIL_ALGORITHM_FOR_EACH_WHILE_N_HPP

#include <Lz/detail/compiler_config.hpp>
#include <Lz/detail/procs/min_max.hpp>
#include <Lz/detail/traits/enable_if.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <Lz/procs/eager_size.hpp>

namespace lz {
namespace detail {

#ifdef LZ_HAS_CXX_17

template<class Iterable, class UnaryOp>
constexpr void for_each_while_n(Iterable&& iterable, size_t n, UnaryOp unary_op) {
    auto it = detail::begin(iterable);

    if constexpr (is_ra_v<iter_t<Iterable>>) {
        using diff_t = diff_iterable_t<Iterable>;
        const auto diff = static_cast<diff_t>(detail::min_variadic2(n, static_cast<size_t>(lz::eager_size(iterable))));
        const auto last = detail::begin(iterable) + diff;

        for (; it != last; ++it) {
            if (!unary_op(*it)) {
                break;
            }
        }
    }
    else if constexpr (is_sized_v<Iterable>) {
        auto count = detail::min_variadic2(n, static_cast<size_t>(lz::eager_size(iterable)));
        for (; count-- != 0; ++it) {
            if (!unary_op(*it)) {
                break;
            }
        }
    }
    else {
        auto begin = detail::begin(iterable);
        const auto end = detail::end(iterable);

        for (size_t count = 0; begin != end && count != n; ++begin, ++count) {
            if (!unary_op(*begin)) {
                break;
            }
        }
    }
}

#else

template<class Iterable, class UnaryOp>
LZ_CONSTEXPR_CXX_14 enable_if_t<is_ra<iter_t<Iterable>>::value>
for_each_while_n(Iterable&& iterable, size_t n, UnaryOp unary_op) {
    using diff_t = diff_iterable_t<Iterable>;

    const auto diff = static_cast<diff_t>(detail::min_variadic2(n, static_cast<size_t>(lz::eager_size(iterable))));
    const auto last = detail::begin(iterable) + diff;

    for (auto it = detail::begin(iterable); it != last; ++it) {
        if (!unary_op(*it)) {
            break;
        }
    }
}

template<class Iterable, class UnaryOp>
LZ_CONSTEXPR_CXX_14 enable_if_t<is_sized<Iterable>::value && !is_ra<iter_t<Iterable>>::value>
for_each_while_n(Iterable&& iterable, size_t n, UnaryOp unary_op) {
    auto count = detail::min_variadic2(n, lz::eager_size(iterable));

    for (auto it = detail::begin(iterable); count-- != 0; ++it) {
        if (!unary_op(*it)) {
            break;
        }
    }
}
template<class Iterable, class UnaryOp>
LZ_CONSTEXPR_CXX_14 enable_if_t<!is_sized<Iterable>::value && !is_ra<iter_t<Iterable>>::value>
for_each_while_n(Iterable&& iterable, size_t n, UnaryOp unary_op) {
    auto begin = detail::begin(iterable);
    const auto end = detail::end(iterable);

    for (size_t count = 0; begin != end && count != n; ++begin, ++count) {
        if (!unary_op(*begin)) {
            break;
        }
    }
}

#endif

} // namespace detail
} // namespace lz

#endif
