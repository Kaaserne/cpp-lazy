#pragma once

#ifndef LZ_DUPLICATES_ADAPTOR_HPP
#define LZ_DUPLICATES_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/iterables/duplicates.hpp>
#include <Lz/detail/procs/operators.hpp>
#include <Lz/detail/traits/is_iterable.hpp>

namespace lz {
namespace detail {
struct duplicates_adaptor {
    using adaptor = duplicates_adaptor;

    /**
     * @brief This iterable returns a pair of each element in the iterable and the number of times it appears in the iterable.
     * `pair::first` is the element and `pair::second` is the count. The count is `size_t` and starts from 1. The input
     * iterable needs to be sorted first in order to count the duplicates. The iterator category is min(bidirectional, <input
     * iterator category>). Its end() function will return a sentinel if the input iterable is forward or has a sentinel. This
     * iterable does not contain a .size() method. Example:
     * ```cpp
     * std::vector<int> input{ 1, 2, 2, 3, 4, 4, 5 };
     * auto dupes = lz::duplicates(input); // { { 1, 1 }, { 2, 2 }, { 3, 1 }, { 4, 2 }, { 5, 1 } }
     * ```
     * @param iterable The iterable to check the duplicates of.
     * @param compare The binary predicate to compare the elements of the iterable. Defaults to `std::less`.
     * @return An iterable that returns a pair of each element in the iterable and the number of times it appears in the iterable.
     */
    template<class Iterable, class BinaryPredicate = less>
    LZ_NODISCARD constexpr duplicates_iterable<remove_ref_t<Iterable>, BinaryPredicate>
    operator()(Iterable&& iterable, BinaryPredicate compare = {}) const {
        return { std::forward<Iterable>(iterable), std::move(compare) };
    }

#ifdef LZ_HAS_CONCEPTS

    /**
     * @brief This iterable returns a pair of each element in the iterable and the number of times it appears in the iterable.
     * `pair::first` is the element and `pair::second` is the count. The count is `size_t` and starts from 1. The input
     * iterable needs to be sorted first in order to count the duplicates. The iterator category is min(bidirectional, <input
     * iterator category>). Its end() function will return a sentinel if the input iterable is forward or has a sentinel. This
     * iterable does not contain a .size() method. Example:
     * ```cpp
     * std::vector<int> input{ 1, 2, 2, 3, 4, 4, 5 };
     * auto dupes = input | lz::duplicates; // { { 1, 1 }, { 2, 2 }, { 3, 1 }, { 4, 2 }, { 5, 1 } }
     * ```
     * @param compare The binary predicate to compare the elements of the iterable. Defaults to `std::less`.
     * @return An adaptor that can be used in pipe expressions
     */
    template<class BinaryPredicate = less>
    [[nodiscard]] constexpr fn_args_holder<adaptor, BinaryPredicate> operator()(BinaryPredicate compare = {}) const
        requires(!iterable<BinaryPredicate>)
    {
        return { std::move(compare) };
    }

#else

    /**
     * @brief This iterable returns a pair of each element in the iterable and the number of times it appears in the iterable.
     * `pair::first` is the element and `pair::second` is the count. The count is `size_t` and starts from 1. The input
     * iterable needs to be sorted first in order to count the duplicates. The iterator category is min(bidirectional, <input
     * iterator category>). Its end() function will return a sentinel if the input iterable is forward or has a sentinel. This
     * iterable does not contain a .size() method. Example:
     * ```cpp
     * std::vector<int> input{ 1, 2, 2, 3, 4, 4, 5 };
     * auto dupes = input | lz::duplicates; // { { 1, 1 }, { 2, 2 }, { 3, 1 }, { 4, 2 }, { 5, 1 } }
     * ```
     * @param compare The binary predicate to compare the elements of the iterable. Defaults to `std::less`.
     * @return An adaptor that can be used in pipe expressions
     */
    template<class BinaryPredicate = less>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<!is_iterable<BinaryPredicate>::value, fn_args_holder<adaptor, BinaryPredicate>>
    operator()(BinaryPredicate compare = {}) const {
        return { std::move(compare) };
    }

#endif
};
} // namespace detail
} // namespace lz

#endif
