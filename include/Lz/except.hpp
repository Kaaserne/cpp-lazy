#pragma once

#ifndef LZ_EXCEPT_HPP
#define LZ_EXCEPT_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/except.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief Excepts an iterable with another iterable. This means that it returns every item that is not in the second iterable
 * argument. Can be used with a custom comparer. Does not contain a .size() method, it's a forward iterator and returns a
 * sentinel. The second iterable must be sorted in order for it to work.
 * Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * std::vector<int> to_except = { 5, 3 };
 * std::sort(to_except.begin(), to_except.end());
 * auto excepted = lz::except(vec, to_except); // excepted = { 1, 2, 4 }
 * // or
 * auto excepted = lz::except(vec, to_except, std::less<int>{}); // excepted = { 1, 2, 4 }
 * // or
 * auto excepted = vec | lz::except(to_except); // excepted = { 1, 2, 4 }
 * // or
 * auto excepted = vec | lz::except(to_except, std::less<int>{}); // excepted = { 1, 2, 4 }
 * ```
 */
constexpr detail::except_adaptor except{};

#else

/**
 * @brief Excepts an iterable with another iterable. This means that it returns every item that is not in the second iterable
 * argument. Can be used with a custom comparer. Does not contain a .size() method, it's a forward iterator and returns a
 * sentinel. The second iterable must be sorted in order for it to work.
 * Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * std::vector<int> to_except = { 5, 3 };
 * std::sort(to_except.begin(), to_except.end());
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

#endif

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
template<class Iterable1, class Iterable2, class BinaryPredicate = MAKE_BIN_PRED(less)>
using except_iterable = detail::except_iterable<Iterable1, Iterable2, BinaryPredicate>;

LZ_MODULE_EXPORT_SCOPE_END

} // end namespace lz

#endif // end LZ_EXCEPT_HPP