#pragma once

#ifndef LZ_FN_ARGS_HOLDER_ADAPTOR_HPP
#define LZ_FN_ARGS_HOLDER_ADAPTOR_HPP

#include <Lz/detail/compiler_config.hpp>
#include <Lz/detail/traits/index_sequence.hpp>
#include <tuple>

namespace lz {
namespace detail {

template<class Adaptor, class... Ts>
class fn_args_holder {
    std::tuple<Ts...> _data;

public:
    using adaptor = fn_args_holder<Adaptor, Ts...>;

    template<class... Args>
    LZ_CONSTEXPR_CXX_14 fn_args_holder(Args&&... args) : _data{ std::forward<Args>(args)... } {
    }

private:
    template<class Iterable, size_t... I>
    LZ_CONSTEXPR_CXX_14 auto
    operator()(Iterable&& iterable,
               index_sequence<I...>) const& -> decltype(std::declval<Adaptor>()(std::forward<Iterable>(iterable),
                                                                                std::get<I>(_data)...)) {
        return Adaptor{}(std::forward<Iterable>(iterable), std::get<I>(_data)...);
    }

    template<class Iterable, size_t... I>
    LZ_CONSTEXPR_CXX_14 auto
    operator()(Iterable&& iterable,
               index_sequence<I...>) && -> decltype(std::declval<Adaptor>()(std::forward<Iterable>(iterable),
                                                                            std::get<I>(std::move(_data))...)) {
        return Adaptor{}(std::forward<Iterable>(iterable), std::get<I>(std::move(_data))...);
    }

public:
    template<class Iterable>
    LZ_CONSTEXPR_CXX_14 auto operator()(Iterable&& iterable) const& -> decltype((*this)(std::forward<Iterable>(iterable),
                                                                                        make_index_sequence<sizeof...(Ts)>{})) {
        return (*this)(std::forward<Iterable>(iterable), make_index_sequence<sizeof...(Ts)>{});
    }

    template<class Iterable>
    LZ_CONSTEXPR_CXX_14 auto
    operator()(Iterable&& iterable) && -> decltype(std::move(*this)(std::forward<Iterable>(iterable),
                                                                    make_index_sequence<sizeof...(Ts)>{})) {
        return std::move(*this)(std::forward<Iterable>(iterable), make_index_sequence<sizeof...(Ts)>{});
    }
};
}
}
#endif
