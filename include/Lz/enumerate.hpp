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
 * ValueType>, where std::pair::first_type is an integer (corresponding to the current index) and std::pair::second_type is the
 * value of the input iterable (by reference). The index starts at 0 by default, but can be changed by passing a start value to
 * the function. Its end() and begin() function will return the same type if its input iterable is sized, otherwise it will return
 * the same type as the input iterable. The iterator category is the same as its input iterable. Example:
 * ```cpp
 * std::forward_list<int> list = {1, 2, 3, 4, 5};
 * auto enumerated = lz::enumerate(list); // enumerated = { {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5} }
 * // enumeated contains a .size() method. Its end() and begin() function return the same iterator type
 * // or, with start index
 * auto enumerated = lz::enumerate(list, 5); // enumerated = { {5, 1}, {6, 2}, {7, 3}, {8, 4}, {9, 5} }
 * // or
 * auto enumerated = list | lz::enumerate(5); // enumerated = { {5, 1}, {6, 2}, {7, 3}, {8, 4}, {9, 5} }
 * // or
 * auto enumerated = list | lz::enumerate; // enumerated = { {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5} }
 * auto c_str = lz::c_string("hello");
 * auto enumerated = lz::enumerate(c_str); // enumerated = { {0, 'h'}, {1, 'e'}, {2, 'l'}, {3, 'l'}, {4, 'o'} }
 * // does not contain a .size() method. Its end() and begin() function return a different iterator type
 * ```
 */
static constexpr detail::enumerate_adaptor detail::enumerate_adaptor::enumerate{};

#else

/**
 * @brief Returns an iterable that enumerates the elements of the input iterable, meaning it returns a std::pair<IntType,
 * ValueType>, where std::pair::first_type is an integer (corresponding to the current index) and std::pair::second_type is the
 * value of the input iterable (by reference). The index starts at 0 by default, but can be changed by passing a start value to
 * the function. Its end() and begin() function will return the same type if its input iterable is sized, otherwise it will return
 * the same type as the input iterable. The iterator category is the same as its input iterable. Example:
 * ```cpp
 * std::forward_list<int> list = {1, 2, 3, 4, 5};
 * auto enumerated = lz::enumerate(list); // enumerated = { {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5} }
 * // enumeated contains a .size() method. Its end() and begin() function return the same iterator type
 * // or, with start index
 * auto enumerated = lz::enumerate(list, 5); // enumerated = { {5, 1}, {6, 2}, {7, 3}, {8, 4}, {9, 5} }
 * // or
 * auto enumerated = list | lz::enumerate(5); // enumerated = { {5, 1}, {6, 2}, {7, 3}, {8, 4}, {9, 5} }
 * // or
 * auto enumerated = list | lz::enumerate; // enumerated = { {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5} }
 * auto c_str = lz::c_string("hello");
 * auto enumerated = lz::enumerate(c_str); // enumerated = { {0, 'h'}, {1, 'e'}, {2, 'l'}, {3, 'l'}, {4, 'o'} }
 * // does not contain a .size() method. Its end() and begin() function return a different iterator type
 * ```
 */
LZ_INLINE_VAR constexpr detail::enumerate_adaptor enumerate{};

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif