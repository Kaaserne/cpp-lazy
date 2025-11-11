#pragma once

#ifndef LZ_DETAIL_TRAITS_INDEX_SEQUENCE_HPP
#define LZ_DETAIL_TRAITS_INDEX_SEQUENCE_HPP

#include <Lz/detail/compiler_config.hpp>

#ifndef LZ_HAS_CXX_11
#include <utility>
#endif

namespace lz {
namespace detail {

#ifdef LZ_HAS_CXX_11

template<size_t... Is>
struct index_sequence {
    using type = index_sequence;
};

template<size_t N>
struct make_index_sequence_impl {
private:
    using left = typename make_index_sequence_impl<N / 2>::type;
    using right = typename make_index_sequence_impl<N - N / 2>::type;

    template<class L, class R>
    struct concat;

    template<size_t... Ls, size_t... Rs>
    struct concat<index_sequence<Ls...>, index_sequence<Rs...>> {
        using type = index_sequence<Ls..., (Rs + sizeof...(Ls))...>;
    };

public:
    using type = typename concat<left, right>::type;
};

template<>
struct make_index_sequence_impl<0> {
    using type = index_sequence<>;
};

template<>
struct make_index_sequence_impl<1> {
    using type = index_sequence<0>;
};

template<size_t N>
using make_index_sequence = typename make_index_sequence_impl<N>::type;

#else // ^^^ has cxx 11 vvv cxx > 11

template<size_t... N>
using index_sequence = std::index_sequence<N...>;

template<size_t N>
using make_index_sequence = std::make_index_sequence<N>;

#endif // LZ_HAS_CXX_11

} // namespace detail
} // namespace lz

#endif // LZ_DETAIL_TRAITS_INDEX_SEQUENCE_HPP
