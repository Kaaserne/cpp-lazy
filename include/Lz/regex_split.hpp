#pragma once

#ifndef LZ_REGEX_SPLIT_HPP
#define LZ_REGEX_SPLIT_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/regex_split.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief Splits a string based on a regex. The regex must be by reference. The `begin()` and `end()` types are different, but
 * `end()` is not an 'actual' sentinel. Rather, its the same type as its input iterator `end()` type. For std::regex, this means
 * `end() == std::regex_token_iterator<>{}` and `begin() = lz::regex_split_iterator`. It does not contain a .size() method and its
 * iterator category is forward. Example:
 * ```cpp
 * std::regex r1(R"(\s+)");
 * std::string s = "    Hello, world! How are you?";
 * auto splitter = lz::regex_split(s, r1); // { "Hello,", "world!", "How", "are", "you?" }
 * // or
 * auto splitter = s | lz::regex_split(r1); // { "Hello,", "world!", "How", "are", "you?" }
 * // you can also use your own regex iterators, as std::regex isn't that performant, example (still using std:: variant though)
 * auto begin = std::sregex_token_iterator(s.begin(), s.end(), r1, -1); // -1 means it will skip the matches (which is exactly
 * what we want), rather than returning the matches auto end = std::sregex_token_iterator<>();
 * auto splitter = lz::regex_split(begin, end); // { "Hello,", "world!", "How", "are", "you?" }
 * ```
 */
static const detail::regex_split_adaptor regex_split{};

#else

/**
 * @brief Splits a string based on a regex. The regex must be by reference. The `begin()` and `end()` types are different, but
 * `end()` is not an 'actual' sentinel. Rather, its the same type as its input iterator `end()` type. For std::regex, this means
 * `end() == std::regex_token_iterator<>{}` and `begin() = lz::regex_split_iterator`. It does not contain a .size() method and its
 * iterator category is forward. Example:
 * ```cpp
 * std::regex r1(R"(\s+)");
 * std::string s = "    Hello, world! How are you?";
 * auto splitter = lz::regex_split(s, r1); // { "Hello,", "world!", "How", "are", "you?" }
 * // or
 * auto splitter = s | lz::regex_split(r1); // { "Hello,", "world!", "How", "are", "you?" }
 * // you can also use your own regex iterators, as std::regex isn't that performant, example (still using std:: variant though)
 * auto begin = std::sregex_token_iterator(s.begin(), s.end(), r1, -1); // -1 means it will skip the matches (which is exactly
 * what we want), rather than returning the matches auto end = std::sregex_token_iterator<>();
 * auto splitter = lz::regex_split(begin, end); // { "Hello,", "world!", "How", "are", "you?" }
 * ```
 */
LZ_INLINE_VAR constexpr detail::regex_split_adaptor regex_split{};

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif