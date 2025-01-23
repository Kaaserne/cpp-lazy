#pragma once

#ifndef LZ_RANGE_ADAPTOR_HPP
#define LZ_RANGE_ADAPTOR_HPP

#include <Lz/detail/iterables/range.hpp>

namespace lz {
namespace detail {
struct range_adaptor {
#ifdef LZ_HAS_CXX_11
    static const range_adaptor range;
#endif

    using adaptor = range_adaptor;

    template<class Arithmetic>
    LZ_NODISCARD constexpr range_iterable<Arithmetic>
    range(const Arithmetic start, const Arithmetic end, const Arithmetic step) const noexcept {
        return { start, end, step };
    }

    template<class Arithmetic>
    LZ_NODISCARD constexpr range_iterable<Arithmetic>
    range(const Arithmetic end) const noexcept {
        return { 0, end, 1 };
    }
};
} // namespace detail
} // namespace lz

#endif