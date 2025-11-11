#pragma once

#ifndef LZ_ALGORTHM_CONTAINS_HPP
#define LZ_ALGORTHM_CONTAINS_HPP

#include <Lz/algorithm/find.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Checks if the range [begin(iterable), end(iterable)) contains the value @p value
 *
 * @param iterable The iterable to check
 * @param value The value to check for
 * @return `true` if the value is found, `false` otherwise
 */
template<class Iterable, class T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool contains(Iterable&& iterable, const T& value) {
    return lz::find(std::forward<Iterable>(iterable), value) != detail::end(iterable);
}

} // namespace lz

#endif
