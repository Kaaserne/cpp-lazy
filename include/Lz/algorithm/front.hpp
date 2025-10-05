#pragma once

#ifndef LZ_ALGORITHM_FRONT_HPP
#define LZ_ALGORITHM_FRONT_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/procs/assert.hpp>
#include <Lz/detail/procs/begin_end.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Returns the first element of the given iterable.
 *
 * @param iterable The iterable to get the first element from.
 * @return The first element of the iterable.
 * @throws std::out_of_range if the iterable is empty.
 */
template<class Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::ref_iterable_t<Iterable> front(Iterable&& iterable) {
    return *detail::begin(iterable);
}

} // namespace lz

#endif
