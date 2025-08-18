#pragma once

#ifndef LZ_EXCEPT_ADAPTOR_HPP
#define LZ_EXCEPT_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/iterables/except.hpp>
#include <Lz/detail/traits.hpp>

namespace lz {
namespace detail {
struct except_adaptor {
    using adaptor = except_adaptor;

    /**
     * @brief Excepts an iterable with another iterable. This means that it returns every item that is not in the second iterable
     * argument. Can be used with a custom comparer. Does not contain a .size() method, it's a bidirectional iterator if the first
     * iterable is at least bidirectional and returns a sentinel if its first iterable has a sentinel or is forward only. The
     * second iterable must be sorted in order for it to work. Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5 };
     * std::vector<int> to_except = { 5, 3 };
     * std::sort(to_except.begin(), to_except.end());
     * auto excepted = lz::except(vec, to_except); // excepted = { 1, 2, 4 }
     * // or
     * auto excepted = lz::except(vec, to_except, std::less<int>{}); // excepted = { 1, 2, 4 }
     * ```
     * @param iterable1 The iterable to except.
     * @param iterable2 The iterable that must be skipped in @p iterable1.
     * @param binary_predicate The binary predicate to use for comparison.
     * @return An iterable that contains every item from @p iterable1 that is not in @p iterable2.
     */
    template<class Iterable1, class Iterable2, class BinaryPredicate = MAKE_BIN_PRED(less)>
    LZ_NODISCARD constexpr except_iterable<remove_ref<Iterable1>, remove_ref<Iterable2>, BinaryPredicate>
    operator()(Iterable1&& iterable1, Iterable2&& iterable2, BinaryPredicate binary_predicate = {}) const {
        return { std::forward<Iterable1>(iterable1), std::forward<Iterable2>(iterable2), std::move(binary_predicate) };
    }

#ifdef LZ_HAS_CONCEPTS

    /**
     * @brief Excepts an iterable with another iterable. This means that it returns every item that is not in the second iterable
     * argument. Can be used with a custom comparer. Does not contain a .size() method, it's a bidirectional iterator if the first
     * iterable is at least bidirectional and returns a sentinel if its first iterable has a sentinel or is forward only. The
     * second iterable must be sorted in order for it to work.
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
     * @param iterable2 The iterable that must be skipped in @p iterable1.
     * @param binary_predicate The binary predicate to use for comparison.
     * @return An adaptor that can be used in pipe expressions
     */
    template<class Iterable2, class BinaryPredicate = MAKE_BIN_PRED(less)>
    [[nodiscard]] constexpr fn_args_holder<adaptor, Iterable2, BinaryPredicate>
    operator()(Iterable2&& iterable2, BinaryPredicate binary_predicate = {}) const
        requires(!lz::iterable<BinaryPredicate>)
    {
        return { std::forward<Iterable2>(iterable2), std::move(binary_predicate) };
    }

#else

    /**
     * @brief Excepts an iterable with another iterable. This means that it returns every item that is not in the second iterable
     * argument. Can be used with a custom comparer. Does not contain a .size() method, it's a bidirectional iterator if the first
     * iterable is at least bidirectional and returns a sentinel if its first iterable has a sentinel or is forward only. The
     * second iterable must be sorted in order for it to work.
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
     * @param iterable2 The iterable that must be skipped in @p iterable1.
     * @param binary_predicate The binary predicate to use for comparison.
     * @return An adaptor that can be used in pipe expressions
     */
    template<class Iterable2, class BinaryPredicate = MAKE_BIN_PRED(less)>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14
    enable_if<!is_iterable<BinaryPredicate>::value, fn_args_holder<adaptor, Iterable2, BinaryPredicate>>
    operator()(Iterable2&& iterable2, BinaryPredicate binary_predicate = {}) const {
        return { std::forward<Iterable2>(iterable2), std::move(binary_predicate) };
    }

#endif
};
} // namespace detail
} // namespace lz

#endif // LZ_EXCEPT_ADAPTOR_HPP