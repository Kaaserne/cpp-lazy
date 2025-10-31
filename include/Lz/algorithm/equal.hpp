#pragma once

#ifndef LZ_ALGORITHM_EQUAL_HPP
#define LZ_ALGORITHM_EQUAL_HPP

#include <Lz/detail/algorithm/equal.hpp>
#include <Lz/detail/procs/operators.hpp>
#include <Lz/detail/traits/enable_if.hpp>
#include <Lz/detail/traits/is_sentinel.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>
#include <Lz/procs/size.hpp>
#include <Lz/traits/is_sized.hpp>

LZ_MODULE_EXPORT namespace lz {

#ifndef LZ_HAS_CXX_17

/**
 * Use this function to check if two lz iterators are the same.
 * @param a An iterable, its underlying value type should have an operator== with `b`
 * @param b An iterable, its underlying value type should have an operator== with `a`
 * @param binary_predicate The predicate to check if two elements are equal
 * @return true if both are equal, false otherwise.
 */
template<class IterableA, class IterableB, class BinaryPredicate = LZ_BIN_OP(equal_to, detail::val_iterable_t<IterableA>)>
LZ_NODISCARD
    LZ_CONSTEXPR_CXX_14 detail::enable_if_t<detail::is_sized<IterableA>::value && detail::is_sized<IterableB>::value, bool>
    equal(IterableA&& a, IterableB&& b, BinaryPredicate binary_predicate = {}) {
    if (lz::size(a) != lz::size(b)) {
        return false;
    }

    return detail::equal(detail::begin(a), detail::end(a), detail::begin(b), detail::end(b), std::move(binary_predicate));
}

/**
 * Use this function to check if two lz iterators are the same.
 * @param a An iterable, its underlying value type should have an operator== with `b`
 * @param b An iterable, its underlying value type should have an operator== with `a`
 * @param binary_predicate The predicate to check if two elements are equal
 * @return true if both are equal, false otherwise.
 */
template<class IterableA, class IterableB, class BinaryPredicate = LZ_BIN_OP(equal_to, detail::val_iterable_t<IterableA>)>
LZ_NODISCARD
    LZ_CONSTEXPR_CXX_14 detail::enable_if_t<!detail::is_sized<IterableA>::value || !detail::is_sized<IterableB>::value, bool>
    equal(IterableA&& a, IterableB&& b, BinaryPredicate binary_predicate = {}) {
    return detail::equal(detail::begin(a), detail::end(a), detail::begin(b), detail::end(b), std::move(binary_predicate));
}

#else

// TODO if constexpr

#endif
} // namespace lz

#endif
// TODO remove all begin end overloads, also remove algorithm:: namespace
