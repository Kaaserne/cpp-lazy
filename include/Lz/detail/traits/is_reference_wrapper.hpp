#pragma once

#ifndef LZ_DETAIL_TRAITS_IS_REFERENCE_WRAPPER_HPP
#define LZ_DETAIL_TRAITS_IS_REFERENCE_WRAPPER_HPP

#include <functional>
#include <type_traits>

namespace lz {
namespace detail {

template<class T>
struct is_reference_wrapper : std::false_type {};

template<class T>
struct is_reference_wrapper<std::reference_wrapper<T>> : std::true_type {};

} // namespace detail
} // namespace lz

#endif
