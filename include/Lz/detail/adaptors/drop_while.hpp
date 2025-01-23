#pragma once

#ifndef LZ_DROP_WHILE_ADAPTOR_HPP
#define LZ_DROP_WHILE_ADAPTOR_HPP

#include <Lz/detail/iterables/drop_while.hpp>
#include <Lz/detail/adaptors/fn_args_holder.hpp>

namespace lz {
namespace detail {
struct drop_while_adaptor {
#ifdef LZ_HAS_CXX_11
    static drop_while_adaptor drop_while;
#endif

    using adaptor = drop_while_adaptor;

    template<class Iterable, class UnaryPredicate>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 drop_while_iterable<Iterable, decay_t<UnaryPredicate>>
    operator()(Iterable&& iterable, UnaryPredicate&& unaryPredicate) const {
        return { std::forward<Iterable>(iterable), std::forward<UnaryPredicate>(unaryPredicate) };
    }

    template<class UnaryPredicate>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, decay_t<UnaryPredicate>>
    operator()(UnaryPredicate&& unaryPredicate) const {
        return { std::forward<UnaryPredicate>(unaryPredicate) };
    }
};
} // namespace detail
} // namespace lz
#endif