#pragma once

#ifndef LZ_COMPILER_CHECK_HPP
#define LZ_COMPILER_CHECK_HPP

#define LZ_VERSION 90000
#define LZ_VERSION_MAJOR 9
#define LZ_VERSION_MINOR 0
#define LZ_VERSION_PATCH 0


#if defined(__has_include)
#define LZ_HAS_INCLUDE(FILE) __has_include(FILE)
#else
#define LZ_HAS_INCLUDE(FILE) 0
#endif // __has_include

#if defined(__has_cpp_attribute)
#define LZ_HAS_ATTRIBUTE(ATTR) __has_cpp_attribute(ATTR)
#else
#define LZ_HAS_ATTRIBUTE(ATTR) 0
#endif // __has_cpp_attribute

#if (defined(__GNUC__)) && !(defined(__clang__))
#define LZ_GCC_VERSION __GNUC__
#endif // GNU/clang

#if defined(_MSVC_LANG)
#define LZ_MSVC _MSVC_LANG
#endif // _MSVC_LANG

#if (defined(LZ_MSVC) && (LZ_MSVC >= 201103L) && (LZ_MSVC < 201402L)) || ((__cplusplus >= 201103L) && (__cplusplus < 201402L))
#define LZ_HAS_CXX_11
#endif // end has cxx 11

#if (__cplusplus >= 201300) || ((defined(LZ_MSVC)) && (LZ_MSVC >= 201300))
#define LZ_CONSTEXPR_CXX_14 constexpr
#else
#define LZ_CONSTEXPR_CXX_14 inline
#endif // has cxx 14

#if (__cplusplus >= 201703L) || ((defined(LZ_MSVC)) && (LZ_MSVC >= 201703L))
#define LZ_HAS_CXX_17
#define LZ_CONSTEXPR_CXX_17 constexpr
#else
#define LZ_CONSTEXPR_CXX_17 inline
#endif // Has cxx 17

#if (__cplusplus >= 202002L) || ((defined(LZ_MSVC) && (LZ_MSVC >= 202002L)))
#define LZ_HAS_CXX_20
#if defined(__cpp_constexpr_dynamic_alloc) && defined(__cpp_lib_constexpr_dynamic_alloc) &&                                      \
    defined(__cpp_lib_constexpr_string) && defined(__cpp_lib_constexpr_vector) && defined(__cpp_lib_constexpr_algorithms)
#define LZ_CONSTEXPR_CXX_20 constexpr
#else
#define LZ_CONSTEXPR_CXX_20 inline
#endif // cpp constexpr new/algo
#else
#define LZ_CONSTEXPR_CXX_20 inline
#endif // Has cxx 20

#ifdef __cpp_inline_variables
#define LZ_INLINE_VAR inline
#else
#define LZ_INLINE_VAR
#endif // __cpp_inline_variables

#if LZ_HAS_ATTRIBUTE(nodiscard) && defined(LZ_HAS_CXX_17)
#define LZ_NODISCARD [[nodiscard]]
#else
#define LZ_NODISCARD
#endif // LZ_HAS_ATTRIBUTE(nodiscard)

#if LZ_HAS_INCLUDE(<string_view>) && (defined(LZ_HAS_CXX_17) && (defined(__cpp_lib_string_view)))
#define LZ_HAS_STRING_VIEW
#endif // has string view

#if LZ_HAS_INCLUDE(<concepts>) && (defined(LZ_HAS_CXX_20)) && (defined(__cpp_lib_concepts))
#define LZ_HAS_CONCEPTS
#endif // Have concepts

#ifdef __cpp_if_constexpr
#define LZ_CONSTEXPR_IF constexpr
#else
#define LZ_CONSTEXPR_IF
#endif // __cpp_if_constexpr

#ifndef LZ_MODULE_EXPORT
#define LZ_MODULE_EXPORT_SCOPE_BEGIN
#define LZ_MODULE_EXPORT_SCOPE_END
#endif

#if defined(__cpp_lib_format) && (LZ_HAS_INCLUDE(<format>)) && defined(LZ_HAS_CXX_20)
#define LZ_HAS_FORMAT
#endif // format

#endif // LZ_COMPILER_CHECK_HPP
