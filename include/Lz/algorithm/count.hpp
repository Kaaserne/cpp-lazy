#pragma once

#ifndef LZ_ALGORITHM_COUNT_HPP
#define LZ_ALGORITHM_COUNT_HPP

#include <Lz/algorithm/count_if.hpp>

LZ_MODULE_EXPORT namespace lz {

template<class Iterator, class S, class T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::diff_type<Iterator> count(Iterator begin, S end, const T& value) {
    return lz::count_if(std::move(begin), std::move(end), [&value](const T& val) { return val == value; });
}

/**
 * @brief Counts the amount of elements in the range [begin(iterable), end(iterable)) that are equal to the value @p value
 *
 * @param iterable The iterable to count in
 * @param value The value to count
 * @return The amount of elements that are equal to @p value
 */
template<class Iterable, class T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::diff_iterable_t<Iterable> count(Iterable&& iterable, const T& value) {
    return lz::count(detail::begin(iterable), detail::end(iterable), value);
}

} // namespace lz

#endif // LZ_ALGORITHM_COUNT_HPP
