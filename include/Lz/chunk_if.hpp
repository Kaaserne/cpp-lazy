#pragma once

#ifndef LZ_CHUNK_IF_HPP
#define LZ_CHUNK_IF_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/chunk_if.hpp>
#include <Lz/string_view.hpp>

LZ_MODULE_EXPORT namespace lz {

#ifdef LZ_HAS_CXX_11

/**
 * @brief This adaptor is used to make chunks of the iterable, based on a condition returned by the function passed. The iterator
 * category is forward, and returns a sentinel. It returns an iterable of iterables, with its value_type being
 * the template parameter passed. If `std::string` or `[lz|std]::string_view` is prefered as its `value_type`, please see
 * `sv_chunk_if`. This iterable does not contain a .size() method. Iterable is forward only.
 *
 * Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * // Both return an iterable of std::vector<int>
 * auto chunked = lz::t_chunk_if<std::vector<int>>{}(vec, [](int i) { return i % 2 == 0; }); // chunked = { {1, 2}, {3, 4}, {5} } }
 * // or
 * auto chunked = vec | lz::t_chunk_if<std::vector<int>>{}([](int i) { return i % 2 == 0; }); // chunked = { {1, 2}, {3, 4}, {5} } }
 * ```
 * @tparam ValueType The value type of the chunked iterable.
 */
template<class ValueType>
using t_chunk_if = detail::chunk_if_adaptor<ValueType>;

#else

/**
 * @brief This adaptor is used to make chunks of the iterable, based on a condition returned by the function passed. The iterator
 * category is forward, and returns a sentinel. It returns an iterable of iterables, with its value_type being
 * the template parameter passed. If `std::string` or `[lz|std]::string_view` is prefered as its `value_type`, please see
 * `sv_chunk_if`. This iterable does not contain a .size() method. Iterable is forward only.
 *
 * Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * // Both return an iterable of std::vector<int>
 * auto chunked = lz::t_chunk_if<std::vector<int>>(vec, [](int i) { return i % 2 == 0; }); // chunked = { {1, 2}, {3, 4}, {5} } }
 * // or
 * auto chunked = vec | lz::t_chunk_if<std::vector<int>>([](int i) { return i % 2 == 0; }); // chunked = { {1, 2}, {3, 4}, {5} } }
 * ```
 * @tparam ValueType The value type of the chunked iterable.
 */
template<class ValueType>
LZ_INLINE_VAR constexpr detail::chunk_if_adaptor<ValueType> t_chunk_if;

#endif

/**
 * @brief This adaptor is used to make chunks of the iterable, based on a condition returned by the function passed. The iterator
 * category is forward, and returns a sentinel. It returns an iterable of iterables, with its value_type being
 * `lz::basic_iterable`. If `std::string` or `[lz|std]::string_view` is prefered as its `value_type`, please see
 * `sv_chunk_if`. This iterable does not contain a .size() method. Iterable is forward only.
 *
 * Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto chunked = lz::chunk_if(vec, [](int i) { return i % 2 == 0; }); // chunked = { {1, 2}, {3, 4}, {5} }
 * // or
 * auto chunked = vec | lz::chunk_if([](int i) { return i % 2 == 0; }); // chunked = { {1, 2}, {3, 4}, {5} }
 * ```
 */
LZ_INLINE_VAR constexpr detail::chunk_if_adaptor<void> chunk_if{};

/**
 * @brief This adaptor is used to make chunks of the iterable, based on a condition returned by the function passed. The iterator
 * category is forward, and returns a sentinel. It returns an iterable of `[std|lz]::string_view`. The input iterable must
 * therefore be random_access. This iterable does not contain a .size() method. Iterable is forward only.
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
 * @brief Helper alias for the chunk_if_iterable.
 * @tparam Iterable The type of the input iterable.
 * @tparam UnaryPredicate The type of the predicate function.
 * Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * using chunked_t = lz::chunk_if_iterable<std::vector<int>, std::vector<int>, std::function<bool(int)>>;
 * chunked_t chunked = lz::chunk_if(vec, [](int i) { return i % 2 == 0; });
 * ```
 */
template<class Iterable, class UnaryPredicate>
using chunk_if_iterable = detail::chunk_if_iterable<basic_iterable<iter_t<Iterable>, iter_t<Iterable>>, Iterable, UnaryPredicate>;

/**
 * @brief Helper alias for the chunk_if_iterable.
 * @tparam ValueType The value type of the chunked iterable which is returned by this iterator operator*.
 * @tparam Iterable The type of the input iterable.
 * @tparam UnaryPredicate The type of the predicate function.
 * Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * using chunked_t = lz::chunk_if_iterable<std::vector<int>, std::vector<int>, std::function<bool(int)>>;
 * chunked_t chunked = lz::chunk_if(vec, [](int i) { return i % 2 == 0; });
 * ```
 */
template<class ValueType, class Iterable, class UnaryPredicate>
using t_chunk_if_iterable = detail::chunk_if_iterable<ValueType, Iterable, UnaryPredicate>;

} // namespace lz

#endif // LZ_CHUNK_IF_HPP
