#pragma once

#ifndef LZ_UNIQUE_ADPTOR_HPP
#define LZ_UNIQUE_ADPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/iterables/unique.hpp>

namespace lz {
namespace detail {
struct unique_adaptor {
    using adaptor = unique_adaptor;

#ifdef LZ_HAS_CONCEPTS

    /**
     * @brief Makes the input iterable unique. Every element therefore only occurs once. The input iterable must be sorted
     * beforehand. This iterator will 'decay' into a bidirectional one if the input iterator is bidirectional or higher. If the
     * input iterable is not bidirectional or higher, then the output iterator will be forward, and will also return a sentinel
     * (or if the input iterable has a sentinel), rather than an iterator. It will also return a sentinel if the input iterable
     * contains a sentinel. This method does not contain a .size() method. Example:
     * ```cpp
     * std::vector<int> vec = { 1, 1, 2, 3, 3, 3, 4, 5, 5 };
     * std::sort(vec.begin(), vec.end());
     * auto unique = lz::unique(vec);
     * auto unique = lz::unique(vec, std::less<>{});
     * ```
     * @param iterable The iterable to make unique. Must be sorted beforehand.
     * @param predicate The predicate to compare the elements with. The default is std::less<>{}
     * @return An iterable that contains only unique elements from the input iterable.
     */
    template<class Iterable, class BinaryPredicate = MAKE_BIN_PRED(less)>
    [[nodiscard]] constexpr unique_iterable<remove_ref<Iterable>, BinaryPredicate>
    operator()(Iterable&& iterable, BinaryPredicate predicate = {}) const
        requires(lz::iterable<Iterable>)
    {
        return { std::forward<Iterable>(iterable), std::move(predicate) };
    }

    /**
     * @brief Makes the input iterable unique. Every element therefore only occurs once. The input iterable must be sorted
     * beforehand. This iterator will 'decay' into a bidirectional one if the input iterator is bidirectional or higher. If the
     * input iterable is not bidirectional or higher, then the output iterator will be forward, and will also return a sentinel
     * (or if the input iterable has a sentinel), rather than an iterator. It will also return a sentinel if the input iterable
     * contains a sentinel.This method does not contain a .size() method. Example:
     * ```cpp
     * std::vector<int> vec = { 1, 1, 2, 3, 3, 3, 4, 5, 5 };
     * std::sort(vec.begin(), vec.end());
     * auto unique = vec | lz::unique;
     * // custom comparer can be passed as argument as well:
     * auto unique = vec | lz::unique(std::less<>{});
     * ```
     * @param predicate The predicate to compare the elements with. The default is std::less<>{}
     * @return An adaptor that can be used in pipe expressions
     */
    template<class BinaryPredicate = MAKE_BIN_PRED(less)>
    [[nodiscard]] constexpr fn_args_holder<adaptor, BinaryPredicate> operator()(BinaryPredicate predicate = {}) const
        requires(!is_iterable<BinaryPredicate>::value)
    {
        return { std::move(predicate) };
    }

#else

    /**
     * @brief Makes the input iterable unique. Every element therefore only occurs once. The input iterable must be sorted
     * beforehand. This iterator will 'decay' into a bidirectional one if the input iterator is bidirectional or higher. If the
     * input iterable is not bidirectional or higher, then the output iterator will be forward, and will also return a sentinel
     * (or if the input iterable has a sentinel), rather than an iterator. It will also return a sentinel if the input iterable
     * contains a sentinel. This method does not contain a .size() method. Example:
     * ```cpp
     * std::vector<int> vec = { 1, 1, 2, 3, 3, 3, 4, 5, 5 };
     * std::sort(vec.begin(), vec.end());
     * auto unique = lz::unique(vec);
     * auto unique = lz::unique(vec, std::less<>{});
     * ```
     * @param iterable The iterable to make unique. Must be sorted beforehand.
     * @param predicate The predicate to compare the elements with. The default is std::less<>{}
     * @return An iterable that contains only unique elements from the input iterable.
     */
    template<class Iterable, class BinaryPredicate = MAKE_BIN_PRED(less)>
    LZ_NODISCARD constexpr enable_if<is_iterable<Iterable>::value, unique_iterable<remove_ref<Iterable>, BinaryPredicate>>
    operator()(Iterable&& iterable, BinaryPredicate predicate = {}) const {
        return { std::forward<Iterable>(iterable), std::move(predicate) };
    }

    /**
     * @brief Makes the input iterable unique. Every element therefore only occurs once. The input iterable must be sorted
     * beforehand. This iterator will 'decay' into a bidirectional one if the input iterator is bidirectional or higher. If the
     * input iterable is not bidirectional or higher, then the output iterator will be forward, and will also return a sentinel
     * (or if the input iterable has a sentinel), rather than an iterator. It will also return a sentinel if the input iterable
     * contains a sentinel.This method does not contain a .size() method. Example:
     * ```cpp
     * std::vector<int> vec = { 1, 1, 2, 3, 3, 3, 4, 5, 5 };
     * std::sort(vec.begin(), vec.end());
     * auto unique = vec | lz::unique;
     * // custom comparer can be passed as argument as well:
     * auto unique = vec | lz::unique(std::less<>{});
     * ```
     * @param predicate The predicate to compare the elements with. The default is std::less<>{}
     * @return An adaptor that can be used in pipe expressions
     */
    template<class BinaryPredicate = MAKE_BIN_PRED(less)>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_iterable<BinaryPredicate>::value, fn_args_holder<adaptor, BinaryPredicate>>
    operator()(BinaryPredicate predicate = {}) const {
        return { std::move(predicate) };
    }

#endif
};
} // namespace detail
} // namespace lz

#endif // LZ_UNIQUE_ADPTOR_HPP
