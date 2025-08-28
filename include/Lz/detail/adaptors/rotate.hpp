#pragma once

#ifndef LZ_ROTATE_ADAPTOR_HPP
#define LZ_ROTATE_ADAPTOR_HPP

#include <Lz/detail/iterables/rotate.hpp>

namespace lz {
namespace detail {
struct rotate_adaptor {
    using adaptor = rotate_adaptor;

    /**
     * @brief Rotates the input iterable by n elements.  If `n` is greater than or equal to its size then the iterable will be
     * empty. Contains a .size() method if the input iterable also has a .size() method. Its iterator category is the same as the
     * input iterable. If the input iterable is forward or has a sentinel, it will return the end type of its input iterable,
     * rather than a rotate_iterator/default_sentinel_t. If n == iterable.size() then calling begin()/end() is undefined. Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5 };
     * auto rotated = lz::rotate(vec, 2); // rotated = { 3, 4, 5, 1, 2 }
     * auto rotated = lz::rotate(vec, 50); // rotated = {}
     *
     * // or, in case input iterable is not forward:
     * auto str = lz::c_string("Hello, World!");
     * auto rotated = lz::rotate(str, 7); // rotated = "World!Hello, "
     * // rotated.end() will be a sentinel, rather than an actual iterator, as rotated str is forward
     * ```
     * @param iterable The input iterable to rotate.
     * @param start The amount of elements to rotate the input iterable by.
     * @return A rotate_iterable that can be used to iterate over the rotated elements.
     */
    template<class Iterable>
    LZ_NODISCARD constexpr rotate_iterable<remove_ref_t<Iterable>>
    operator()(Iterable&& iterable, const diff_iterable_t<Iterable> start) const {
        return { std::forward<Iterable>(iterable), start };
    }

    /**
     * @brief Rotates the input iterable by n elements.  If `n` is greater than or equal to its size then the iterable will be
     * empty. Contains a .size() method if the input iterable also has a .size() method. Its iterator category is the same as the
     * input iterable. If the input iterable is forward or has a sentinel, it will return the end type of its input iterable,
     * rather than a rotate_iterator/default_sentinel_t. If n == iterable.size() then calling begin()/end() is undefined. Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5 };
     * auto rotated = vec | lz::rotate(2); // rotated = { 3, 4, 5, 1, 2 }
     * auto rotated = vec | lz::rotate(50); // rotated = {}
     *
     * // or, in case input iterable is not forward:
     * auto str = lz::c_string("Hello, World!");
     * auto rotated = str | lz::rotate(7); // rotated = "World!Hello, "
     * // rotated.end() will be a sentinel, rather than an actual iterator, as rotated str is forward
     * ```
     * @param start The amount of elements to rotate the input iterable by.
     * @return An adaptor that can be used in pipe expressions
     */
    template<class DiffT>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, DiffT> operator()(const DiffT start) const {
        return { start };
    }
};
} // namespace detail
} // namespace lz

#endif
