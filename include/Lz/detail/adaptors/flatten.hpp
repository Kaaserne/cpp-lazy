#pragma once

#ifndef LZ_FLATTEN_ADAPTOR_HPP
#define LZ_FLATTEN_ADAPTOR_HPP

#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterables/flatten.hpp>

namespace lz {

template<class, class = void>
struct dimensions;

template<LZ_CONCEPT_ITERABLE Iterable>
struct dimensions<Iterable, detail::enable_if<!std::is_array<Iterable>::value>> : detail::count_dims<Iterable> {};

template<LZ_CONCEPT_ITERABLE Iterable>
struct dimensions<Iterable, detail::enable_if<std::is_array<Iterable>::value>>
    : std::integral_constant<std::size_t, std::rank<detail::remove_cvref<Iterable>>::value> {};

#ifdef LZ_HAS_CXX_17

template<LZ_CONCEPT_ITERABLE Iterable>
inline constexpr std::size_t dimensions_v = dimensions<Iterable>::value;

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
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14
    flatten_iterable<remove_ref<Iterable>, dimensions<remove_ref<Iterable>>::value - !std::is_array<remove_ref<Iterable>>::value>
    operator()(Iterable&& iterable) const {
        static_assert(std::is_move_assignable<iter_t<Iterable>>::value || std::is_copy_assignable<iter_t<Iterable>>::value,
                      "underlying iterator needs to be copy or move assignable");
        return { std::forward<Iterable>(iterable) };
    }

    // clang-format on
};
} // namespace detail
} // namespace lz

#endif
