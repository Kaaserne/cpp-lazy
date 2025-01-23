#pragma once

#ifndef LZ_MAP_ADAPTOR_HPP
#define LZ_MAP_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/iterables/map.hpp>

namespace lz {
namespace detail {
    struct map_adaptor {
#ifdef LZ_HAS_CXX_11
    static map_adaptor map;
#endif

    using adaptor = map_adaptor;

    template<class Iterable, class Function>
    constexpr map_iterable<Iterable, decay_t<Function>>
    operator()(Iterable&& iterable, Function&& function) const {
        return { std::forward<Iterable>(iterable), std::forward<Function>(function) };
    }

    template<class Function>
    LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, decay_t<Function>> operator()(Function&& function) const {
        return { std::forward<Function>(function) };
    }
};
} // namespace detail
} // namespace lz

#endif