#pragma once

#ifndef LZ_FN_ARGS_HOLDER_ADAPTOR_HPP
#define LZ_FN_ARGS_HOLDER_ADAPTOR_HPP

#include <Lz/detail/concepts.hpp>
#include <Lz/detail/traits.hpp>
#include <tuple>

namespace lz {
namespace detail {

template<class Adaptor, class... Ts>
struct fn_args_holder {
    std::tuple<Ts...> data;

    using adaptor = fn_args_holder<Adaptor, Ts...>;

    template<class... Args>
    LZ_CONSTEXPR_CXX_14 fn_args_holder(Args&&... args) : data{ std::forward<Args>(args)... } {
    }

    template<LZ_CONCEPT_ITERABLE Iterable, std::size_t... I>
    LZ_CONSTEXPR_CXX_14 auto
    operator()(Iterable&& iterable,
               index_sequence<I...>) const& -> decltype(std::declval<Adaptor>()(std::forward<Iterable>(iterable),
                                                                                std::get<I>(data)...)) {
        return Adaptor{}(std::forward<Iterable>(iterable), std::get<I>(data)...);
    }

    template<LZ_CONCEPT_ITERABLE Iterable, std::size_t... I>
    LZ_CONSTEXPR_CXX_14 auto
    operator()(Iterable&& iterable,
               index_sequence<I...>) && -> decltype(std::declval<Adaptor>()(std::forward<Iterable>(iterable),
                                                                            std::get<I>(std::move(data))...)) {
        return Adaptor{}(std::forward<Iterable>(iterable), std::get<I>(std::move(data))...);
    }

    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_CONSTEXPR_CXX_14 auto operator()(Iterable&& iterable) const& -> decltype((*this)(std::forward<Iterable>(iterable),
                                                                                        make_index_sequence<sizeof...(Ts)>{})) {
        return (*this)(std::forward<Iterable>(iterable), make_index_sequence<sizeof...(Ts)>{});
    }

    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_CONSTEXPR_CXX_14 auto
    operator()(Iterable&& iterable) && -> decltype(std::move(*this)(std::forward<Iterable>(iterable),
                                                                    make_index_sequence<sizeof...(Ts)>{})) {
        return std::move(*this)(std::forward<Iterable>(iterable), make_index_sequence<sizeof...(Ts)>{});
    }
};
}
}
#endif