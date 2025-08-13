#pragma once

#ifndef LZ_GROUP_BY_ADAPTORS_HPP
#define LZ_GROUP_BY_ADAPTORS_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/iterables/group_by.hpp>

namespace lz {
namespace detail {
struct group_by_adaptor {
    using adaptor = group_by_adaptor;

    /**
     * @brief Creates chunks of elements of which the predicate returns true. Input iterable must be sorted first before using
     * this function. Its end iterator is a sentinel one if ithe input iterable is forward or has a sentinel. The iterable does
     * not contain a .size() method. Its iterator category is bidirectional if possible. Example:
     * ```cpp
     * char str[] = "aaabbccccd";
     * // normally, use std::sort(str.begin(), str.end()) before using this function, if it isn't sorted already
     * auto grouper = lz::group_by(cstr, [](char a, char b) { return a == b; });
     * // grouper = {{'a', 'a', 'a'}, {'b', 'b'}, {'c', 'c', 'c', 'c'}, {'d'}}
     * // or
     * auto grouper = cstr | lz::group_by([](char a, char b) { return a == b; });
     * // grouper = {{'a', 'a', 'a'}, {'b', 'b'}, {'c', 'c', 'c', 'c'}, {'d'}}
     * ```
     * @param iterable The iterable to group by
     * @param binary_predicate The predicate to group by
     * @return An iterable that groups elements by the given predicate
     */
    template<class Iterable, class BinaryPredicate>
    LZ_NODISCARD constexpr group_by_iterable<remove_ref<Iterable>, BinaryPredicate>
    operator()(Iterable&& iterable, BinaryPredicate binary_predicate) const {
        return { std::forward<Iterable>(iterable), std::move(binary_predicate) };
    }

    /**
     * @brief Creates chunks of elements of which the predicate returns true. Input iterable must be sorted first before using
     * this function. Its end iterator is a sentinel one if ithe input iterable is forward or has a sentinel. The iterable does
     * not contain a .size() method. Its iterator category is bidirectional if possible. Example:
     * ```cpp
     * char str[] = "aaabbccccd";
     * // normally, use std::sort(str.begin(), str.end()) before using this function, if it isn't sorted already
     * auto grouper = cstr | lz::group_by([](char a, char b) { return a == b; });
     * // grouper = {{'a', 'a', 'a'}, {'b', 'b'}, {'c', 'c', 'c', 'c'}, {'d'}}
     * ```
     * @param binary_predicate The predicate to group by
     * @return An adaptor that can be used in pipe expressions
     */
    template<class BinaryPredicate>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, BinaryPredicate>
    operator()(BinaryPredicate binary_predicate) const {
        return { std::move(binary_predicate) };
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_GROUP_BY_ADAPTORS_HPP
