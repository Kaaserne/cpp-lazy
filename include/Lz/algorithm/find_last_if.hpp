#pragma once

#ifndef LZ_ALGORITHM_FIND_LAST_IF_HPP
#define LZ_ALGORITHM_FIND_LAST_IF_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/algorithm/find_last_if.hpp>

LZ_MODULE_EXPORT namespace lz {

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 Iterator find_last_if(Iterator begin, S end, UnaryPredicate&& unary_predicate) {
    return find_last_if(lz::make_basic_iterable(std::move(begin), std::move(end)), std::forward<UnaryPredicate>(unary_predicate));
}

/**
 * @brief Finds the last element in the range [begin(iterable), end(iterable)) that satisfies the unary_predicate @p
 * unary_predicate. Does not use `lz::cached_reverse` to reverse the elements (if bidirectional).
 * @param iterable The iterable to find the element in
 * @param unary_predicate The unary_predicate to find the element with
 * @return An iterator to the first element that satisfies the unary_predicate or `end(iterable)` if the element is not found
 */
template<class Iterable, class UnaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iter_t<Iterable> find_last_if(Iterable&& iterable, UnaryPredicate&& unary_predicate) {
    return detail::algorithm::find_last_if(std::forward<Iterable>(iterable), std::forward<UnaryPredicate>(unary_predicate));
}

} // namespace lz

#endif // LZ_ALGORITHM_FIND_LAST_IF_HPP
