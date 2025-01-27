#pragma once

#ifndef LZ_ENUMERATE_ADAPTOR_HPP
#define LZ_ENUMERATE_ADAPTOR_HPP

#include <Lz/detail/iterables/enumerate.hpp>
#include <Lz/detail/adaptors/fn_args_holder.hpp>

namespace lz {
namespace detail {
struct enumerate_adaptor {
    using adaptor = enumerate_adaptor;

    template<LZ_CONCEPT_ITERABLE Iterable, class IntType = int>
    LZ_NODISCARD constexpr enable_if<is_iterable<Iterable>::value, enumerate_iterable<Iterable, IntType>>
    operator()(Iterable&& iterable, const IntType start = 0) const {
        return { std::forward<Iterable>(iterable), start };
    }

    template<class IntType = int>
    LZ_NODISCARD constexpr enable_if<std::is_arithmetic<IntType>::value, fn_args_holder<adaptor, IntType>>
    operator()(const IntType start = 0) const {
        return { start };
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_ENUMERATE_ADAPTOR_HPP