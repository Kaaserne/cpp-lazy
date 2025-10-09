#pragma once

#ifndef LZ_ALGORITHM_HAS_MANY_HPP
#define LZ_ALGORITHM_HAS_MANY_HPP

#include <Lz/algorithm/empty.hpp>

LZ_MODULE_EXPORT namespace lz {
template<class Iterator, class S>
LZ_CONSTEXPR_CXX_14 bool has_many(Iterator begin, S end) {
    return !lz::empty(begin, end) && !lz::empty(++begin, end);
}

/**
 * Returns whether the sequence holds >= 2 elements.
 * @param iterable The sequence to check whether it has many (>= 2) elements.
 * @return True if the amount of values are >= 2, false otherwise.
 */
template<class Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool has_many(Iterable&& iterable) {
    return lz::has_many(detail::begin(iterable), detail::end(iterable));
}
} // namespace lz

#endif
