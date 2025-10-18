#pragma once

#ifndef LZ_ALGORITHM_MAX_ELEMENT_HPP
#define LZ_ALGORITHM_MAX_ELEMENT_HPP

#include <Lz/detail/algorithm/max_element.hpp>
#include <Lz/detail/procs/operators.hpp>
#include <Lz/detail/traits/enable_if.hpp>
#include <Lz/detail/traits/is_iterable.hpp>
#include <Lz/detail/traits/is_sentinel.hpp>

LZ_MODULE_EXPORT namespace lz {

#ifdef LZ_HAS_CXX_17

template<class Iterator, class S, class BinaryPredicate = detail::less>
[[nodiscard]] constexpr Iterator max_element(Iterator begin, S end, BinaryPredicate binary_predicate = {}) {
    if constexpr (detail::is_sentinel_v<Iterator, S>) {
        return detail::algorithm::max_element(std::move(begin), std::move(end), std::move(binary_predicate));
    }
    else {
        return std::max_element(std::move(begin), std::move(end), std::move(binary_predicate));
    }
}

#else

template<class Iterator, class S, class BinaryPredicate = detail::less>
LZ_CONSTEXPR_CXX_14
    detail::enable_if_t<detail::is_sentinel<Iterator, S>::value && !detail::is_iterable<Iterator>::value, Iterator>
    max_element(Iterator begin, S end, BinaryPredicate binary_predicate = {}) {
    return detail::algorithm::max_element(std::move(begin), std::move(end), std::move(binary_predicate));
}

template<class Iterator, class S, class BinaryPredicate = detail::less>
LZ_CONSTEXPR_CXX_14
    detail::enable_if_t<!detail::is_sentinel<Iterator, S>::value && !detail::is_iterable<Iterator>::value, Iterator>
    max_element(Iterator begin, S end, BinaryPredicate binary_predicate = {}) {
    return std::max_element(std::move(begin), std::move(end), std::move(binary_predicate));
}

#endif

/**
 * @brief Finds the maximum element in the range [begin, end) using the binary binary_predicate @p binary_predicate
 * @param iterable The iterable to find the maximum element in
 * @param binary_predicate The binary operator to find the maximum element with
 * @return The maximum element in the range, if the range is empty, the return value is `end`
 */
template<class Iterable, class BinaryPredicate = detail::less>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::enable_if_t<detail::is_iterable<Iterable>::value, iter_t<Iterable>>
max_element(Iterable&& iterable, BinaryPredicate binary_predicate = {}) {
    return lz::max_element(detail::begin(iterable), detail::end(iterable), std::move(binary_predicate));
}
} // namespace lz

#endif
