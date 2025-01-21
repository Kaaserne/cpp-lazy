#pragma once

#ifndef LZ_CHUNK_IF_HPP
#define LZ_CHUNK_IF_HPP

#include <Lz/detail/iterators/chunk_if.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class ValueType>
struct chunk_if_adaptor {
#ifdef LZ_HAS_CXX_11
    static chunk_if_adaptor<ValueType> chunk_if;
#endif

    using adaptor = chunk_if_adaptor;

    template<class Iterable, class UnaryPredicate>
    constexpr detail::chunk_if_iterable<ValueType, Iterable, detail::decay_t<UnaryPredicate>>
    operator()(Iterable&& iterable, UnaryPredicate&& predicate) const {
        return { std::forward<Iterable>(iterable), std::forward<UnaryPredicate>(predicate) };
    }

    template<class UnaryPredicate>
    constexpr detail::fn_args_holder<chunk_if_adaptor<ValueType>, detail::decay_t<UnaryPredicate>>
    operator()(UnaryPredicate&& predicate) const {
        return { std::forward<UnaryPredicate>(predicate) };
    }
};

template<>
struct chunk_if_adaptor<void> {
#ifdef LZ_HAS_CXX_11
    static chunk_if_adaptor<void> chunk_if;
#endif

    using adaptor = chunk_if_adaptor<void>;

    template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
    constexpr detail::chunk_if_iterable<basic_iterable<iter_t<Iterable>>, Iterable, detail::decay_t<UnaryPredicate>>
    operator()(Iterable&& iterable, UnaryPredicate&& predicate) const {
        return { std::forward<Iterable>(iterable), std::forward<UnaryPredicate>(predicate) };
    }

    template<class UnaryPredicate>
    constexpr detail::fn_args_holder<chunk_if_adaptor<void>, detail::decay_t<UnaryPredicate>>
    operator()(UnaryPredicate&& predicate) const {
        return { std::forward<UnaryPredicate>(predicate) };
    }
};

#ifdef LZ_HAS_CXX_11

/**
 * @brief This adaptor is used to make chunks of the iterable, based on a condition returned by the function passed. The iterator
 * category is forward. It returns an iterable of iterables. If `std::string` or `[lz|std]::string_view` is preffed as its
 * `value_type`, please see `s_chunk_if` or `sv_chunk_if` respectively.
 *
 * Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto chunked = lz::chunk_if(vec, [](int i) { return i % 2 == 0; }); // chunked = { {1, 2}, {3, 4}, {5} }
 * // or
 * auto chunked = vec | lz::chunk_if([](int i) { return i % 2 == 0; }); // chunked = { {1, 2}, {3, 4}, {5} }
 * }
 * ```
 */
chunk_if_adaptor<void> chunk_if_adaptor<void>::chunk_if{};

/**
 * @brief This adaptor is used to make chunks of the iterable, based on a condition returned by the function passed. The iterator
 * category is forward. It returns an iterable of `[std|lz]::string_view`. The input iterable must therefore be random_access.
 *
 * Example:
 * ```cpp
 * std::string s = "hello;world;;";
 * auto chunked = lz::sv_chunk_if(s, [](char c) { return c == ';'; });
 * // chunked = { string_view{"hello"}, string_view{"world"}, string_view{""}, string_view{""} }
 * // or
 * auto chunked = s | lz::chunk_if([](char c) { return c == ';'; });
 * // chunked = { string_view{"hello"}, string_view{"world"}, string_view{""}, string_view{""} }
 * ```
 */
chunk_if_adaptor<lz::string_view> chunk_if_adaptor<lz::string_view>::sv_chunk_if{};

/**
 * @brief This adaptor is used to make chunks of the iterable, based on a condition returned by the function passed. The iterator
 * category is forward. It returns an iterable of `std::string`. The input iterable must therefore be random_access.
 *
 * Example:
 * ```cpp
 * std::string s = "hello;world;;";
 * auto chunked = lz::sv_chunk_if(s, [](char c) { return c == ';'; });
 * // chunked = { std::string{"hello"}, std::string{"world"}, std::string{""}, std::string{""} }
 * // or
 * auto chunked = s | lz::chunk_if([](char c) { return c == ';'; });
 * // chunked = { std::string{"hello"}, std::string{"world"}, std::string{""}, std::string{""} }
 * ```
 */
chunk_if_adaptor<std::string> chunk_if_adaptor<std::string>::s_chunk_if{};

#else

/**
 * @brief This adaptor is used to make chunks of the iterable, based on a condition returned by the function passed. The iterator
 * category is forward. It returns an iterable of iterables. If `std::string` or `[lz|std]::string_view` is preffed as its
 * `value_type`, please see `s_chunk_if` or `sv_chunk_if` respectively.
 *
 * Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto chunked = lz::chunk_if(vec, [](int i) { return i % 2 == 0; }); // chunked = { {1, 2}, {3, 4}, {5} }
 * // or
 * auto chunked = vec | lz::chunk_if([](int i) { return i % 2 == 0; }); // chunked = { {1, 2}, {3, 4}, {5} }
 * }
 * ```
 */
LZ_INLINE_VAR constexpr chunk_if_adaptor<void> chunk_if{};

/**
 * @brief This adaptor is used to make chunks of the iterable, based on a condition returned by the function passed. The iterator
 * category is forward. It returns an iterable of `[std|lz]::string_view`. The input iterable must therefore be random_access.
 *
 * Example:
 * ```cpp
 * std::string s = "hello;world;;";
 * auto chunked = lz::sv_chunk_if(s, [](char c) { return c == ';'; });
 * // chunked = { string_view{"hello"}, string_view{"world"}, string_view{""}, string_view{""} }
 * // or
 * auto chunked = s | lz::chunk_if([](char c) { return c == ';'; });
 * // chunked = { string_view{"hello"}, string_view{"world"}, string_view{""}, string_view{""} }
 * ```
 */
LZ_INLINE_VAR constexpr chunk_if_adaptor<lz::string_view> sv_chunk_if{};

/**
 * @brief This adaptor is used to make chunks of the iterable, based on a condition returned by the function passed. The iterator
 * category is forward. It returns an iterable of `std::string`. The input iterable must therefore be random_access.
 *
 * Example:
 * ```cpp
 * std::string s = "hello;world;;";
 * auto chunked = lz::sv_chunk_if(s, [](char c) { return c == ';'; });
 * // chunked = { std::string{"hello"}, std::string{"world"}, std::string{""}, std::string{""} }
 * // or
 * auto chunked = s | lz::chunk_if([](char c) { return c == ';'; });
 * // chunked = { std::string{"hello"}, std::string{"world"}, std::string{""}, std::string{""} }
 * ```
 */
LZ_INLINE_VAR constexpr chunk_if_adaptor<std::string> s_chunk_if{};

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_CHUNK_IF_HPP
