#pragma once

#ifndef LZ_EXCLUDE_ADAPTOR_HPP
#define LZ_EXCLUDE_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/iterables/exclude.hpp>

namespace lz {
namespace detail {
struct exclude_adaptor {
    using adaptor = exclude_adaptor;

    template<class Iterable, class DiffType>
    LZ_NODISCARD constexpr exclude_iterable<Iterable>
    operator()(Iterable&& iterable, DiffType from, DiffType to) const {
        return { std::forward<Iterable>(iterable), from, to };
    }

    template<class DiffType>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, DiffType, DiffType> operator()(DiffType from, DiffType to) const {
        return { from, to };
    }
};
} // namespace detail
} // namespace lz

#endif
