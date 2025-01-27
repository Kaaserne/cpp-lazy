#pragma once

#ifndef LZ_CHUNK_IF_ADAPTOR_HPP
#define LZ_CHUNK_IF_ADAPTOR_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterables/chunk_if.hpp>
#include <Lz/detail/adaptors/fn_args_holder.hpp>

namespace lz {
namespace detail {
template<class ValueType>
struct chunk_if_adaptor {
    using adaptor = chunk_if_adaptor<ValueType>;

    template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
    constexpr chunk_if_iterable<ValueType, Iterable, decay_t<UnaryPredicate>>
    operator()(Iterable&& iterable, UnaryPredicate&& predicate) const {
        return { std::forward<Iterable>(iterable), std::forward<UnaryPredicate>(predicate) };
    }

    template<class UnaryPredicate>
    constexpr fn_args_holder<adaptor, decay_t<UnaryPredicate>> operator()(UnaryPredicate&& predicate) const {
        return { std::forward<UnaryPredicate>(predicate) };
    }
};

template<>
struct chunk_if_adaptor<void> {
    using adaptor = chunk_if_adaptor<void>;

    template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
    constexpr chunk_if_iterable<basic_iterable<iter_t<Iterable>>, Iterable, decay_t<UnaryPredicate>>
    operator()(Iterable&& iterable, UnaryPredicate&& predicate) const {
        return { std::forward<Iterable>(iterable), std::forward<UnaryPredicate>(predicate) };
    }

    template<class UnaryPredicate>
    constexpr fn_args_holder<adaptor, decay_t<UnaryPredicate>> operator()(UnaryPredicate&& predicate) const {
        return { std::forward<UnaryPredicate>(predicate) };
    }
};
} // namespace detail
} // namespace lz

#endif