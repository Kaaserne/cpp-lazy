#pragma once

#ifndef LZ_ALGORITHM_HAS_ONE_HPP
#define LZ_ALGORITHM_HAS_ONE_HPP

#include <Lz/algorithm/empty.hpp>

LZ_MODULE_EXPORT namespace lz {

template<class Iterator, class S>
LZ_CONSTEXPR_CXX_14 bool has_one(Iterator begin, S end) {
    return !lz::empty(begin, end) && lz::empty(++begin, end);
}

/**
 * Returns whether the sequence holds exactly one element.
 * @param iterable The sequence to check whether is has exactly one element.
 * @return True if it has one element exactly, false otherwise.
 */
template<class Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool has_one(Iterable&& iterable) {
    return lz::has_one(detail::begin(iterable), detail::end(iterable));
}

} // namespace lz

#endif
