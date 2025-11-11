#pragma once

#ifndef LZ_DETAIL_TRAITS_IS_ADAPTOR_HPP
#define LZ_DETAIL_TRAITS_IS_ADAPTOR_HPP

#include <Lz/detail/traits/void.hpp>
#include <type_traits>

namespace lz {
namespace detail {

template<class T, class = void>
struct is_adaptor : std::false_type {};

template<class T>
struct is_adaptor<T, void_t<typename T::adaptor>> : std::true_type {};

} // namespace detail
} // namespace lz

#endif
