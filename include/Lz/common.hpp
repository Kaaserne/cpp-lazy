#pragma once

#ifndef LZ_COMMON_HPP
#define LZ_COMMON_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/iterators/common.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

struct common_adaptor {
#ifdef LZ_HAS_CXX_11
    static common_adaptor common;
#endif

    using adaptor = common_adaptor;

    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 detail::common_iterable<Iterable> operator()(Iterable&& iterable) const {
        static_assert(detail::has_sentinel<Iterable>::value, "Iterator and Sentinel must be different types");
        return { std::forward<Iterable>(iterable) };
    }
};

#ifdef LZ_HAS_CXX_11

/**
 * Creates a common view from an iterator and a sentinel. The iterable must have a sentinel type (i.e. its begin() function
 * must return a different type than its end() function), otherwise a static assertion will fail.
 * Example:
 * ```cpp
 * auto c_str = lz::c_string("Hello, World!"); // begin() and end() return different types
 * auto common_view = lz::common(c_str);
 * // or
 * auto common_view = lz::c_string("Hello, World!") | lz::common;
 * // now you can use common_view in <algorithm> functions
 * ```
 * @attention Always try to use lz::common as late as possible. cpp-lazy is implemented in such a way that it will return
 * sentinels whenever possible. So this for example:
 * ```cpp
 * auto common = lz::c_string("Hello, World!") | lz::common | lz::take(5);
 * ```
 * Will not result in an iterable where its end() and begin() functions return the same type. This is because
 * the common view is created first and then the take view is created. The take view will return another sentinel, because
 * that's how cpp-lazy is implemented. It will return a sentinel if its input iterable has a forward iterator.
 * @param iterable The iterable to create a common view from.
 * @return A common view object. Can be used in <algorithm> functions.
 */
common_adaptor common_adaptor::common{};

#else

/**
 * Creates a common view from an iterator and a sentinel. The iterable must have a sentinel type (i.e. its begin() function
 * must return a different type than its end() function), otherwise a static assertion will fail.
 * Example:
 * ```cpp
 * auto c_str = lz::c_string("Hello, World!"); // begin() and end() return different types
 * auto common_view = lz::common(c_str);
 * // or
 * auto common_view = lz::c_string("Hello, World!") | lz::common;
 * // now you can use common_view in <algorithm> functions
 * ```
 * @attention Always try to use lz::common as late as possible. cpp-lazy is implemented in such a way that it will return
 * sentinels whenever possible. So this for example:
 * ```cpp
 * auto common = lz::c_string("Hello, World!") | lz::common | lz::take(5);
 * ```
 * Will not result in an iterable where its end() and begin() functions return the same type. This is because
 * the common view is created first and then the take view is created. The take view will return another sentinel, because
 * that's how cpp-lazy is implemented. It will return a sentinel if its input iterable has a forward iterator.
 * @param iterable The iterable to create a common view from.
 * @return A common view object. Can be used in <algorithm> functions.
 */
LZ_INLINE_VAR constexpr common_adaptor common{};

#endif

LZ_MODULE_EXPORT_SCOPE_END
} // namespace lz

#endif
