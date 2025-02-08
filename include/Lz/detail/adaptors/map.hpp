#pragma once

#ifndef LZ_MAP_ADAPTOR_HPP
#define LZ_MAP_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/iterables/map.hpp>

namespace lz {
namespace detail {
struct map_adaptor {
    using adaptor = map_adaptor;

    template<class Iterable, class Function>
    constexpr map_iterable<Iterable, Function> operator()(Iterable&& iterable, Function function) const {
        return { std::forward<Iterable>(iterable), std::move(function) };
    }

    template<class Function>
    LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, Function> operator()(Function&& function) const {
        return { std::move(function) };
    }
};
} // namespace detail
} // namespace lz

#endif