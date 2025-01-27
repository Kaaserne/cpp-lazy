#pragma once

#ifndef LZ_CHUNK_IF_HPP
#define LZ_CHUNK_IF_HPP

#include <Lz/detail/adaptors/chunk_if.hpp>
#include <Lz/string_view.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief This adaptor is used to make chunks of the iterable, based on a condition returned by the function passed. The iterator
 * category is forward, and returns a sentinel. It returns an iterable of iterables. If `std::string` or `[lz|std]::string_view`
 * is preffed as its `value_type`, please see `s_chunk_if` or `sv_chunk_if` respectively. This iterable does not contain a .size()
 * method.
 *
 * Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto chunked = lz::chunk_if(vec, [](int i) { return i % 2 == 0; }); // chunked = { {1, 2}, {3, 4}, {5} }
 * // or
 * auto chunked = vec | lz::chunk_if([](int i) { return i % 2 == 0; }); // chunked = { {1, 2}, {3, 4}, {5} }
 * }
 * ```
 */
detail::chunk_if_adaptor<void> chunk_if{};

/**
 * @brief This adaptor is used to make chunks of the iterable, based on a condition returned by the function passed. The iterator
 * category is forward, and returns a sentinel. It returns an iterable of `[std|lz]::string_view`. The input iterable must
 * therefore be random_access. This iterable does not contain a .size() method.
 *
 * Example:
 * ```cpp
 * std::string s = "hello;world;;";
 * auto chunked = lz::sv_chunk_if(s, [](char c) { return c == ';'; });
 * // chunked = { string_view{"hello"}, string_view{"world"}, string_view{""}, string_view{""} }
 * // or
 * auto chunked = s | lz::sv_chunk_if([](char c) { return c == ';'; });
 * // chunked = { string_view{"hello"}, string_view{"world"}, string_view{""}, string_view{""} }
 * ```
 */
detail::chunk_if_adaptor<lz::string_view> detail::chunk_if_adaptor<lz::string_view>::sv_chunk_if{};

/**
 * @brief This adaptor is used to make chunks of the iterable, based on a condition returned by the function passed. The iterator
 * category is forward, and returns a sentinel. It returns an iterable of `std::string`. The input iterable must therefore be
 * random_access. This iterable does not contain a .size() method.
 *
 * Example:
 * ```cpp
 * std::string s = "hello;world;;";
 * auto chunked = lz::s_chunk_if(s, [](char c) { return c == ';'; });
 * // chunked = { std::string{"hello"}, std::string{"world"}, std::string{""}, std::string{""} }
 * // or
 * auto chunked = s | lz::s_chunk_if([](char c) { return c == ';'; });
 * // chunked = { std::string{"hello"}, std::string{"world"}, std::string{""}, std::string{""} }
 * ```
 */
detail::chunk_if_adaptor<std::string> detail::chunk_if_adaptor<std::string>::s_chunk_if{};

#else

/**
 * @brief This adaptor is used to make chunks of the iterable, based on a condition returned by the function passed. The iterator
 * category is forward, and returns a sentinel. It returns an iterable of iterables. If `std::string` or `[lz|std]::string_view`
 * is preffed as its `value_type`, please see `s_chunk_if` or `sv_chunk_if` respectively. This iterable does not contain a .size()
 * method.
 *
 * Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto chunked = lz::chunk_if(vec, [](int i) { return i % 2 == 0; }); // chunked = { {1, 2}, {3, 4}, {5} }
 * // or
 * auto chunked = vec | lz::chunk_if([](int i) { return i % 2 == 0; }); // chunked = { {1, 2}, {3, 4}, {5} }
 * }
 * ```
 */
LZ_INLINE_VAR constexpr detail::chunk_if_adaptor<void> chunk_if{};

/**
 * @brief This adaptor is used to make chunks of the iterable, based on a condition returned by the function passed. The iterator
 * category is forward, and returns a sentinel. It returns an iterable of `[std|lz]::string_view`. The input iterable must
 * therefore be random_access. This iterable does not contain a .size() method.
 *
 * Example:
 * ```cpp
 * std::string s = "hello;world;;";
 * auto chunked = lz::sv_chunk_if(s, [](char c) { return c == ';'; });
 * // chunked = { string_view{"hello"}, string_view{"world"}, string_view{""}, string_view{""} }
 * // or
 * auto chunked = s | lz::sv_chunk_if([](char c) { return c == ';'; });
 * // chunked = { string_view{"hello"}, string_view{"world"}, string_view{""}, string_view{""} }
 * ```
 */
LZ_INLINE_VAR constexpr detail::chunk_if_adaptor<lz::string_view> sv_chunk_if{};

/**
 * @brief This adaptor is used to make chunks of the iterable, based on a condition returned by the function passed. The iterator
 * category is forward, and returns a sentinel. It returns an iterable of `std::string`. The input iterable must therefore be
 * random_access. This iterable does not contain a .size() method.
 *
 * Example:
 * ```cpp
 * std::string s = "hello;world;;";
 * auto chunked = lz::s_chunk_if(s, [](char c) { return c == ';'; });
 * // chunked = { std::string{"hello"}, std::string{"world"}, std::string{""}, std::string{""} }
 * // or
 * auto chunked = s | lz::s_chunk_if([](char c) { return c == ';'; });
 * // chunked = { std::string{"hello"}, std::string{"world"}, std::string{""}, std::string{""} }
 * ```
 */
LZ_INLINE_VAR constexpr detail::chunk_if_adaptor<std::string> s_chunk_if{};

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_CHUNK_IF_HPP
