#pragma once

#ifndef LZ_DETAIL_PROCS_ASSERT_HPP
#define LZ_DETAIL_PROCS_ASSERT_HPP

#include <Lz/detail/compiler_checks.hpp>

// clang-format off

#if defined(LZ_DEBUG_ASSERTIONS)
  #define LZ_USE_DEBUG_ASSERTIONS
#endif

#if defined(LZ_USE_DEBUG_ASSERTIONS)
  #include <cstdio>
  #include <exception>
  #if defined(LZ_HAS_CXX_23) && LZ_HAS_INCLUDE(<stacktrace>)
    #include <stacktrace>
  #endif
#endif

// clang-format on

#if defined(LZ_USE_DEBUG_ASSERTIONS)

namespace lz {
namespace detail {

[[noreturn]] inline void
assertion_fail(const char* file, const int line, const char* func, const char* message, const char* expr) {
#if defined(LZ_HAS_CXX_23) && defined(__cpp_lib_stacktrace) && LZ_HAS_INCLUDE(<stacktrace>)

    auto st = std::stacktrace::current();
    auto str = std::to_string(st);
    std::fprintf(stderr, "%s:%d assertion \"%s\" failed in function '%s' with message:\n\t%s\nStacktrace:\n%s\n", file, line,
                 expr, func, message, str.c_str());

#else // ^^ defined(__cpp_lib_stacktrace) vv !defined(__cpp_lib_stacktrace)

    std::fprintf(stderr, "%s:%d assertion \"%s\" failed in function '%s' with message:\n\t%s\n", file, line, expr, func, message);

#endif // __cpp_lib_stacktrace

    std::terminate();
}
} // namespace detail
} // namespace lz

#define LZ_ASSERT(CONDITION, MSG)                                                                                                \
    ((CONDITION) ? (static_cast<void>(0)) : (lz::detail::assertion_fail(__FILE__, __LINE__, __func__, MSG, #CONDITION)))
#define LZ_ASSERT_INCREMENTABLE(CONDITION) LZ_ASSERT(CONDITION, "Cannot increment after end")
#define LZ_ASSERT_DECREMENTABLE(CONDITION) LZ_ASSERT(CONDITION, "Cannot decrement before begin")
#define LZ_ASSERT_DEREFERENCABLE(CONDITION) LZ_ASSERT(CONDITION, "Cannot dereference end")
#define LZ_ASSERT_ADDABLE(CONDITION) LZ_ASSERT(CONDITION, "Cannot add after end")
#define LZ_ASSERT_SUBTRACTABLE(CONDITION) LZ_ASSERT(CONDITION, "Cannot subtract before end")
#define LZ_ASSERT_SUB_ADDABLE(CONDITION) LZ_ASSERT(CONDITION, "Cannot add after end/cannot subtract before begin")
#define LZ_ASSERT_COMPATIBLE(CONDITION) LZ_ASSERT(CONDITION, "Incompatible iterators")

#else // ^^ defined(LZ_USE_DEBUG_ASSERTIONS) vv !defined(LZ_USE_DEBUG_ASSERTIONS)

#define LZ_ASSERT(CONDITION, MSG)
#define LZ_ASSERT_INCREMENTABLE(CONDITION)
#define LZ_ASSERT_DECREMENTABLE(CONDITION)
#define LZ_ASSERT_DEREFERENCABLE(CONDITION)
#define LZ_ASSERT_ADDABLE(CONDITION)
#define LZ_ASSERT_SUBTRACTABLE(CONDITION)
#define LZ_ASSERT_SUB_ADDABLE(CONDITION)
#define LZ_ASSERT_COMPATIBLE(CONDITION)

#endif // defined(LZ_USE_DEBUG_ASSERTIONS)

#endif
