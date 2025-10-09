#pragma once

#ifndef LZ_ALGORTHM_CONTAINS_HPP
#define LZ_ALGORTHM_CONTAINS_HPP

#include <Lz/algorithm/find.hpp>

LZ_MODULE_EXPORT namespace lz {

template<class Iterator, class S, class T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool contains(Iterator begin, S end, const T& value) {
    return lz::find(std::move(begin), std::move(end), value) != end;
}

/**
 * @brief Checks if the range [begin(iterable), end(iterable)) contains the value @p value
 *
 * @param iterable The iterable to check
 * @param value The value to check for
 * @return `true` if the value is found, `false` otherwise
 */
template<class Iterable, class T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool contains(Iterable&& iterable, const T& value) {
    return lz::contains(detail::begin(iterable), detail::end(iterable), value);
}

} // namespace lz

#endif
