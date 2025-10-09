#pragma once

#ifndef LZ_ALOGORITHM_EMPTY_HPP
#define LZ_ALOGORITHM_EMPTY_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/procs/begin_end.hpp>

LZ_MODULE_EXPORT namespace lz {
template<class Iterator, class S>
constexpr bool empty(Iterator begin, S end) {
    return begin == end;
}

/**
 * Checks whether [begin, end) is empty.
 * @param iterable The iterable to check whether it is empty.
 * @return True if it is empty, false otherwise.
 */
template<class Iterable>
LZ_NODISCARD constexpr bool empty(Iterable&& iterable) {
    return lz::empty(detail::begin(iterable), detail::end(iterable));
}
} // namespace lz

#endif
