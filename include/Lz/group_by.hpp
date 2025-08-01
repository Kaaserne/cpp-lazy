#pragma once

#ifndef LZ_GROUP_BY_HPP
#define LZ_GROUP_BY_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/group_by.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Creates chunks of elements of which the predicate returns true. Input iterable must be sorted first before using this
 * function. Its end iterator is a sentinel one if ithe input iterable is forward or has a sentinel. The iterable does not contain
 * a .size() method. Its iterator category is bidirectional if possible. Example:
 * ```cpp
 * char str[] = "aaabbccccd";
 * // normally, use std::sort(std::begin(str), std::end(str)) before using this function, if it isn't sorted already
 * auto grouper = lz::group_by(cstr, [](char a, char b) { return a == b; });
 * // grouper = {{'a', 'a', 'a'}, {'b', 'b'}, {'c', 'c', 'c', 'c'}, {'d'}}
 * // or
 * auto grouper = cstr | lz::group_by([](char a, char b) { return a == b; });
 * // grouper = {{'a', 'a', 'a'}, {'b', 'b'}, {'c', 'c', 'c', 'c'}, {'d'}}
 * ```
 */
LZ_INLINE_VAR constexpr detail::group_by_adaptor group_by{};

/**
 * @brief Helper alias for group by iterable.
 * @tparam Iterable The type of the iterable to group by.
 * ```cpp
 * char str[] = "aaabbccccd";
 * using group_t = lz::group_by_iterable<decltype(str), std::function<bool(char, char)>>;
 * group_t grouper = lz::group_by(str, [](char a, char b) { return a == b; });
 * ```
 */
template<class Iterable, class BinaryPredicate>
using group_by_iterable = detail::group_by_iterable<Iterable, BinaryPredicate>;

} // namespace lz

#endif // LZ_GROUP_BY_HPP
