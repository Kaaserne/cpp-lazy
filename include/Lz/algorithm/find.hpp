#pragma once

#ifndef LZ_DETAIL_ALGORITHM_FIND_HPP
#define LZ_DETAIL_ALGORITHM_FIND_HPP

#include <Lz/algorithm/find_if.hpp>
#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Finds the first element in the range [begin(iterable), end(iterable)) that satisfies the value @p value
 *
 * @param iterable The iterable to find the element in
 * @param value The value to find
 * @return The iterator to the first element that satisfies the value or `end(iterable)` if the element is not found
 */
template<class Iterable, class T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iter_t<Iterable> find(Iterable&& iterable, const T& value) {
    return lz::find_if(std::forward<Iterable>(iterable), [&value](detail::ref_t<iter_t<Iterable>> val) { return val == value; });
}

} // namespace lz

#endif
