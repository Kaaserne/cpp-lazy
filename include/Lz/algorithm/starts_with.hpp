#pragma once

#ifndef LZ_ALGORITHM_STARTS_WITH_HPP
#define LZ_ALGORITHM_STARTS_WITH_HPP

#include <Lz/algorithm/equal.hpp>
#include <Lz/detail/algorithm/starts_with.hpp>
#include <Lz/detail/procs/operators.hpp>
#include <Lz/detail/traits/std_algo_compat.hpp>

LZ_MODULE_EXPORT namespace lz {

#ifdef LZ_HAS_CXX_17

/**
 * @brief Checks if the beginning of the first iterable matches the second iterable, using an optional binary predicate for
 * comparison.
 *
 * @param iterable The first iterable to check.
 * @param iterable2 The second iterable to compare against.
 * @param binary_predicate A binary predicate that takes two elements (one from each iterable) and returns true if they are
 * considered equal. Defaults to `std::equal_to<>`.
 * @return true if the beginning of the first iterable matches the second iterable, false otherwise.
 */
template<class Iterable, class Iterable2, class BinaryPredicate = LZ_BIN_OP(equal_to, val_iterable_t<Iterable>)>
[[nodiscard]] constexpr bool starts_with(Iterable && iterable, Iterable2 && iterable2, BinaryPredicate binary_predicate = {}) {
    if constexpr (is_sized_v<Iterable> && is_sized_v<Iterable2>) {
        if (lz::size(iterable2) > lz::size(iterable)) {
            return false;
        }
    }
    return detail::starts_with(detail::begin(iterable), detail::end(iterable), detail::begin(iterable2), detail::end(iterable2),
                               std::move(binary_predicate));
}

#else

/**
 * @brief Checks if the beginning of the first iterable matches the second iterable, using an optional binary predicate for
 * comparison.
 *
 * @param iterable The first iterable to check.
 * @param iterable2 The second iterable to compare against.
 * @param binary_predicate A binary predicate that takes two elements (one from each iterable) and returns true if they are
 * considered equal. Defaults to `std::equal_to<>`.
 * @return true if the beginning of the first iterable matches the second iterable, false otherwise.
 */
template<class Iterable, class Iterable2, class BinaryPredicate = LZ_BIN_OP(equal_to, detail::val_iterable_t<Iterable>)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::enable_if_t<is_sized<Iterable>::value && is_sized<Iterable2>::value, bool> starts_with(
    Iterable && iterable, Iterable2 && iterable2, BinaryPredicate binary_predicate = {}) {
    if (lz::size(iterable2) > lz::size(iterable)) {
        return false;
    }
    return detail::starts_with(detail::begin(iterable), detail::end(iterable), detail::begin(iterable2), detail::end(iterable2),
                               std::move(binary_predicate));
}

/**
 * @brief Checks if the beginning of the first iterable matches the second iterable, using an optional binary predicate for
 * comparison.
 *
 * @param iterable The first iterable to check.
 * @param iterable2 The second iterable to compare against.
 * @param binary_predicate A binary predicate that takes two elements (one from each iterable) and returns true if they are
 * considered equal. Defaults to `std::equal_to<>`.
 * @return true if the beginning of the first iterable matches the second iterable, false otherwise.
 */
template<class Iterable, class Iterable2, class BinaryPredicate = LZ_BIN_OP(equal_to, detail::val_iterable_t<Iterable>)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::enable_if_t<!is_sized<Iterable>::value || !is_sized<Iterable2>::value, bool> starts_with(
    Iterable && iterable, Iterable2 && iterable2, BinaryPredicate binary_predicate = {}) {
    return detail::starts_with(detail::begin(iterable), detail::end(iterable), detail::begin(iterable2), detail::end(iterable2),
                               std::move(binary_predicate));
}

#endif

} // namespace lz

#endif
