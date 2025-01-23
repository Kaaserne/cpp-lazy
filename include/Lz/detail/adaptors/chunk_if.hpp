#pragma once

#ifndef LZ_CHUNK_IF_ADAPTOR_HPP
#define LZ_CHUNK_IF_ADAPTOR_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterables/chunk_if.hpp>

namespace lz {
namespace detail {
template<class ValueType>
struct chunk_if_adaptor {
#ifdef LZ_HAS_CXX_11
    static chunk_if_adaptor<ValueType> chunk_if;
#endif

    using adaptor = chunk_if_adaptor<ValueType>;

    template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
    constexpr detail::chunk_if_iterable<ValueType, Iterable, detail::decay_t<UnaryPredicate>>
    operator()(Iterable&& iterable, UnaryPredicate&& predicate) const {
        return { std::forward<Iterable>(iterable), std::forward<UnaryPredicate>(predicate) };
    }

    template<class UnaryPredicate>
    constexpr detail::fn_args_holder<adaptor, detail::decay_t<UnaryPredicate>> operator()(UnaryPredicate&& predicate) const {
        return { std::forward<UnaryPredicate>(predicate) };
    }
};

template<>
struct chunk_if_adaptor<void> {
#ifdef LZ_HAS_CXX_11
    static chunk_if_adaptor<void> chunk_if;
#endif

    using adaptor = chunk_if_adaptor<void>;

    template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
    constexpr detail::chunk_if_iterable<basic_iterable<iter_t<Iterable>>, Iterable, detail::decay_t<UnaryPredicate>>
    operator()(Iterable&& iterable, UnaryPredicate&& predicate) const {
        return { std::forward<Iterable>(iterable), std::forward<UnaryPredicate>(predicate) };
    }

    template<class UnaryPredicate>
    constexpr detail::fn_args_holder<adaptor, detail::decay_t<UnaryPredicate>> operator()(UnaryPredicate&& predicate) const {
        return { std::forward<UnaryPredicate>(predicate) };
    }
};
} // namespace detail
} // namespace lz

#endif