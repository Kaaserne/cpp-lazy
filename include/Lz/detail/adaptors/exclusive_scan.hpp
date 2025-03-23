#pragma once

#ifndef LZ_EXCLUSIVE_SCAN_ADAPTOR_HPP
#define LZ_EXCLUSIVE_SCAN_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterables/exclusive_scan.hpp>
#include <Lz/detail/traits.hpp>

namespace lz {
namespace detail {
struct exclusive_scan_adaptor {
    using adaptor = exclusive_scan_adaptor;

#ifdef LZ_HAS_CXX_11

    static constexpr adaptor exclusive_scan{};

#endif

    /**
     * @brief Performs an exclusive scan on a container. The first element will be the init value, the second element will be the
     * first element of the container + the init value, the third element will be the second element + last returned element
     * previously, etc. It contains a .size() method if the input iterable also has a .size() method. Its end() function returns a
     * sentinel rather than an iterator. Its iterator category is forward. Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5 };
     * // std::plus is used as the default binary operation
     * auto scan = lz::exclusive_scan(vec, 0); // scan = { 0, 1, 3, 6, 10, 15 }
     * // so 0 (= 0), 0 + 1 (= 1), 1 + 2 (= 3), 3 + 3 (= 6), 6 + 4 (= 10), 10 + 5 (= 15)
     *
     * // you can also add a custom operator:
     * auto scan = vec | lz::exclusive_scan(0, std::plus<int>{}); // scan = { 0, 1, 3, 6, 10, 15 }
     * // When working with pipe expressions, you always need to specify the init value. When working with 'regular' functions,
     * you
     * // can omit the init value, in which case it will be a default constructed object of the type of the container.
     * // Example
     * auto scan = lz::exclusive_scan(vec); // OK
     * auto scan = lz::exclusive_scan(vec, 0); // OK
     * // auto scan = vec | lz::exclusive_scan; // Error, unable to determine the type of the init value
     * auto scan = vec | lz::exclusive_scan(0); // OK
     * ```
     * @param iterable The iterable to perform the exclusive scan on.
     * @param init The initial value to start the scan with.
     * @param binary_op The binary operation to perform on the elements. Plus by default.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class T = val_iterable_t<Iterable>, class BinaryOp = MAKE_BIN_PRED(plus)>
    LZ_NODISCARD constexpr enable_if<is_invocable<BinaryOp, ref_iterable_t<Iterable>, decay_t<T>>::value,
                                     exclusive_scan_iterable<remove_ref<Iterable>, decay_t<T>, BinaryOp>>
    operator()(Iterable&& iterable, T&& init = {}, BinaryOp binary_op = {}) const {
        return { std::forward<Iterable>(iterable), std::forward<T>(init), std::move(binary_op) };
    }

    // clang-format off
    /**
     * @brief Performs an exclusive scan on a container. The first element will be the init value, the second element will be the
     * first element of the container + the init value, the third element will be the second element + last returned element
     * previously, etc. It contains a .size() method if the input iterable also has a .size() method. Its end() function returns a
     * sentinel rather than an iterator. Its iterator category is forward. Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5 };
     * // std::plus is used as the default binary operation
     *
     * // you can also add a custom operator:
     * auto scan = vec | lz::exclusive_scan(0, std::plus<int>{}); // scan = { 0, 1, 3, 6, 10, 15 }
     * // When working with pipe expressions, you always need to specify the init value. When working with 'regular' functions,
     * you
     * // can omit the init value, in which case it will be a default constructed object of the type of the container.
     * // Example
     * auto scan = lz::exclusive_scan(vec); // OK
     * auto scan = lz::exclusive_scan(vec, 0); // OK
     * // auto scan = vec | lz::exclusive_scan; // Error, unable to determine the type of the init value
     * auto scan = vec | lz::exclusive_scan(0); // OK
     * ```
     * @param init The initial value to start the scan with.
     * @param binary_op The binary operation to perform on the elements. Plus by default.
     */
    template<class T, class BinaryOp = MAKE_BIN_PRED(plus)>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14
    enable_if<is_invocable<BinaryOp, decay_t<T>, decay_t<T>>::value, fn_args_holder<adaptor, decay_t<T>, BinaryOp>>
    operator()(T&& init, BinaryOp binary_op = {}) const {
        return { std::forward<T>(init), std::move(binary_op) };
    }
    // clang-format on
};
} // namespace detail
} // namespace lz
#endif
