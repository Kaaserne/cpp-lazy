#pragma once

#ifndef LZ_TRAITS_ITER_TYPE_HPP
#define LZ_TRAITS_ITER_TYPE_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/procs/begin_end.hpp>
#include <utility>

LZ_MODULE_EXPORT namespace lz {
    
/**
 * @brief Can be used to get the iterator type of an iterable. Example: `lz::iter_t<std::vector<int>>` will return
 * `std::vector<int>::iterator`.
 *
 * @tparam Iterable The iterable to get the iterator type from.
 */
template<class Iterable>
using iter_t = decltype(detail::begin(std::declval<Iterable&>()));

/**
 * @brief Can be used to get the sentinel type of an iterable. Example: `lz::sentinel_t<std::vector<int>>` will return
 * `std::vector<int>::iterator`.
 * @tparam Iterable The iterable to get the sentinel type from.
 */
template<class Iterable>
using sentinel_t = decltype(detail::end(std::declval<Iterable&>()));

}

#endif
