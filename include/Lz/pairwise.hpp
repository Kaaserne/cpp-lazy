#pragma once

#ifndef LZ_PAIRWISE_HPP
#define LZ_PAIRWISE_HPP

#include <Lz/procs/chain.hpp>
#include <Lz/detail/adaptors/pairwise.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Creates a pairwise iterable with pairs of size @p pair_size from the given @p iterable
 * ```cpp
 * auto vec = std::vector{1, 2, 3, 4, 5};
 * auto paired = lz::pairwise(vec, 2); // {{1, 2}, {2, 3}, {3, 4}, {4, 5}}
 * auto pairwise_by_2 = vec | lz::pairwise(2); // {{1, 2}, {2, 3}, {3, 4}, {4, 5}}
 * ```
 */
LZ_INLINE_VAR constexpr detail::pairwise_adaptor pairwise{};

/**
 * @brief The pairwise iterable helper alias.
 *
 * @tparam Iterable The iterable type
 * ```cpp
 * std::vector<int> vec = {1, 2, 3, 4, 5};
 * lz::pairwise_iterable<std::vector<int>> paired = lz::pairwise(vec, 2);
 * ```
 */
template<class Iterable>
using pairwise_iterable = detail::pairwise_iterable<Iterable>;

} // namespace lz

#endif
