#pragma once

#ifndef LZ_ALGORITHM_FIND_LAST_HPP
#define LZ_ALGORITHM_FIND_LAST_HPP

#include <Lz/algorithm/find_last_if.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Finds the first element in the range [begin(iterable), end(iterable)) that satisfies the value @p value.
 * Does not use `lz::cached_reverse` to reverse the elements (if bidirectional).
 * @param iterable The iterable to find the element in
 * @param value The value to find
 * @return An iterator to the first element that satisfies the unary_predicate or `end(iterable)` if the element is not found
 */
template<class Iterable, class T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iter_t<Iterable> find_last(Iterable&& iterable, const T& value) {
    return lz::find_last_if(std::forward<Iterable>(iterable),
                            [&value](detail::ref_t<iter_t<Iterable>> val) { return val == value; });
}

} // namespace lz

#endif
