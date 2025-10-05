#pragma once

#ifndef LZ_DETAIL_ALGORITHM_FIND_HPP
#define LZ_DETAIL_ALGORITHM_FIND_HPP

#include <Lz/algorithm/find_if.hpp>
#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>

LZ_MODULE_EXPORT namespace lz {

template<class Iterator, class S, class T>
LZ_CONSTEXPR_CXX_14 Iterator find(Iterator begin, S end, const T& value) {
    using std::find_if;
    using lz::find_if;
    return find_if(std::move(begin), std::move(end), [&value](detail::ref_t<Iterator> val) { return val == value; });
}

/**
 * @brief Finds the first element in the range [begin(iterable), end(iterable)) that satisfies the value @p value
 *
 * @param iterable The iterable to find the element in
 * @param value The value to find
 * @return The iterator to the first element that satisfies the value or `end(iterable)` if the element is not found
 */
template<class Iterable, class T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iter_t<Iterable> find(Iterable&& iterable, const T& value) {
    using lz::find;
    using std::find;
    return find(detail::begin(iterable), detail::end(iterable), value);
}

} // namespace lz

#endif
