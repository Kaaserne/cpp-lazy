#pragma once

#ifndef LZ_ALGORITHM_COUNT_HPP
#define LZ_ALGORITHM_COUNT_HPP

#include <Lz/detail/algorithm/count.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Counts the amount of elements in the range [begin(iterable), end(iterable)) that are equal to the value @p value
 *
 * @param iterable The iterable to count in
 * @param value The value to count
 * @return The amount of elements that are equal to @p value
 */
template<class Iterable, class T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::diff_iterable_t<Iterable> count(Iterable&& iterable, const T& value) {
    return detail::count(detail::begin(iterable), detail::end(iterable), value);
}

} // namespace lz

#endif // LZ_ALGORITHM_COUNT_HPP
