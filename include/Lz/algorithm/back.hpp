#pragma once

#ifndef LZ_ALGORITHM_BACK_HPP
#define LZ_ALGORITHM_BACK_HPP

#include <Lz/detail/algorithm/back.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Returns a reference to the last element in the given iterable. If the iterable is not a random access or
 * bidirectional iterable, this function will iterate through the entire iterable to find the last element. If the iterable is
 * empty, the behavior is undefined.
 * @param iterable The iterable to get the last element from.
 * @return A reference to the last element in the iterable.
 */
template<class Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::ref_iterable_t<Iterable> back(Iterable&& iterable) {
    return detail::back(detail::begin(iterable), detail::end(iterable));
}
} // namespace lz

#endif
