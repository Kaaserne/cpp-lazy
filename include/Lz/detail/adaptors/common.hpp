#pragma once

#ifndef LZ_COMMON_ADAPTOR_HPP
#define LZ_COMMON_ADAPTOR_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/iterables/common.hpp>

namespace lz {
namespace detail {
struct common_adaptor {
    using adaptor = common_adaptor;

#ifdef LZ_HAS_CXX_17

    /**
     * Creates a common view from an iterator and a sentinel. If the iterable does not have a sentinel (i.e. its begin() function
     * must return a different type than its end() function), the iput iterable is returned. . Example:
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
    template<class Iterable>
    [[nodiscard]] constexpr decltype(auto) operator()(Iterable&& iterable) const {
        using it = iter_t<Iterable>;

        if constexpr (!has_sentinel_v<Iterable>) {
            return std::forward<Iterable>(iterable);
        }
        else if constexpr (is_ra_v<it>) {
            const auto size = detail::end(std::forward<Iterable>(iterable)) - iterable.begin();
            return basic_iterable<it>{ iterable.begin(), iterable.begin() + size };
        }
        else if constexpr (std::is_assignable_v<it, sentinel_t<Iterable>>) {
            auto end = std::begin(iterable);
            end = detail::end(std::forward<Iterable>(iterable));
            if constexpr (is_sized_v<Iterable>) {
                return sized_iterable<it>{ detail::begin(std::forward<Iterable>(iterable)), end, lz::size(iterable) };
            }
            else {
                return make_basic_iterable(detail::begin(std::forward<Iterable>(iterable)), end);
            }
        }
        else {
            return common_iterable<remove_ref_t<Iterable>>{ std::forward<Iterable>(iterable) };
        }
    }

#else

    /**
     * Creates a common view from an iterator and a sentinel. If the iterable does not have a sentinel (i.e. its begin() function
     * must return a different type than its end() function), the iput iterable is returned. . Example:
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
    template<class Iterable>
    LZ_NODISCARD constexpr enable_if_t<!has_sentinel<Iterable>::value, Iterable> operator()(Iterable&& iterable) const {
        return std::forward<Iterable>(iterable);
    }

    /**
     * Creates a common view from an iterator and a sentinel. If the iterable does not have a sentinel (i.e. its begin() function
     * must return a different type than its end() function), the iput iterable is returned. . Example:
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
    template<class Iterable>
    LZ_NODISCARD constexpr enable_if_t<has_sentinel<Iterable>::value &&
                                           !std::is_assignable<iter_t<Iterable>, sentinel_t<Iterable>>::value &&
                                           !is_ra<iter_t<Iterable>>::value,
                                       common_iterable<remove_ref_t<Iterable>>>
    operator()(Iterable&& iterable) const {
        return common_iterable<remove_ref_t<Iterable>>{ std::forward<Iterable>(iterable) };
    }

    /**
     * Creates a common view from an iterator and a sentinel. If the iterable does not have a sentinel (i.e. its begin() function
     * must return a different type than its end() function), the iput iterable is returned. . Example:
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
    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14
        enable_if_t<!is_ra<iter_t<Iterable>>::value && has_sentinel<Iterable>::value &&
                        std::is_assignable<iter_t<Iterable>, sentinel_t<Iterable>>::value && !is_sized<Iterable>::value,
                    basic_iterable<iter_t<Iterable>>>
        operator()(Iterable&& iterable) const {
        auto end = std::begin(iterable);
        end = detail::end(std::forward<Iterable>(iterable));
        return make_basic_iterable(detail::begin(std::forward<Iterable>(iterable)), end);
    }

    /**
     * Creates a common view from an iterator and a sentinel. If the iterable does not have a sentinel (i.e. its begin() function
     * must return a different type than its end() function), the iput iterable is returned. . Example:
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
    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14
        enable_if_t<!is_ra<iter_t<Iterable>>::value && has_sentinel<Iterable>::value &&
                        std::is_assignable<iter_t<Iterable>, sentinel_t<Iterable>>::value && is_sized<Iterable>::value,
                    sized_iterable<iter_t<Iterable>>>
        operator()(Iterable&& iterable) const {
        auto end = std::begin(iterable);
        end = detail::end(std::forward<Iterable>(iterable));
        return { detail::begin(std::forward<Iterable>(iterable)), end, lz::size(iterable) };
    }

    /**
     * Creates a common view from an iterator and a sentinel. If the iterable does not have a sentinel (i.e. its begin() function
     * must return a different type than its end() function), the iput iterable is returned. . Example:
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
    template<class Iterable>
    LZ_NODISCARD constexpr enable_if_t<is_ra<iter_t<Iterable>>::value && has_sentinel<Iterable>::value,
                                       basic_iterable<iter_t<Iterable>>>
    operator()(Iterable&& iterable) const {
        return basic_iterable<iter_t<Iterable>>{ iterable.begin(), iterable.begin() + (iterable.end() - iterable.begin()) };
    }

#endif
};
} // namespace detail
} // namespace lz

#endif
