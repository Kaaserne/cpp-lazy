#pragma once

#ifndef LZ_DETAIL_TRAITS_FIRST_ARG_HPP
#define LZ_DETAIL_TRAITS_FIRST_ARG_HPP

namespace lz {
namespace detail {

template<class... Args>
struct first_arg {};

template<class Arg, class... Rest>
struct first_arg<Arg, Rest...> {
    using type = Arg;
};

template<>
struct first_arg<> {
    using type = void;
};

template<class... Args>
using first_arg_t = typename first_arg<Args...>::type;

} // namespace detail
} // namespace lz

#endif
