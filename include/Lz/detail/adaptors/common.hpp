#pragma once

#ifndef LZ_COMMON_ADAPTOR_HPP
#define LZ_COMMON_ADAPTOR_HPP

#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterables/common.hpp>

namespace lz {
namespace detail {
struct common_adaptor {
    using adaptor = common_adaptor;

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
     * sentinels if possible to avoid duplicate data. So this for example:
     * ```cpp
     * auto common = lz::c_string("Hello, World!") | lz::common | lz::take(5);
     * ```
     * Will not result in an iterable where its end() and begin() functions return the same type. This is because
     * the common view is created first and then the take view is created. The take view will return another sentinel.
     * @param iterable The iterable to create a common view from.
     * @return A common iterable that can be used with algorithms that require a common iterator and sentinel.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD constexpr common_iterable<remove_ref<Iterable>> operator()(Iterable&& iterable) const {
        static_assert(has_sentinel<Iterable>::value, "Iterator and Sentinel must be different types");
        return { std::forward<Iterable>(iterable) };
    }
};
} // namespace detail
} // namespace lz

#endif