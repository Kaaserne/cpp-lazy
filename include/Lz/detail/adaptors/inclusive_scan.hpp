#pragma once

#ifndef LZ_INCLUSIVE_SCAN_ADAPTOR_HPP
#define LZ_INCLUSIVE_SCAN_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterables/inclusive_scan.hpp>
#include <Lz/detail/traits.hpp>

namespace lz {
namespace detail {
struct inclusive_scan_adaptor {
    using adaptor = inclusive_scan_adaptor;

    // clang-format off

    /**
     * @brief Performs an inclusive scan on a container. The first element will be the result of the binary operation with init value
     * and the first element of the input iterable. The second element will be the previous result + the next value of the input
     * iterable, the third element will be previous result + the next value of the input iterable, etc. It contains a .size() method
     * if the input iterable also has a .size() method. Its end() function returns a sentinel rather than an iterator and its iterator
     * category is forward. Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5 };
     * // std::plus is used as the default binary operation
     * auto scan = lz::inclusive_scan(vec, 0); // scan = { 1, 3, 6, 10, 15 }
     * // so 0 + 1 (= 1), 1 + 2 (= 3), 3 + 3 (= 6), 6 + 4 (= 10), 10 + 5 (= 15)
     *
     * // or
     * auto scan = vec | lz::inclusive_scan(0); // scan = { 1, 3, 6, 10, 15 }
     * // you can also add a custom operator:
     * auto scan = vec | lz::inclusive_scan(0, std::plus<int>{}); // scan = { 1, 3, 6, 10, 15 }
     * // When working with pipe expressions, you always need to specify the init value. When working with 'regular' functions, you
     * // can omit the init value, in which case it will be a default constructed object of the type of the container.
     * // Example
     * auto scan = lz::inclusive_scan(vec); // OK
     * auto scan = lz::inclusive_scan(vec, 0); // OK
     * // auto scan = vec | lz::inclusive_scan; // Error, unable to determine the type of the init value
     * auto scan = vec | lz::inclusive_scan(0); // OK
     * ```
     * @param iterable The iterable to perform the inclusive scan on.
     * @param init The initial value to start the inclusive scan with.
     * @param binary_op The binary operation to perform on the elements. The default is std::plus.
     */
    template<class Iterable, class T = val_iterable_t<Iterable>, class BinaryOp = MAKE_BIN_PRED(plus)>
    LZ_NODISCARD constexpr 
    enable_if<is_invocable<BinaryOp, decay_t<T>, decay_t<T>>::value, inclusive_scan_iterable<remove_ref<Iterable>, decay_t<T>, BinaryOp>>
    operator()(Iterable&& iterable, T init = {}, BinaryOp binary_op = {}) const {
        return { std::forward<Iterable>(iterable), std::move(init), std::move(binary_op) };
    }

    /**
     * @brief Performs an inclusive scan on a container. The first element will be the result of the binary operation with init value
     * and the first element of the input iterable. The second element will be the previous result + the next value of the input
     * iterable, the third element will be previous result + the next value of the input iterable, etc. It contains a .size() method
     * if the input iterable also has a .size() method. Its end() function returns a sentinel rather than an iterator and its iterator
     * category is forward. Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5 };
     * // std::plus is used as the default binary operation
     * auto scan = lz::inclusive_scan(vec, 0); // scan = { 1, 3, 6, 10, 15 }
     * // so 0 + 1 (= 1), 1 + 2 (= 3), 3 + 3 (= 6), 6 + 4 (= 10), 10 + 5 (= 15)
     *
     * // you can also add a custom operator:
     * auto scan = vec | lz::inclusive_scan(0, std::plus<int>{}); // scan = { 1, 3, 6, 10, 15 }
     * // When working with pipe expressions, you always need to specify the init value. When working with 'regular' functions, you
     * // can omit the init value, in which case it will be a default constructed object of the type of the container.
     * // Example
     * auto scan = lz::inclusive_scan(vec); // OK
     * auto scan = lz::inclusive_scan(vec, 0); // OK
     * // auto scan = vec | lz::inclusive_scan; // Error, unable to determine the type of the init value
     * auto scan = vec | lz::inclusive_scan(0); // OK
     * ```
     * @param init The initial value to start the inclusive scan with.
     * @param binary_op The binary operation to perform on the elements. The default is std::plus.
     */
    template<class T, class BinaryOp = MAKE_BIN_PRED(plus)>
    LZ_NODISCARD constexpr 
    enable_if<is_invocable<BinaryOp, decay_t<T>, decay_t<T>>::value, fn_args_holder<adaptor, decay_t<T>, BinaryOp>>
    operator()(T&& init, BinaryOp binary_op = {}) const {
        return { std::forward<T>(init), std::move(binary_op) };
    }

    // clang-format on
};
} // namespace detail
} // namespace lz

#endif