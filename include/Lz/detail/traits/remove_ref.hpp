#pragma once

#ifndef LZ_DETAIL_TRAITS_REMOVE_CVREF_HPP
#define LZ_DETAIL_TRAITS_REMOVE_CVREF_HPP

#include <type_traits>

namespace lz {
namespace detail {

template<class T>
struct remove_rvalue_reference {
    using type = T;
};

template<class T>
struct remove_rvalue_reference<T&&> {
    using type = T;
};

template<class T>
using remove_rvalue_reference_t = typename remove_rvalue_reference<T>::type;

template<class T>
using remove_ref_t = typename std::remove_reference<T>::type;

template<class T>
using remove_cref_t = typename std::remove_const<remove_ref_t<T>>::type;

#ifdef LZ_HAS_CXX_20

template<class T>
using remove_cvref_t = std::remove_cvref_t<T>;

#else // ^^^ has cxx 20 vvv cxx < 20

template<class T>
using remove_cvref_t = typename std::remove_cv<remove_ref_t<T>>::type;

#endif // LZ_HAS_CXX_20

} // namespace detail
} // namespace lz

#endif
