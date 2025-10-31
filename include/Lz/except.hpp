#pragma once

#ifndef LZ_EXCEPT_HPP
#define LZ_EXCEPT_HPP

#include <Lz/procs/chain.hpp>
#include <Lz/detail/adaptors/except.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Excepts an iterable with another iterable. This means that it returns every item that is not in the second iterable
 * argument. Can be used with a custom comparer. Does not contain a .size() method, stores the (eager) size of the second iterable
 * internally using lz::cache_size, and its category is min(bidirectional, <category first iterable>). Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * std::vector<int> to_except = { 5, 3 };
 * std::sort(to_except.begin(), to_except.end()); // to_except is now { 3, 5 }
 * auto excepted = lz::except(vec, to_except); // excepted = { 1, 2, 4 }
 * // or
 * auto excepted = lz::except(vec, to_except, std::less<int>{}); // excepted = { 1, 2, 4 }
 * // or
 * auto excepted = vec | lz::except(to_except); // excepted = { 1, 2, 4 }
 * // or
 * auto excepted = vec | lz::except(to_except, std::less<int>{}); // excepted = { 1, 2, 4 }
 * ```
 */
LZ_INLINE_VAR constexpr detail::except_adaptor except{};

/**
 * @brief A type alias for the except iterable.
 * @tparam Iterable1 The first iterable type.
 * @tparam Iterable2 The second iterable type.
 * @tparam BinaryPredicate The binary predicate type used for comparison. Defaults to `std::less`.
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * std::vector<int> to_except = { 5, 3 };
 * std::sort(to_except.begin(), to_except.end());
 * using except_t = lz::except_iterable<std::vector<int>, std::vector<int>, std::less<int>>;
 * except_t excepted = lz::except(vec, to_except, std::less<int>{});
 * ```
 */
template<class Iterable1, class Iterable2, class BinaryPredicate = LZ_BIN_OP(less, detail::val_iterable_t<Iterable1>)>
using except_iterable = detail::except_iterable<Iterable1, Iterable2, BinaryPredicate>;

} // end namespace lz

#endif // end LZ_EXCEPT_HPP
