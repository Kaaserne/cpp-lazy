#pragma once

#ifndef LZ_ALGORITHM_FIND_IF_HPP
#define LZ_ALGORITHM_FIND_IF_HPP

#include <Lz/detail/algorithm/find_if.hpp>
#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/traits/is_sentinel.hpp>
#include <Lz/traits/iter_type.hpp>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

#include <algorithm>

LZ_MODULE_EXPORT namespace lz {

#ifdef LZ_HAS_CXX_17

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 Iterator find_if(Iterator begin, S end, UnaryPredicate unary_predicate) {
    if constexpr (detail::is_sentinel_v<Iterator, S>) {
        return detail::algorithm::find_if(std::move(begin), std::move(end), std::move(unary_predicate));
    }
    else {
        return std::find_if(std::move(begin), std::move(end), std::move(unary_predicate));
    }
}

#else

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 detail::enable_if_t<detail::is_sentinel<Iterator, S>::value, Iterator>
find_if(Iterator begin, S end, UnaryPredicate unary_predicate) {
    return detail::algorithm::find_if(std::move(begin), std::move(end), std::move(unary_predicate));
}

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 detail::enable_if_t<!detail::is_sentinel<Iterator, S>::value, Iterator>
find_if(Iterator begin, S end, UnaryPredicate unary_predicate) {
    return std::find_if(std::move(begin), std::move(end), std::move(unary_predicate));
}

#endif

/**
 * @brief Finds the first element in the range [begin, end) that satisfies the binary_predicate @p binary_predicate
 *
 * @param iterable The iterable to find the element in
 * @param unary_predicate The binary_predicate to find the element with
 * @return Iterator to the first element that satisfies the binary_predicate or `end(iterable)` if the element is not found
 */
template<class Iterable, class UnaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iter_t<Iterable> find_if(Iterable&& iterable, UnaryPredicate unary_predicate) {
    return lz::find_if(detail::begin(iterable), detail::end(iterable), std::move(unary_predicate));
}
} // namespace lz

#endif
