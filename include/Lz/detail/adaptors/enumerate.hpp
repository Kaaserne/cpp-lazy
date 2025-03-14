#pragma once

#ifndef LZ_ENUMERATE_ADAPTOR_HPP
#define LZ_ENUMERATE_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterables/enumerate.hpp>

namespace lz {
namespace detail {
struct enumerate_adaptor {
    using adaptor = enumerate_adaptor;

#ifdef LZ_HAS_CXX_11

    static constexpr adaptor enumerate{};

#endif

    /**
     * @brief Returns an iterable that enumerates the elements of the input iterable, meaning it returns a std::pair<IntType,
     * ValueType>, where std::pair::first_type is an integer (corresponding to the current index) and std::pair::second_type is
     * the value of the input iterable (by reference). The index starts at 0 by default, but can be changed by passing a start
     * value to the function. If the input iterable is exactly bidirectional and not sized (like `lz::filter` for example), the
     * entire sequence is traversed to get its end size (using `lz::eager_size`), so it may be worth your while to use
     * `lz::cache_size`. It will return a `default_sentinel` if the input iterable is forward. The iterator category is the same
     * as its input iterable. Example:
     * ```cpp
     * std::forward_list<int> list = {1, 2, 3, 4, 5};
     * auto enumerated = lz::enumerate(list); // enumerated = { {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5} }
     *
     * // or
     *
     * auto enumerated = list | lz::enumerate; // enumerated = { {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5} }
     * ```
     * @param iterable The iterable to enumerate
     * @param start The start index of the enumeration
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class IntType = int>
    LZ_NODISCARD constexpr enable_if<is_iterable<Iterable>::value, enumerate_iterable<Iterable, IntType>>
    operator()(Iterable&& iterable, const IntType start = 0) const {
        return { std::forward<Iterable>(iterable), start };
    }

    /**
     * @brief Returns an iterable that enumerates the elements of the input iterable, meaning it returns a std::pair<IntType,
     * ValueType>, where std::pair::first_type is an integer (corresponding to the current index) and std::pair::second_type is
     * the value of the input iterable (by reference). The index starts at 0 by default, but can be changed by passing a start
     * value to the function. If the input iterable is exactly bidirectional and not sized (like `lz::filter` for example), the
     * entire sequence is traversed to get its end size (using `lz::eager_size`), so it may be worth your while to use
     * `lz::cache_size`. It will return a `default_sentinel` if the input iterable is forward. The iterator category is the same
     * as its input iterable. Example:
     * ```cpp
     * std::forward_list<int> list = {1, 2, 3, 4, 5};
     * auto enumerated = lz::enumerate(list); // enumerated = { {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5} }
     *
     * // or
     *
     * auto enumerated = list | lz::enumerate; // enumerated = { {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5} }
     * ```
     * @param start The start index of the enumeration
     */
    template<class IntType = int>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<std::is_arithmetic<IntType>::value, fn_args_holder<adaptor, IntType>>
    operator()(const IntType start = 0) const {
        return { start };
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_ENUMERATE_ADAPTOR_HPP