#pragma once

#ifndef LZ_FLATTEN_ADAPTOR_HPP
#define LZ_FLATTEN_ADAPTOR_HPP

#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterables/flatten.hpp>

namespace lz {

template<class, class = void>
struct dimensions;

template<LZ_CONCEPT_ITERABLE Iterable>
struct dimensions<Iterable, detail::enable_if<!detail::is_c_array<Iterable>::value>> : detail::count_dims<iter_t<Iterable>> {};

template<LZ_CONCEPT_ITERABLE Iterable>
struct dimensions<Iterable, detail::enable_if<detail::is_c_array<Iterable>::value>>
    : std::integral_constant<std::size_t, std::rank<detail::remove_cvref<Iterable>>::value> {};

#ifdef LZ_HAS_CXX_17

template<LZ_CONCEPT_ITERABLE Iterable>
inline constexpr std::size_t dimensions_v = dimensions<Iterable>::value;

#endif

namespace detail {
struct flatten_adaptor {
    using adaptor = flatten_adaptor;

    // clang-format off

    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20
    flatten_iterable<Iterable, dimensions<Iterable>::value - !detail::is_c_array<Iterable>::value>
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
