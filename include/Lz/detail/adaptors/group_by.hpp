#pragma once

#ifndef LZ_GROUP_BY_ADAPTORS_HPP
#define LZ_GROUP_BY_ADAPTORS_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/iterables/group_by.hpp>

namespace lz {
namespace detail {
struct group_by_adaptor {
    using adaptor = group_by_adaptor;

    template<class Iterable, class BinaryPredicate>
    LZ_NODISCARD constexpr group_by_iterable<Iterable, decay_t<BinaryPredicate>>
    operator()(Iterable&& iterable, BinaryPredicate binary_predicate) const {
        return { std::forward<Iterable>(iterable), std::move(binary_predicate) };
    }

    template<class BinaryPredicate>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, decay_t<BinaryPredicate>>
    operator()(BinaryPredicate binary_predicate) const {
        return { std::move(binary_predicate) };
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_GROUP_BY_ADAPTORS_HPP
