#pragma once

#ifndef LZ_DROP_WHILE_HPP
#define LZ_DROP_WHILE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/iterators/drop_while.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

struct drop_while_adaptor {
#ifdef LZ_HAS_CXX_11
    static drop_while_adaptor drop_while;
#endif

    using adaptor = drop_while_adaptor;

    template<class Iterable, class UnaryPredicate>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::drop_while_iterable<Iterable, detail::decay_t<UnaryPredicate>>
    operator()(Iterable&& iterable, UnaryPredicate&& unaryPredicate) const {
        return { std::forward<Iterable>(iterable), std::forward<UnaryPredicate>(unaryPredicate) };
    }

    template<class UnaryPredicate>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::fn_args_holder<adaptor, detail::decay_t<UnaryPredicate>>
    operator()(UnaryPredicate&& unaryPredicate) const {
        return { std::forward<UnaryPredicate>(unaryPredicate) };
    }
};

#ifdef LZ_HAS_CXX_11

drop_while_adaptor drop_while_adaptor::drop_while{};

#else

LZ_INLINE_VAR constexpr drop_while_adaptor drop_while{};

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_DROP_WHILE_HPP