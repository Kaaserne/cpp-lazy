#pragma once

#ifndef LZ_ENUMERATE_HPP
#define LZ_ENUMERATE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/enumerate.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief Returns an iterable that enumerates the elements of the input iterable, meaning it returns a std::pair<IntType,
 * ValueType>, where std::pair::first_type is an integer (corresponding to the current index) and std::pair::second_type is
 * the value of the input iterable (by reference). The index starts at 0 by default, but can be changed by passing a start
 * value to the function. If the input iterable is exactly bidirectional and not sized (like `lz::filter` for example), the entire
 * sequence is traversed to get its end size (using `lz::eager_size`), so it may be worth your while to use `lz::cache_size`.
 * It will return a `default_sentinel` if the input iterable is forward. The iterator category is the same as its input
 * iterable. Example:
 * ```cpp
 * std::forward_list<int> list = {1, 2, 3, 4, 5};
 * auto enumerated = lz::enumerate(list); // enumerated = { {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5} }
 * auto enumerated = lz::enumerate(list, 5); // enumerated = { {5, 1}, {6, 2}, {7, 3}, {8, 4}, {9, 5} }
 *
 * // or
 *
 * auto enumerated = list | lz::enumerate; // enumerated = { {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5} }
 * auto enumerated = list | lz::enumerate(5); // enumerated = { {5, 1}, {6, 2}, {7, 3}, {8, 4}, {9, 5} }
 * ```
 * So, all in all: use lz::cache_size if:
 * - Your iterable is exactly bidirectional (so forward/random access excluded) and
 * - Your iterable is not sized and
 * - You either use multiple/a combination of the following iterables OR (see last point):
 * - `lz::chunks`
 * - `lz::enumerate`
 * - `lz::exclude`
 * - `lz::interleave`
 * - `lz::take`
 * - `lz::take_every`
 * - `lz::zip_longest`
 * - `lz::zip`
 * - Are planning to call begin() or end() multiple times on the same instance (with one or more of the above iterable
 * combinations)
 */
constexpr detail::enumerate_adaptor enumerate{};

#else

/**
 * @brief Returns an iterable that enumerates the elements of the input iterable, meaning it returns a std::pair<IntType,
 * ValueType>, where std::pair::first_type is an integer (corresponding to the current index) and std::pair::second_type is
 * the value of the input iterable (by reference). The index starts at 0 by default, but can be changed by passing a start
 * value to the function. If the input iterable is exactly bidirectional and not sized (like `lz::filter` for example), the entire
 * sequence is traversed to get its end size (using `lz::eager_size`), so it may be worth your while to use `lz::cache_size`.
 * It will return a `default_sentinel` if the input iterable is forward. The iterator category is the same as its input
 * iterable. Example:
 * ```cpp
 * std::forward_list<int> list = {1, 2, 3, 4, 5};
 * auto enumerated = lz::enumerate(list); // enumerated = { {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5} }
 * auto enumerated = lz::enumerate(list, 5); // enumerated = { {5, 1}, {6, 2}, {7, 3}, {8, 4}, {9, 5} }
 *
 * // or
 *
 * auto enumerated = list | lz::enumerate; // enumerated = { {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5} }
 * auto enumerated = list | lz::enumerate(5); // enumerated = { {5, 1}, {6, 2}, {7, 3}, {8, 4}, {9, 5} }
 * ```
 * So, all in all: use lz::cache_size if:
 * - Your iterable is exactly bidirectional (so forward/random access excluded) and
 * - Your iterable is not sized and
 * - You either use multiple/a combination of the following iterables OR (see last point):
 * - `lz::chunks`
 * - `lz::enumerate`
 * - `lz::exclude`
 * - `lz::interleave`
 * - `lz::take`
 * - `lz::take_every`
 * - `lz::zip_longest`
 * - `lz::zip`
 * - Are planning to call begin() or end() multiple times on the same instance (with one or more of the above iterable
 * combinations)
 */
LZ_INLINE_VAR constexpr detail::enumerate_adaptor enumerate{};

#endif

using detail::enumerate_iterable;

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif