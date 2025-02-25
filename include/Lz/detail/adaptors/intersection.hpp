#pragma once

#ifndef LZ_INTERSECTION_ADAPTOR_HPP
#define LZ_INTERSECTION_ADAPTOR_HPP

#include <Lz/detail/iterables/intersection.hpp>
#include <Lz/detail/adaptors/fn_args_holder.hpp>

namespace lz {
namespace detail {
struct intersection_adaptor {
    using adaptor = intersection_adaptor;

    template<class Iterable, class Iterable2, class BinaryPredicate = MAKE_BIN_PRED(less)>
    LZ_NODISCARD constexpr enable_if<is_iterable<Iterable2>::value, intersection_iterable<Iterable, Iterable2, BinaryPredicate>>
    operator()(Iterable&& iterable, Iterable2&& iterable2, BinaryPredicate compare = {}) const {
        return { std::forward<Iterable>(iterable), std::forward<Iterable2>(iterable2), std::move(compare) };
    }

    template<class Iterable2, class BinaryPredicate = MAKE_BIN_PRED(less)>
    LZ_NODISCARD constexpr enable_if<!is_iterable<BinaryPredicate>::value, fn_args_holder<adaptor, Iterable2, BinaryPredicate>>
    operator()(Iterable2&& iterable2, BinaryPredicate compare = {}) const {
        return { std::forward<Iterable2>(iterable2), std::move(compare) };
    }
};
} // namespace detail
} // namespace lz

#endif
