#pragma once

#ifndef LZ_ALGORITHM_EQUAL_HPP
#define LZ_ALGORITHM_EQUAL_HPP

#include <Lz/detail/algorithm/equal.hpp>
#include <Lz/detail/procs/operators.hpp>
#include <Lz/detail/traits/enable_if.hpp>
#include <Lz/detail/traits/is_sentinel.hpp>
#include <Lz/procs/size.hpp>
#include <Lz/traits/is_sized.hpp>

LZ_MODULE_EXPORT namespace lz {

#ifdef LZ_HAS_CXX_17

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate = detail::equal_to>
[[nodiscard]] constexpr bool equal(Iterator1 begin1, S1 end1, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate = {}) {
    if constexpr (detail::is_sentinel_v<Iterator1, S1> || detail::is_sentinel_v<Iterator2, S2>) {
        return detail::algorithm::equal(std::move(begin1), std::move(end1), std::move(begin2), std::move(end2),
                                        std::move(binary_predicate));
    }
    else {
        return std::equal(std::move(begin1), std::move(end1), std::move(begin2), std::move(end2), std::move(binary_predicate));
    }
}

/**
 * Use this function to check if two lz iterators are the same.
 * @param a An iterable, its underlying value type should have an operator== with `b`
 * @param b An iterable, its underlying value type should have an operator== with `a`
 * @return true if both are equal, false otherwise.
 */
template<class IterableA, class IterableB, class BinaryPredicate = detail::equal_to>
[[nodiscard]] constexpr bool equal(IterableA&& a, IterableB&& b, BinaryPredicate binary_predicate = {}) {
    if constexpr (is_sized_v<IterableA> && is_sized_v<IterableB>) {
        if (lz::size(a) != lz::size(b)) {
            return false;
        }
    }

    return lz::equal(detail::begin(a), detail::end(a), detail::begin(b), detail::end(b), std::move(binary_predicate));
}

#elif defined(LZ_HAS_CXX_14)

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate = detail::equal_to>
LZ_CONSTEXPR_CXX_14
    detail::enable_if_t<!detail::is_sentinel<Iterator1, S1>::value && !detail::is_sentinel<Iterator2, S2>::value, bool>
    equal(Iterator1 begin1, S1 end1, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate = {}) {
    return std::equal(std::move(begin1), std::move(end1), std::move(begin2), std::move(end2), std::move(binary_predicate));
}

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate = detail::equal_to>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14
    detail::enable_if_t<detail::is_sentinel<Iterator1, S1>::value || detail::is_sentinel<Iterator2, S2>::value, bool>
    equal(Iterator1 begin1, S1 end1, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate = {}) {
    return detail::algorithm::equal(std::move(begin1), std::move(end1), std::move(begin2), std::move(end2),
                                    std::move(binary_predicate));
}

#else // vv cxx 11

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate = detail::equal_to>
LZ_CONSTEXPR_CXX_14
    detail::enable_if_t<!detail::is_sentinel<Iterator1, S1>::value && !detail::is_sentinel<Iterator2, S2>::value, bool>
    equal(Iterator1 begin1, S1 end1, Iterator2 begin2, S2, BinaryPredicate binary_predicate = {}) {
    return std::equal(std::move(begin1), std::move(end1), std::move(begin2), std::move(binary_predicate));
}

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate = detail::equal_to>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14
    detail::enable_if_t<detail::is_sentinel<Iterator1, S1>::value || detail::is_sentinel<Iterator2, S2>::value, bool>
    equal(Iterator1 begin1, S1 end1, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate = {}) {
    return detail::algorithm::equal(std::move(begin1), std::move(end1), std::move(begin2), std::move(end2),
                                    std::move(binary_predicate));
}

#endif

#if !defined(LZ_HAS_CXX_17)

/**
 * Use this function to check if two lz iterators are the same.
 * @param a An iterable, its underlying value type should have an operator== with `b`
 * @param b An iterable, its underlying value type should have an operator== with `a`
 * @param binary_predicate The predicate to check if two elements are equal
 * @return true if both are equal, false otherwise.
 */
template<class IterableA, class IterableB, class BinaryPredicate = detail::equal_to>
LZ_NODISCARD
    LZ_CONSTEXPR_CXX_14 detail::enable_if_t<detail::is_sized<IterableA>::value && detail::is_sized<IterableB>::value, bool>
    equal(IterableA&& a, IterableB&& b, BinaryPredicate binary_predicate = {}) {
    if (lz::size(a) != lz::size(b)) {
        return false;
    }

    return lz::equal(detail::begin(a), detail::end(a), detail::begin(b), detail::end(b), std::move(binary_predicate));
}

/**
 * Use this function to check if two lz iterators are the same.
 * @param a An iterable, its underlying value type should have an operator== with `b`
 * @param b An iterable, its underlying value type should have an operator== with `a`
 * @param binary_predicate The predicate to check if two elements are equal
 * @return true if both are equal, false otherwise.
 */
template<class IterableA, class IterableB, class BinaryPredicate = detail::equal_to>
LZ_NODISCARD
    LZ_CONSTEXPR_CXX_14 detail::enable_if_t<!detail::is_sized<IterableA>::value || !detail::is_sized<IterableB>::value, bool>
    equal(IterableA&& a, IterableB&& b, BinaryPredicate binary_predicate = {}) {
    return lz::equal(detail::begin(a), detail::end(a), detail::begin(b), detail::end(b), std::move(binary_predicate));
}

#endif

} // namespace lz

#endif
