#pragma once

#ifndef LZ_DETAIL_TRAITS_ENABLE_IF_HPP
#define LZ_DETAIL_TRAITS_ENABLE_IF_HPP

namespace lz {
namespace detail {

template<bool B>
struct enable_if {};

template<>
struct enable_if<true> {
    template<class T>
    using type = T;
};

template<bool B, class T = void>
using enable_if_t = typename enable_if<B>::template type<T>;

} // namespace detail
} // namespace lz

#endif // LZ_ENABLE_IF_HPP
