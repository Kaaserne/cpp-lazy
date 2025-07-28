#pragma once

#ifndef LZ_COMMON_ADAPTOR_HPP
#define LZ_COMMON_ADAPTOR_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterables/common.hpp>

namespace lz {
namespace detail {
struct common_adaptor {
    using adaptor = common_adaptor;

#ifdef LZ_HAS_CXX_17
    /**
     * Creates a common view from an iterator and a sentinel. The iterable must have a sentinel type (i.e. its begin() function
     * must return a different type than its end() function), otherwise a static assertion will fail. If the input iterable is
     * random access, it will return a basic_iterable. Otherwise, it will return a common_iterable. Example:
     * ```cpp
     * auto c_str = lz::c_string("Hello, World!"); // begin() and end() return different types
     * auto common_view = lz::common(c_str);
     * // or
     * auto common_view = lz::c_string("Hello, World!") | lz::common;
     * // now you can use common_view in <algorithm> functions
     *
     * // random access:
     * auto repeated = lz::repeat(20, 5); // begin() and end() return different types
     * auto common_view = lz::common(repeated); // common_view is a basic_iterable
     * // or
     * auto common_view = lz::repeat(20, 5) | lz::common;
     * ```
     * @attention Always try to use lz::common as late as possible. cpp-lazy is implemented in such a way that it will return
     * sentinels if possible to avoid duplicate data. So this for example:
     * ```cpp
     * auto common = lz::c_string("Hello, World!") | lz::common | lz::take(5);
     * ```
     * Will not result in an iterable where its end() and begin() functions return the same type. This is because
     * the common view is created first and then the take view is created. The take view will return another sentinel.
     * @param iterable The iterable to create a common view from.
     * @return A common iterable that can be used with algorithms that require a common iterator.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    [[nodiscard]] constexpr auto operator()(Iterable&& iterable) const {
        static_assert(has_sentinel<Iterable>::value, "Iterator and Sentinel must be different types");

        if constexpr (is_ra<iter_t<Iterable>>::value) {
            const auto size = detail::end(std::forward<Iterable>(iterable)) - std::begin(iterable);
            return basic_iterable<iter_t<Iterable>>{ std::begin(iterable), std::begin(iterable) + size };
        }
        else {
            return common_iterable<remove_ref<Iterable>>{ std::forward<Iterable>(iterable) };
        }
    }

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
     * sentinels if possible to avoid duplicate data. So this for example:
     * ```cpp
     * auto common = lz::c_string("Hello, World!") | lz::common | lz::take(5);
     * ```
     * Will not result in an iterable where its end() and begin() functions return the same type. This is because
     * the common view is created first and then the take view is created. The take view will return another sentinel.
     * @param iterable The iterable to create a common view from.
     * @return A common iterable that can be used with algorithms that require a common iterator.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD constexpr enable_if<!is_ra<iter_t<Iterable>>::value, common_iterable<remove_ref<Iterable>>>
    operator()(Iterable&& iterable) const {
        static_assert(has_sentinel<Iterable>::value, "Iterator and Sentinel must be different types");
        return common_iterable<remove_ref<Iterable>>{ std::forward<Iterable>(iterable) };
    }

    /**
     * Creates a common view/basic iterable from an iterator and a sentinel. The iterable must have a sentinel type (i.e. its
     * begin() function must return a different type than its end() function), otherwise a static assertion will fail. This
     * overload is used for random access iterators. Example:
     * ```cpp
     * auto repeated = lz::repeat(20, 5); // begin() and end() return different types
     * auto common_view = lz::common(repeated); // common_view is a basic_iterable
     * // or
     * auto common_view = lz::repeat(20, 5) | lz::common;
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
     * @return A common iterable/basic_iterable that can be used with algorithms that require a common iterator.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD constexpr enable_if<is_ra<iter_t<Iterable>>::value, basic_iterable<iter_t<Iterable>>>
    operator()(Iterable&& iterable) const {
        static_assert(has_sentinel<Iterable>::value, "Iterator and Sentinel must be different types");
        return basic_iterable<iter_t<Iterable>>{ std::begin(iterable),
                                                 std::begin(iterable) + (std::end(iterable) - std::begin(iterable)) };
    }

#endif
};
} // namespace detail
} // namespace lz

#endif