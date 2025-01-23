#pragma once

#ifndef LZ_FN_ARGS_HOLDER_ADAPTOR_HPP
#define LZ_FN_ARGS_HOLDER_ADAPTOR_HPP

#include <Lz/detail/traits.hpp>
#include <tuple>

namespace lz {
namespace detail {

template<class Adaptor, class... Ts>
struct fn_args_holder {
    std::tuple<Ts...> data;

    using adaptor = fn_args_holder<Adaptor, Ts...>;

    template<class... Args>
    LZ_CONSTEXPR_CXX_14 fn_args_holder(Args&&... args) noexcept : data(std::forward<Args>(args)...) {
    }

    template<class Iterable, std::size_t... I>
    LZ_CONSTEXPR_CXX_14 auto operator()(Iterable&& iterable, index_sequence_helper<I...>) const& {
        constexpr Adaptor adaptor;
        return adaptor(std::forward<Iterable>(iterable), std::get<I>(data)...);
    }

    template<class Iterable, std::size_t... I>
    LZ_CONSTEXPR_CXX_14 auto
    operator()(Iterable&& iterable, index_sequence_helper<I...>) &&
            -> decltype(std::declval<Adaptor>()(std::forward<Iterable>(iterable), std::get<I>(std::move(data))...)) {
        constexpr Adaptor adaptor;
        return adaptor(std::forward<Iterable>(iterable), std::get<I>(std::move(data))...);
    }

    template<class Iterable>
    LZ_CONSTEXPR_CXX_14 auto operator()(Iterable&& iterable) const& -> decltype((*this)(std::forward<Iterable>(iterable),
                                                                                        make_index_sequence<sizeof...(Ts)>{})) {
        return (*this)(std::forward<Iterable>(iterable), make_index_sequence<sizeof...(Ts)>{});
    }

    template<class Iterable>
    LZ_CONSTEXPR_CXX_14 auto operator()(Iterable&& iterable) && -> decltype(std::move(*this)(std::forward<Iterable>(iterable))) {
        return std::move(*this)(std::forward<Iterable>(iterable), make_index_sequence<sizeof...(Ts)>{});
    }
};
}
}
#endif