#pragma once

#ifndef LZ_RANGE_ADAPTOR_HPP
#define LZ_RANGE_ADAPTOR_HPP

#include <Lz/detail/iterables/range.hpp>

namespace lz {
namespace detail {
struct range_adaptor {
    using adaptor = range_adaptor;

    /**
     * @brief Creates n amount of numbers starting from 0 (or specified otherwise). It contains a
     * .size() method and is random access. Example:
     * ```
     * auto range = lz::range(0.0, 1.0, 0.1); // {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9} (T = double)
     * auto range = lz::range(0.0, 1.0, 0.2); // {0.0, 0.2, 0.4, 0.6, 0.8} (T = double)
     * auto range = lz::range(0, 10); // {0, 1, 2, 3, 4, 5, 6, 7, 8, 9} (T = int)
     * auto range = lz::range(0, 10, 2) | lz::reverse; // {8, 6, 4, 2, 0} (T = int)
     * ```
     * @param start The start of the range
     * @param end The end of the range (exclusive)
     * @param step The step of the range
     * @return A range_iterable that can be used in range-based for loops or with other algorithms
     */
    template<class Arithmetic>
    LZ_NODISCARD constexpr range_iterable<Arithmetic>
    operator()(const Arithmetic start, const Arithmetic end, const Arithmetic step = 1) const noexcept {
        return { start, end, step };
    }

    /**
     * @brief Creates n amount of numbers starting from 0 (or specified otherwise). It contains a
     * .size() method and is random access. Example:
     * ```
     * // This will create a range from 0 to 4
     * auto range = lz::range(5); // {0, 1, 2, 3, 4} (T = int)
     * ```
     * @param end The end of the range (exclusive)
     * @return A range_iterable that can be used in range-based for loops or with other algorithms
     */
    template<class Arithmetic>
    LZ_NODISCARD constexpr range_iterable<Arithmetic>
    operator()(const Arithmetic end) const noexcept {
        return { 0, end, 1 };
    }
};
} // namespace detail
} // namespace lz

#endif