#pragma once

#ifndef LZ_INCLUSIVE_SCAN_HPP
#define LZ_INCLUSIVE_SCAN_HPP

#include <Lz/detail/basic_iterable.hpp>
#include <Lz/detail/iterators/inclusive_scan.hpp>

namespace lz {
LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterator, class S, class T, class BinaryOp>
class inclusive_scan_iterable final
    : public detail::basic_iterable<detail::inclusive_scan_iterator<Iterator, S, T, BinaryOp>, default_sentinel> {
public:
    using iterator = detail::inclusive_scan_iterator<Iterator, S, T, BinaryOp>;
    using const_iterator = iterator;

    constexpr inclusive_scan_iterable() = default;

    LZ_CONSTEXPR_CXX_14 inclusive_scan_iterable(Iterator begin, S end, T init, BinaryOp binary_op) :
        detail::basic_iterable<iterator, default_sentinel>(
            iterator(std::move(begin), std::move(end), std::move(init), std::move(binary_op))) {
    }
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief Returns an inclusive scan iterator.
 * @details Returns an inclusive scan iterator. This iterator begins by returning `*std::begin(iterable)`. It then
 * proceeds to the next one, which is essentially the previously calculated value (calculated by `binary_op(std::move(current),
 * *it)`
 * + the current iterator element being handled. Example:
 * @example
 * ```cpp
 * int array[] = {3, 5, 2, 3, 4, 2, 3};
 * // start the scan from 3
 * auto scan = lz::inclusive_scan(array);
 *
 * for (const int& i : scan) {
 *     fmt::print("{} ", i);
 * }
 * // prints 3 8 10 13 17 19 22
 * ```
 *
 * @param iterable The iterable to perform the inclusive scan over
 * @param init The value to start with
 * @param binary_op The fold function. Essentially, it is executed as (`init = binary_op(std::move(init), *iterator);`)
 * @return An inclusive scan view object.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class T = val_iterable_t<Iterable>,
         class BinaryOp = MAKE_BIN_PRED(std::plus, val_iterable_t<Iterable>)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 inclusive_scan_iterable<iter_t<Iterable>, sentinel_t<Iterable>, T, BinaryOp>
inclusive_scan(Iterable&& iterable, T init = {}, BinaryOp binary_op = {}) {
    if (lz::empty(iterable)) {
        return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)), std::move(init),
                 std::move(binary_op) };
    }
    auto begin = detail::begin(std::forward<Iterable>(iterable));
    auto tmp = binary_op(std::move(init), *begin);
    return { std::move(begin), detail::end(std::forward<Iterable>(iterable)), std::move(tmp), std::move(binary_op) };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END
} // namespace lz

#endif