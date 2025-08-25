#pragma once

#ifndef LZ_FLATTEN_ADAPTOR_HPP
#define LZ_FLATTEN_ADAPTOR_HPP

#include <Lz/detail/iterables/flatten.hpp>

namespace lz {

template<class, class = void>
struct dimensions;

/**
 * @brief Gets the number of dimensions of an iterable. For instance, a vector of vectors will return 2, a vector of vectors of
 * vectors will return 3, etc. Example:
 * ```cpp
 * std::vector<std::vector<int>> vectors = { { 1, 2, 3 }, { 4, 5, 6 }, { 7 } };
 * auto dim = lz::dimensions<decltype(vectors)>::value; // 2
 * ```
 * @tparam Iterable The iterable type to get the dimensions of.
 */
template<class Iterable>
struct dimensions<Iterable, detail::enable_if_t<!std::is_array<Iterable>::value>> : detail::count_dims<Iterable> {};

/**
 * @brief Gets the number of dimensions of an iterable. For instance, a vector of vectors will return 2, a vector of vectors of
 * vectors will return 3, etc. Example:
 * ```cpp
 * int arrs[3][3] = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } };
 * auto dim = lz::dimensions<decltype(arrs)>::value; // 2
 * ```
 * @tparam Iterable The iterable type to get the dimensions of.
 */
template<class Iterable>
struct dimensions<Iterable, detail::enable_if_t<std::is_array<Iterable>::value>>
    : std::integral_constant<size_t, std::rank<detail::remove_cvref_t<Iterable>>::value> {};

#ifdef LZ_HAS_CXX_14

/**
 * @brief Gets the number of dimensions of an iterable. For instance, a vector of vectors will return 2, a vector of vectors of
 * vectors will return 3, etc. Example:
 * ```cpp
 * int arrs[3][3] = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } };
 * auto dim = lz::dimensions_v<decltype(arrs)>; // 2
 * std::vector<std::vector<int>> vectors = { { 1, 2, 3 }, { 4, 5, 6 }, { 7 } };
 * auto dim2 = lz::dimensions_v<decltype(vectors)>; // 2
 * ```
 * @tparam Iterable The iterable type to get the dimensions of.
 */
template<class Iterable>
LZ_INLINE_VAR constexpr size_t dimensions_v = dimensions<Iterable>::value;

#endif

namespace detail {
struct flatten_adaptor {
    using adaptor = flatten_adaptor;

    // clang-format off

    /**
     * @brief Flattens a nested iterable. For instance a vector of vectors will be flattened to a single iterable. Returns a
     * bidirectional iterable if the input is bidirectional, otherwise the same as the input iterable. If its input iterable is
     * forward or less, the end iterator will be a sentinel. Contains a .size() method if all the input iterables are sized. Example:
     * ```cpp
     * std::vector<std::vector<int>> vectors = { { 1, 2, 3 }, { 4, 5, 6 }, { 7 } };
     * auto flattened = lz::flatten(vectors); // { 1, 2, 3, 4, 5, 6, 7 }
     * // or
     * auto flattened = vectors | lz::flatten; // { 1, 2, 3, 4, 5, 6, 7 }
     * ```
     * @param iterable The iterable(s) to flatten
     * @return An iterable that is flattened, with the same type as the input iterable.
     */
    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14
    flatten_iterable<remove_ref_t<Iterable>, dimensions<remove_ref_t<Iterable>>::value - !std::is_array<remove_ref_t<Iterable>>::value>
    operator()(Iterable&& iterable) const {
        using flattener = flatten_iterable<remove_ref_t<Iterable>, 
                                           dimensions<remove_ref_t<Iterable>>::value - !std::is_array<remove_ref_t<Iterable>>::value>;
        using it = iter_t<Iterable>;

        static_assert(std::is_default_constructible<it>::value, "underlying iterator needs to be default constructible");
        static_assert(std::is_copy_assignable<it>::value || std::is_move_assignable<it>::value,
                      "underling iterator needs to be copy or move assignable");

        return flattener{ std::forward<Iterable>(iterable) };
    }

    // clang-format on
};
} // namespace detail
} // namespace lz

#endif
