#pragma once

#ifndef LZ_INTERSECTION_ADAPTOR_HPP
#define LZ_INTERSECTION_ADAPTOR_HPP

#include <Lz/detail/iterables/intersection.hpp>
#include <Lz/detail/adaptors/fn_args_holder.hpp>
// TODO
namespace lz {
namespace detail {
struct intersection_adaptor {
    template<class Iterable, class Iterable2, class Compare>
    LZ_NODISCARD constexpr intersection_iterable<Iterable, Iterable2, Compare>
    operator()(Iterable&& iterable, Iterable2&& iterable2, Compare compare) {
        return { std::forward<Iterable>(iterable), std::forward<Iterable2>(iterable2), std::move(compare) };
    }
};


} // namespace detail
} // namespace lz

#endif
