#pragma once

#ifndef LZ_UNIQUE_ADPTOR_HPP
#define LZ_UNIQUE_ADPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/iterables/unique.hpp>

namespace lz {
namespace detail {
struct unique_adaptor {
    using adaptor = unique_adaptor;

    template<class Iterable, class BinaryPredicate = MAKE_BIN_PRED(less)>
    LZ_NODISCARD constexpr enable_if<is_iterable<Iterable>::value, unique_iterable<Iterable, BinaryPredicate>>
    operator()(Iterable&& iterable, BinaryPredicate predicate = {}) const {
        return { std::forward<Iterable>(iterable), std::move(predicate) };
    }

    // TODO add docs for all adaptors
    template<class BinaryPredicate = MAKE_BIN_PRED(less)>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_iterable<BinaryPredicate>::value, fn_args_holder<adaptor, BinaryPredicate>>
    operator()(BinaryPredicate predicate = {}) const {
        return { std::move(predicate) };
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_UNIQUE_ADPTOR_HPP
