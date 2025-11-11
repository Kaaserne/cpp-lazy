#pragma once

#ifndef LZ_DETAIL_TRAITS_CONCEPTS_HPP
#define LZ_DETAIL_TRAITS_CONCEPTS_HPP

#include <Lz/detail/compiler_config.hpp>

#ifdef LZ_HAS_CONCEPTS

#include <Lz/detail/procs/begin_end.hpp>
#include <Lz/procs/size.hpp>

LZ_MODULE_EXPORT namespace lz {

template<class T>
concept sized = requires(T&& c) {
    { lz::size(c) };
};

template<class T>
concept iterable = requires(T&& c) {
    { detail::begin(c) };
    { detail::end(c) };
};

template<class T>
concept adaptor = requires(T) { typename T::adaptor; };

} // namespace lz

#endif // LZ_HAS_CONCEPTS

#endif
