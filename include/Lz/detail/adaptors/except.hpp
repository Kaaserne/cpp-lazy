#pragma once

#ifndef LZ_EXCEPT_ADAPTOR_HPP
#define LZ_EXCEPT_ADAPTOR_HPP

#include <Lz/detail/iterables/except.hpp>
#include <Lz/detail/traits.hpp>
#include <Lz/detail/adaptors/fn_args_holder.hpp>

namespace lz {
namespace detail {
struct except_adaptor {
    using adaptor = except_adaptor;

    template<LZ_CONCEPT_ITERABLE Iterable1, LZ_CONCEPT_ITERABLE Iterable2, 
             class BinaryPredicate = MAKE_BIN_PRED(std::equal_to, val_iterable_t<Iterable1>)>
    LZ_NODISCARD constexpr 
    enable_if<is_iterable<Iterable2>::value, except_iterable<Iterable1, Iterable2, BinaryPredicate>>
    operator()(Iterable1&& iterable1, Iterable2&& iterable2, BinaryPredicate binary_predicate = {}) const {
        return { std::forward<Iterable1>(iterable1), std::forward<Iterable2>(iterable2), std::move(binary_predicate) };
    }

    template<LZ_CONCEPT_ITERABLE Iterable2, class BinaryPredicate = MAKE_BIN_PRED(std::equal_to, val_iterable_t<Iterable1>)>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14
    enable_if<!is_iterable<BinaryPredicate>::value, fn_args_holder<adaptor, Iterable2, BinaryPredicate>>
    operator()(Iterable2&& iterable2, BinaryPredicate binary_predicate = {}) const {
        return { std::forward<Iterable2>(iterable2), std::move(binary_predicate) };
    }

};
} // namespace detail
} // namespace lz

#endif // LZ_EXCEPT_ADAPTOR_HPP