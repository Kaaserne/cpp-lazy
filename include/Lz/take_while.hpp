#pragma once

#ifndef LZ_TAKE_WHILE_HPP
#define LZ_TAKE_WHILE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/iterators/take_while.hpp>

namespace lz {
LZ_MODULE_EXPORT_SCOPE_BEGIN

struct take_while_adaptor {
#ifdef LZ_HAS_CXX_11
    static take_while_adaptor take_while;
#endif

    using adaptor = take_while_adaptor;

    // TODO add LZ_CONCEPT_ITERABLE to all adaptors
    template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
    LZ_NODISCARD constexpr detail::take_while_iterable<Iterable, detail::decay_t<UnaryPredicate>>
    operator()(Iterable&& iterable, UnaryPredicate&& unary_predicate) const {
        return { std::forward<Iterable>(iterable), std::forward<UnaryPredicate>(unary_predicate) };
    }

    template<class UnaryPredicate>
    LZ_NODISCARD constexpr detail::fn_args_holder<adaptor, detail::decay_t<UnaryPredicate>>
    operator()(UnaryPredicate&& unary_predicate) const {
        return { *this, std::forward<UnaryPredicate>(unary_predicate) };
    }
};

#ifdef LZ_HAS_CXX_11

take_while_adaptor take_while_adaptor::take_while{};

#else

LZ_INLINE_VAR constexpr take_while_adaptor take_while{};

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_TAKE_WHILE_HPP