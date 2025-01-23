#pragma once

#ifndef LZ_COMMON_ADAPTOR_HPP
#define LZ_COMMON_ADAPTOR_HPP

#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterables/common.hpp>

namespace lz {
namespace detail {
struct common_adaptor {
#ifdef LZ_HAS_CXX_11
    static common_adaptor common;
#endif

    using adaptor = common_adaptor;

    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 common_iterable<Iterable> operator()(Iterable&& iterable) const {
        static_assert(has_sentinel<Iterable>::value, "Iterator and Sentinel must be different types");
        return { std::forward<Iterable>(iterable) };
    }
};
} // namespace detail
} // namespace lz

#endif