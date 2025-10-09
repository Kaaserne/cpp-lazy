#pragma once

#ifndef LZ_DISTANCE_HPP
#define LZ_DISTANCE_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/procs/distance.hpp>
#include <Lz/detail/traits/enable_if.hpp>
#include <Lz/detail/traits/remove_ref.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Gets the distance of an iterator pair. If iterator is not random access, the operation will be O(n), otherwise O(1).
 *
 * @param begin The beginning of the iterator
 * @param end The end of the iterator
 * @return The length of the iterator
 */
template<class Iterator, class S>
LZ_NODISCARD constexpr detail::diff_type<Iterator> distance(Iterator begin, S end) {
    return detail::distance_impl(std::move(begin), std::move(end));
}

/**
 * @brief Gets the distance of an iterable. If iterable is not random access, the operation will be O(n), otherwise O(1).
 *
 * @param iterable The iterable to get the distance of
 * @return The length of the iterable
 */
template<class Iterable>
LZ_NODISCARD constexpr detail::diff_iterable_t<detail::remove_cvref_t<Iterable>> distance(Iterable&& iterable) {
    using std::distance;
    return distance(detail::begin(iterable), detail::end(iterable));
}
} // namespace lz

#endif
