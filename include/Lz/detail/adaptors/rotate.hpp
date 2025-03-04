#pragma once

#ifndef LZ_ROTATE_ADAPTOR_HPP
#define LZ_ROTATE_ADAPTOR_HPP

#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterables/rotate.hpp>

namespace lz {
namespace detail {
struct rotate_adaptor {
    using adaptor = rotate_adaptor;

#ifdef LZ_HAS_CXX_11

    static constexpr adaptor rotate{};

#endif

    /**
     * @brief Rotates the input iterable by n elements. Contains a .size() method if the input iterable also has a .size() method.
     * Its iterator category is the same as the input iterable if it has a .size() method. If the input iterable does not have a
     * .size() method, it will return the end type of its input iterable, rather than a rotate_iterator. Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5 };
     * auto rotated = lz::rotate(vec, 2); // rotated = { 3, 4, 5, 1, 2 }
     *
     * // or, in case input iterable does not have .size() method:
     * auto str = lz::c_string("Hello, World!");
     * auto rotated = lz::rotate(str, 7); // rotated = "World!Hello, "
     * // rotated.end() will be a sentinel, rather than an actual iterator, as rotated str does not have a .size() method
     *
     * // Invalid: filter decays into a forward iterator because it doesn't have a .size() method
     * // auto rotated =
     *         vec | lz::filter([](int i) { return i % 2 == 0; }) | lz::rotate(2) | lz::reverse; // Error, rotated is forward
     *
     * // Instead, do (first rotate, then filter, instead of filter then rotate):
     * auto rotated = vec | lz::rotate(2) | lz::filter([](int i) { return i % 2 == 0; }) | lz::reverse | lz::to<std::vector>();
     * // rotated = { 2, 4 }
     * ```
     * @param iterable The input iterable to rotate.
     * @param start The amount of elements to rotate the input iterable by.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD constexpr rotate_iterable<Iterable>
    operator()(Iterable&& iterable, const diff_iterable_t<Iterable> start) const {
        return { std::forward<Iterable>(iterable), start };
    }

    /**
     * @brief Rotates the input iterable by n elements. Contains a .size() method if the input iterable also has a .size() method.
     * Its iterator category is the same as the input iterable if it has a .size() method. If the input iterable does not have a
     * .size() method, it will return the end type of its input iterable, rather than a rotate_iterator. Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5 };
     * auto rotated = vec | lz::rotate(2); // rotated = { 3, 4, 5, 1, 2 }
     * // or, in case input iterable does not have .size() method:
     *
     * auto str = lz::c_string("Hello, World!");
     * auto rotated = lz::rotate(str, 7); // rotated = "World!Hello, "
     * // rotated.end() will be a sentinel, rather than an actual iterator, as rotated str does not have a .size() method
     *
     * // Invalid: filter decays into a forward iterator because it doesn't have a .size() method
     * // auto rotated =
     *         vec | lz::filter([](int i) { return i % 2 == 0; }) | lz::rotate(2) | lz::reverse; // Error, rotated is forward
     *
     * // Instead, do (first rotate, then filter, instead of filter then rotate):
     * auto rotated = vec | lz::rotate(2) | lz::filter([](int i) { return i % 2 == 0; }) | lz::reverse | lz::to<std::vector>();
     * // rotated = { 2, 4 }
     * ```
     * @param start The amount of elements to rotate the input iterable by.
     */
    template<class DiffT>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, DiffT> operator()(const DiffT start) const {
        return { start };
    }
};
} // namespace detail
} // namespace lz

#endif
