#pragma once

#ifndef LZ_ALGORITHM_IS_SORTED_HPP
#define LZ_ALGORITHM_IS_SORTED_HPP

#include <Lz/detail/algorithm/is_sorted.hpp>
#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/procs/begin_end.hpp>
#include <Lz/detail/procs/operators.hpp>
#include <Lz/detail/traits/enable_if.hpp>
#include <Lz/detail/traits/is_iterable.hpp>
#include <Lz/detail/traits/is_sentinel.hpp>

LZ_MODULE_EXPORT namespace lz {

template<class Iterator, class S, class BinaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool is_sorted(Iterator begin, S end, BinaryPredicate binary_predicate) {
    if constexpr (detail::is_sentinel_v<Iterator, S>) {
        return detail::algorithm::is_sorted(std::move(begin), std::move(end), std::move(binary_predicate));
    }
    else {
        return std::is_sorted(std::move(begin), std::move(end), std::move(binary_predicate));
    }
}

#ifdef LZ_HAS_CONCEPTS

template<class Iterator, class S>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool is_sorted(Iterator begin, S end)
    requires(!detail::is_iterable<Iterator>::value)
{
    return is_sorted(std::move(begin), std::move(end), detail::less{});
}

/**
 * @brief Checks if the range [begin(iterable), end(iterable)) is sorted using the binary binary_predicate @p
 * binary_predicate
 *
 * @param iterable The iterable to check
 * @param binary_predicate The binary binary_predicate to check the range with
 * @return true if the range is sorted, false otherwise
 */
template<class Iterable, class BinaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool is_sorted(Iterable&& iterable, BinaryPredicate binary_predicate)
    requires(detail::is_iterable<Iterable>::value)
{
    return is_sorted(detail::begin(iterable), detail::end(iterable), std::move(binary_predicate));
}

#else

template<class Iterator, class S>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::enable_if_t<!detail::is_iterable<Iterator>::value, bool>
is_sorted(Iterator begin, S end) {
    return is_sorted(std::move(begin), std::move(end), detail::less{});
}

/**
 * @brief Checks if the range [begin(iterable), end(iterable)) is sorted using the binary binary_predicate @p
 * binary_predicate
 *
 * @param iterable The iterable to check
 * @param binary_predicate The binary binary_predicate to check the range with
 * @return true if the range is sorted, false otherwise
 */
template<class Iterable, class BinaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::enable_if_t<detail::is_iterable<Iterable>::value, bool>
is_sorted(Iterable&& iterable, BinaryPredicate binary_predicate) {
    return is_sorted(detail::begin(iterable), detail::end(iterable), std::move(binary_predicate));
}

#endif

/**
 * @brief Checks if the range [begin(iterable), end(iterable)) is sorted using the default `operator<`
 *
 * @param iterable The iterable to check
 * @return true if the range is sorted, false otherwise
 */
template<class Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool is_sorted(Iterable&& iterable) {
    return is_sorted(std::forward<Iterable>(iterable), detail::less{});
}

} // namespace lz

#endif
