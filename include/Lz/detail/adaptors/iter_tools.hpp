#ifndef LZ_ITER_TOOLS_ADAPTORS_HPP
#define LZ_ITER_TOOLS_ADAPTORS_HPP

#pragma once

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/map.hpp>

namespace lz {
namespace detail {
template<class To>
struct convert_fn {
    template<class From>
    constexpr To operator()(From&& f) const {
        return static_cast<To>(f);
    }
};

template<class T>
struct as_adaptor {
    using adaptor = as_adaptor<T>;

#ifdef LZ_HAS_CXX_11

    static const adaptor as;

#endif

    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 map_iterable<Iterable, convert_fn<T>> operator()(Iterable&& iterable) const {
        return lz::map(std::forward<Iterable>(iterable), convert_fn<T>{});
    }
};

} // namespace detail
} // namespace lz

#endif // LZ_ITER_TOOLS_ADAPTORS_HPP
