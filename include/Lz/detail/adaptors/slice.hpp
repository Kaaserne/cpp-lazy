#pragma once

#ifndef LZ_SLICE_ADAPTOR_HPP
#define LZ_SLICE_ADAPTOR_HPP

#include <Lz/take.hpp>
#include <Lz/drop.hpp>

namespace lz {
namespace detail {
struct slice_adaptor {
    using adaptor = slice_adaptor;

    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD constexpr detail::take_iterable<detail::drop_iterable<Iterable>>
    operator()(Iterable&& iterable, const diff_iterable_t<Iterable> from, const diff_iterable_t<Iterable> to) const {
        return { lz::drop(std::forward<Iterable>(iterable), from), to - from };
    }

    template<class DiffT>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::fn_args_holder<adaptor, DiffT, DiffT>
    operator()(const DiffT from, const DiffT to) const {
        return { from, to };
    }
};
} // namespace detail
} // namespace lz

#endif
