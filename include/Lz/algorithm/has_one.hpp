#pragma once

#ifndef LZ_ALGORITHM_HAS_ONE_HPP
#define LZ_ALGORITHM_HAS_ONE_HPP

#include <Lz/algorithm/empty.hpp>
#include <Lz/basic_iterable.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * Returns whether the sequence holds exactly one element.
 * @param iterable The sequence to check whether is has exactly one element.
 * @return True if it has one element exactly, false otherwise.
 */
template<class Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool has_one(Iterable&& iterable) {
    auto begin = detail::begin(iterable);
    auto end = detail::end(iterable);
    return !lz::empty(iterable) && lz::empty(make_basic_iterable(++begin, end));
}

} // namespace lz

#endif
