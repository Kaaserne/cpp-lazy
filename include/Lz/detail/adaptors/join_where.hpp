#pragma once

#ifndef LZ_JOIN_WHERE_ADAPTOR_HPP
#define LZ_JOIN_WHERE_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/iterables/join_where.hpp>

namespace lz {
namespace detail {
struct join_where_adaptor {
    using adaptor = join_where_adaptor;

    template<class IterableA, class IterableB, class SelectorA, class SelectorB, class ResultSelector>
    LZ_NODISCARD constexpr join_where_iterable<IterableA, IterableB, SelectorA, SelectorB, ResultSelector>
    operator()(IterableA&& iterable_a, IterableB&& iterable_b, SelectorA a, SelectorB b, ResultSelector result_selector) const {
        return { std::forward<IterableA>(iterable_a), std::forward<IterableB>(iterable_b), std::move(a), std::move(b),
                 std::move(result_selector) };
    }

    template<class IterableB, class SelectorA, class SelectorB, class ResultSelector>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, IterableB, SelectorA, SelectorB, ResultSelector>
    operator()(IterableB&& iterable_b, SelectorA a, SelectorB b, ResultSelector result_selector) const {
        return { std::forward<IterableB>(iterable_b), std::move(a), std::move(b), std::move(result_selector) };
    }
};
} // namespace detail
} // namespace lz

#endif
