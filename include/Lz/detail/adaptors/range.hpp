#pragma once

#ifndef LZ_RANGE_ADAPTOR_HPP
#define LZ_RANGE_ADAPTOR_HPP

#include <Lz/detail/iterables/range.hpp>

namespace lz {
namespace detail {
struct range_adaptor {
    using adaptor = range_adaptor;

    template<class Arithmetic>
    LZ_NODISCARD constexpr range_iterable<Arithmetic>
    operator()(const Arithmetic start, const Arithmetic end, const Arithmetic step = 1) const noexcept {
        return { start, end, step };
    }

    template<class Arithmetic>
    LZ_NODISCARD constexpr range_iterable<Arithmetic>
    operator()(const Arithmetic end) const noexcept {
        return { 0, end, 1 };
    }
};
} // namespace detail
} // namespace lz

#endif