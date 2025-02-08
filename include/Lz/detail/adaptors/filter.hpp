#pragma once

#ifndef LZ_FILTER_ADAPTOR_HPP
#define LZ_FILTER_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/iterables/filter.hpp>

namespace lz {
namespace detail {
struct filter_adaptor {
    using adaptor = filter_adaptor;

    template<class Iterable, class UnaryPredicate>
    LZ_NODISCARD constexpr filter_iterable<Iterable, UnaryPredicate>
    operator()(Iterable&& iterable, UnaryPredicate predicate) const {
        return { std::forward<Iterable>(iterable), std::move(predicate) };
    }

    template<class UnaryPredicate>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, UnaryPredicate> operator()(UnaryPredicate predicate) const {
        return { std::move(predicate) };
    }
};
} // namespace detail
} // namespace lz

#endif
