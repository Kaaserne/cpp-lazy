#pragma once

#ifndef LZ_TAKE_WHILE_ADAPTOR
#define LZ_TAKE_WHILE_ADAPTOR


#include <Lz/detail/iterables/take_while.hpp>
#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/concepts.hpp>

namespace lz {
namespace detail {
struct take_while_adaptor {
    using adaptor = take_while_adaptor;

    // TODO add LZ_CONCEPT_ITERABLE to all adaptors
    template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
    LZ_NODISCARD constexpr take_while_iterable<Iterable, decay_t<UnaryPredicate>>
    operator()(Iterable&& iterable, UnaryPredicate&& unary_predicate) const {
        return { std::forward<Iterable>(iterable), std::forward<UnaryPredicate>(unary_predicate) };
    }

    template<class UnaryPredicate>
    LZ_NODISCARD constexpr fn_args_holder<adaptor, decay_t<UnaryPredicate>>
    operator()(UnaryPredicate&& unary_predicate) const {
        return { *this, std::forward<UnaryPredicate>(unary_predicate) };
    }
};

} // namespace detail
} // namespace lz
#endif // LZ_TAKE_WHILE_ADAPTOR