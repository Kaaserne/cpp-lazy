#pragma once

#ifndef LZ_DROP_WHILE_ADAPTOR_HPP
#define LZ_DROP_WHILE_ADAPTOR_HPP

#include <Lz/detail/iterables/drop_while.hpp>
#include <Lz/detail/adaptors/fn_args_holder.hpp>

namespace lz {
namespace detail {
struct drop_while_adaptor {
    using adaptor = drop_while_adaptor;

    template<class Iterable, class UnaryPredicate>
    LZ_NODISCARD constexpr drop_while_iterable<Iterable, UnaryPredicate>
    operator()(Iterable&& iterable, UnaryPredicate unary_predicate) const {
        return { std::forward<Iterable>(iterable), std::move(unary_predicate) };
    }

    template<class UnaryPredicate>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, UnaryPredicate> operator()(UnaryPredicate unary_predicate) const {
        return { std::move(unary_predicate) };
    }
};
} // namespace detail
} // namespace lz
#endif