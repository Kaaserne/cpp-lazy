#pragma once

#ifndef LZ_ALGORITHM_INDEX_OF_HPP
#define LZ_ALGORITHM_INDEX_OF_HPP

#include <Lz/algorithm/index_of_if.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Returns the index of the first occurrence of the specified value in the iterable. If the value is not found, it
 * returns lz::npos.
 * @param iterable The iterable to search.
 * @param value The value to search for.
 * @return The index of the first occurrence of the specified value in the iterable, or lz::npos if the value is not found.
 */
template<class Iterable, class T>
LZ_CONSTEXPR_CXX_14 size_t index_of(Iterable&& iterable, const T& value) {
    return detail::index_of_if(detail::begin(iterable), detail::end(iterable),
                               [&value](detail::ref_t<iter_t<Iterable>> val) { return val == value; });
}

} // namespace lz

#endif // LZ_ALGORITHM_INDEX_OF_HPP
