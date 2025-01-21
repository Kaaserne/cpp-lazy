// #pragma once

// #ifndef LZ_EXCLUSIVE_SCAN_HPP
// #define LZ_EXCLUSIVE_SCAN_HPP

// #include <Lz/basic_iterable.hpp>
// #include <Lz/detail/iterators/exclusive_scan.hpp>
// #include <Lz/detail/traits.hpp>

// namespace lz {
// LZ_MODULE_EXPORT_SCOPE_BEGIN

// template<class Iterator, class S, class T, class BinaryOp>
// class exclusive_scan_iterable final
//     : public detail::basic_iterable<detail::exclusive_scan_iterator<Iterator, S, T, BinaryOp>, default_sentinel> {
// public:
//     using iterator = detail::exclusive_scan_iterator<Iterator, S, T, BinaryOp>;
//     using const_iterator = iterator;

//     constexpr exclusive_scan_iterable() = default;

//     constexpr exclusive_scan_iterable(Iterator first, S last, T init, BinaryOp binary_op) :
//         detail::basic_iterable<iterator, default_sentinel>(
//             iterator(std::move(first), std::move(last), std::move(init), std::move(binary_op))) {
//     }
// };

// /**
//  * @addtogroup ItFns
//  * @{
//  */

// // clang-format off

// /**
//  * Returns an exclusive scan iterator. This iterator begins by returning @p init. It then uses @p binary_op to calculate
//  * the next value, which is essentially @p init (the previously calculated value) + the current iterator element being handled.
//  * Example:
//  * ```cpp
//  * int array[] = {3, 5, 2, 3, 4, 2, 3};
//  * // start the scan from 0
//  * auto scan = lz::exclusive_scan(array, 0);
//  *
//  * for (const int& i : scan) {
//  *     fmt::print("{} ", i);
//  * }
//  * // prints 0 3 8 10 13 17 19
//  * ```
//  *
//  * @param iterable The iterable to perform the exclusive scan over
//  * @param init The value to start with
//  * @param binary_op The fold function. Essentially, it is executed as (`init = binary_op(std::move(init), *iterator);`)
//  * @return An exclusive scan view object.
//  */
// template<LZ_CONCEPT_ITERABLE Iterable, class T = val_iterable_t<Iterable>,
//          class BinaryOp = MAKE_BIN_PRED(std::plus, detail::decay_t<T>)>
// LZ_NODISCARD constexpr
// exclusive_scan_iterable<iter_t<Iterable>, sentinel_t<Iterable>, detail::decay_t<T>, detail::decay_t<BinaryOp>>
// exclusive_scan(Iterable&& iterable, T&& init = {}, BinaryOp&& binary_op = {}) {
//     return { std::forward<Iterable>(iterable).begin()), std::forward<Iterable>(iterable).end(),
//              std::forward<T>(init), std::forward<BinaryOp>(binary_op) };
// }
// // clang-format on
// // End of group
// /**
//  * @}
//  */

// LZ_MODULE_EXPORT_SCOPE_END
// } // namespace lz

// #endif