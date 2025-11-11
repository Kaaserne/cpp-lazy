#pragma once

#ifndef LZ_PROCS_CHAIN_HPP
#define LZ_PROCS_CHAIN_HPP

#include <Lz/detail/compiler_config.hpp>
#include <Lz/detail/traits/enable_if.hpp>
#include <Lz/detail/traits/is_adaptor.hpp>
#include <Lz/detail/traits/is_iterable.hpp>
#include <Lz/detail/traits/remove_ref.hpp>

#ifdef LZ_HAS_CONCEPTS

#include <Lz/traits/concepts.hpp>

LZ_MODULE_EXPORT template<class Iterable, class Adaptor>
    requires(lz::adaptor<lz::detail::remove_cref_t<Adaptor>> && lz::iterable<lz::detail::remove_ref_t<Iterable>>)
[[nodiscard]] constexpr auto operator|(Iterable&& iterable, Adaptor&& adaptor) {
    return std::forward<Adaptor>(adaptor)(std::forward<Iterable>(iterable));
}

#else

LZ_MODULE_EXPORT template<class Iterable, class Adaptor>
LZ_NODISCARD constexpr auto operator|(Iterable&& iterable, Adaptor&& adaptor)
    -> lz::detail::enable_if_t<lz::detail::is_adaptor<lz::detail::remove_cref_t<Adaptor>>::value &&
                                   lz::detail::is_iterable<lz::detail::remove_ref_t<Iterable>>::value,
                               decltype(std::forward<Adaptor>(adaptor)(std::forward<Iterable>(iterable)))> {
    return std::forward<Adaptor>(adaptor)(std::forward<Iterable>(iterable));
}

#endif // LZ_HAS_CONCEPTS

#endif // LZ_PROCS_CHAIN_HPP
