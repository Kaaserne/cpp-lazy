#pragma once

#ifndef LZ_INTERSECTION_ADAPTOR_HPP
#define LZ_INTERSECTION_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterables/intersection.hpp>

namespace lz {
namespace detail {
struct intersection_adaptor {
    using adaptor = intersection_adaptor;

    // clang-format off

    /**
     * @brief Intersects the first iterable with the second iterable. The result is a new iterable containing the elements that
     * are in both iterables. Returns a bidirectional iterable if the input iterable is at least bidirectional, otherwise forward.
     * Returns a sentinel if it is a forward iterable or a sentinel. Does not contain a .size() method. Example:
     * ```cpp
     * std::string a = "aaaabbcccddee";
     * std::string b = "aabccce";
     *
     * std::sort(a.begin(), a.end());
     * std::sort(b.begin(), b.end());
     *
     * auto intersect = lz::intersection(a, b); // { 'a', 'a', 'b', 'c', 'c', 'c', 'e' }
     * // or
     * auto intersect = lz::intersection(a, b, std::less<>{}); // { 'a', 'a', 'b', 'c', 'c', 'c', 'e' }
     * ```
     * @param iterable The first iterable
     * @param iterable2 The second iterable
     * @param compare The comparison function. std::less<> by default
     * @return An iterable that contains the intersection of the two iterables
     */
    template<class Iterable, class Iterable2, class BinaryPredicate = MAKE_BIN_PRED(less)>
    LZ_NODISCARD constexpr
    enable_if<is_iterable<Iterable2>::value, intersection_iterable<remove_ref<Iterable>, remove_ref<Iterable2>, BinaryPredicate>>
    operator()(Iterable&& iterable, Iterable2&& iterable2, BinaryPredicate compare = {}) const {
        return { std::forward<Iterable>(iterable), std::forward<Iterable2>(iterable2), std::move(compare) };
    }

    // clang-format on

    /**
     * @brief Intersects the first iterable with the second iterable. The result is a new iterable containing the elements that
     * are in both iterables.Returns a bidirectional iterable if the input iterable is at least bidirectional, otherwise forward.
     * Returns a sentinel if it is a forward iterable or a sentinel. Does not contain a .size() method. Example:
     * ```cpp
     * std::string a = "aaaabbcccddee";
     * std::string b = "aabccce";
     *
     * std::sort(a.begin(), a.end());
     * std::sort(b.begin(), b.end());
     *
     * auto intersect = a | lz::intersection(b); // { 'a', 'a', 'b', 'c', 'c', 'c', 'e' }
     * // or
     * auto intersect = a | lz::intersection(b, std::less<>{}); // { 'a', 'a', 'b', 'c', 'c', 'c', 'e' }
     * ```
     * @param iterable2 The second iterable
     * @param compare The comparison function. std::less<> by default
     */
    template<class Iterable2, class BinaryPredicate = MAKE_BIN_PRED(less)>
    LZ_NODISCARD constexpr enable_if<!is_iterable<BinaryPredicate>::value, fn_args_holder<adaptor, Iterable2, BinaryPredicate>>
    operator()(Iterable2&& iterable2, BinaryPredicate compare = {}) const {
        return { std::forward<Iterable2>(iterable2), std::move(compare) };
    }
};
} // namespace detail
} // namespace lz

#endif
