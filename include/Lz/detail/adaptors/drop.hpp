#pragma once

#ifndef LZ_DROP_ADAPTOR_HPP
#define LZ_DROP_ADAPTOR_HPP

#include <Lz/detail/iterables/drop.hpp>
#include <Lz/detail/concepts.hpp>
#include <Lz/detail/adaptors/fn_args_holder.hpp>

namespace lz {
namespace detail {
struct drop_adaptor {
    using adaptor = drop_adaptor;

    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 drop_iterable<Iterable>
    operator()(Iterable&& iterable, const diff_iterable_t<Iterable> n) const {
        return { std::forward<Iterable>(iterable), n };
    }

    template<class DiffT>
    LZ_NODISCARD constexpr fn_args_holder<adaptor, DiffT> operator()(const DiffT n) const {
        return { n };
    }
};
} // namespace lz
} // namespace detail

#endif