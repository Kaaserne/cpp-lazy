#pragma once

#ifndef LZ_DETAIL_PROCS_HPP
#define LZ_DETAIL_PROCS_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/func_container.hpp>
#include <Lz/detail/traits.hpp>
#include <cstddef>
#include <iterator>
#include <limits>
#include <tuple>

#ifndef NDEBUG
#include <exception>
#endif // NDEBUG

#if defined(LZ_STANDALONE) && (!defined(LZ_HAS_FORMAT)) && defined(__cpp_lib_to_chars)
#include <charconv>
#endif

#if defined(__cpp_lib_stacktrace) && LZ_HAS_INCLUDE(<stacktrace>)
#include <stacktrace>
#endif

#include <exception>

namespace lz {

template<class Iter, class S>
constexpr diff_type<Iter> distance(Iter first, S last);

namespace detail {

[[noreturn]] inline void assertion_fail(const char* file, const int line, const char* func, const char* message) {
#if defined(__cpp_lib_stacktrace) && LZ_HAS_INCLUDE(<stacktrace>)
    auto st = std::stacktrace::current();
    auto str = std::to_string(st);
    std::fprintf(stderr, "%s:%d assertion failed in function '%s' with message:\n\t%s\nStacktrace:\n%s\n", file, line, func,
                 message, str.c_str());
#else
    std::fprintf(stderr, "%s:%d assertion failed in function '%s' with message:\n\t%s\n", file, line, func, message);
#endif
    std::terminate();
}

#define LZ_ASSERT(CONDITION, MSG) ((CONDITION) ? ((void)0) : (lz::detail::assertion_fail(__FILE__, __LINE__, __func__, MSG)))

template<class Iterable>
LZ_NODISCARD constexpr auto begin(Iterable&& c) noexcept -> decltype(std::forward<Iterable>(c).begin()) {
    return std::forward<Iterable>(c).begin();
}

template<class Iterable>
LZ_NODISCARD constexpr auto end(Iterable&& c) noexcept -> decltype(std::forward<Iterable>(c).end()) {
    return std::forward<Iterable>(c).end();
}

template<class T, size_t N>
LZ_NODISCARD constexpr auto begin(T (&array)[N]) noexcept -> decltype(std::begin(array)) {
    return std::begin(array);
}

template<class T, size_t N>
LZ_NODISCARD constexpr auto end(T (&array)[N]) noexcept -> decltype(std::end(array)) {
    return std::end(array);
}

template<class T, size_t N>
LZ_NODISCARD constexpr auto begin(const T (&array)[N]) noexcept -> decltype(std::begin(array)) {
    return std::begin(array);
}

template<class T, size_t N>
LZ_NODISCARD constexpr auto end(const T (&array)[N]) noexcept -> decltype(std::end(array)) {
    return std::end(array);
}

template<class IterableTuple, std::size_t... I>
LZ_CONSTEXPR_CXX_14 auto begin_tuple(IterableTuple&& iterable_tuple, std::index_sequence<I...>)
    -> decltype(std::make_tuple(detail::begin(std::get<I>(std::forward<IterableTuple>(iterable_tuple)))...)) {
    return std::make_tuple(detail::begin(std::get<I>(std::forward<IterableTuple>(iterable_tuple)))...);
}

template<class IterableTuple, std::size_t... I>
LZ_CONSTEXPR_CXX_14 auto end_tuple(IterableTuple&& iterable_tuple, std::index_sequence<I...>)
    -> decltype(std::make_tuple(detail::end(std::get<I>(std::forward<IterableTuple>(iterable_tuple)))...)) {
    return std::make_tuple(detail::end(std::get<I>(std::forward<IterableTuple>(iterable_tuple)))...);
}

template<class Fn>
struct tuple_expand {
private:
    func_container<Fn> _fn;

public:
    constexpr tuple_expand() = default;

    template<class F>
    explicit constexpr tuple_expand(F&& fn) : _fn(std::forward<F>(fn)) {
    }

    template<class Tuple, std::size_t... I>
    LZ_CONSTEXPR_CXX_14 auto call(Tuple&& tuple, index_sequence_helper<I...>)
        -> decltype(_fn(std::get<I>(std::forward<Tuple>(tuple))...)) {
        return _fn(std::get<I>(std::forward<Tuple>(tuple))...);
    }

    template<class Tuple, std::size_t... I>
    LZ_CONSTEXPR_CXX_14 auto call(Tuple&& tuple, index_sequence_helper<I...>) const
        -> decltype(_fn(std::get<I>(std::forward<Tuple>(tuple))...)) {
        return _fn(std::get<I>(std::forward<Tuple>(tuple))...);
    }

    template<class Tuple>
    LZ_CONSTEXPR_CXX_14 auto operator()(Tuple&& tuple)
        -> decltype(call(std::forward<Tuple>(tuple), make_index_sequence<std::tuple_size<decay_t<Tuple>>::value>{})) {
        return call(std::forward<Tuple>(tuple), make_index_sequence<std::tuple_size<decay_t<Tuple>>::value>{});
    }

    template<class Tuple>
    LZ_CONSTEXPR_CXX_14 auto operator()(Tuple&& tuple) const
        -> decltype(call(std::forward<Tuple>(tuple), make_index_sequence<std::tuple_size<decay_t<Tuple>>::value>{})) {
        return call(std::forward<Tuple>(tuple), make_index_sequence<std::tuple_size<decay_t<Tuple>>::value>{});
    }
};

template<class Fn>
constexpr tuple_expand<decay_t<Fn>> make_expand_fn(Fn&& fn) {
    return { std::forward<Fn>(fn) };
}

template<class GenFn, class... Args>
using tuple_invoker = decltype(make_expand_fn(std::declval<GenFn>()));

template<class GenFn, class... Args>
using tuple_invoker_ret = decltype(std::declval<tuple_invoker<GenFn, Args...>>()(
    std::declval<typename std::add_lvalue_reference<std::tuple<Args...>>::type>()));

template<class Arithmetic>
LZ_CONSTEXPR_CXX_14 bool is_even(const Arithmetic value) noexcept {
    return (value % 2) == 0;
}

template<class... Ts>
constexpr void decompose(const Ts&...) noexcept {
}

template<class Result, class Arithmetic>
LZ_CONSTEXPR_CXX_14 Result round_even(const Arithmetic a, const Arithmetic b) noexcept {
    LZ_ASSERT(a != 0 && b != 0, "division by zero error");
    if (b == 1) {
        return static_cast<Result>(a);
    }
    if (b == -1) {
        return -static_cast<Result>(a);
    }
    if (is_even(a) && is_even(b)) {
        return static_cast<Result>(a / b);
    }
    return static_cast<Result>(a / b) + 1;
}

template<class Iter, class S>
LZ_CONSTEXPR_CXX_14 diff_type<Iter> size_hint(Iter begin, S end) {
    if LZ_CONSTEXPR_IF (is_ra<Iter>::value) {
        return lz::distance(std::move(begin), std::move(end));
    }
    else {
        return 0;
    }
}

#if defined(LZ_STANDALONE) && (!defined(LZ_HAS_FORMAT))
template<class T>
struct safe_buffer_size : std::integral_constant<std::size_t, std::numeric_limits<T>::digits10 + 3> {};

template<>
struct safe_buffer_size<bool> : std::integral_constant<std::size_t, sizeof("false") + 1> {};

inline void to_string_from_buff(const char value, char buff[safe_buffer_size<char>::value]) {
    buff[0] = value;
    std::fill(buff + 1, buff + safe_buffer_size<char>::value, '\0');
}

inline void to_string_from_buff(const bool value, char buff[safe_buffer_size<bool>::value]) {
    std::snprintf(buff, safe_buffer_size<bool>::value, "%s", value ? "true" : "false");
}

#if !defined(__cpp_lib_to_chars)
template<class TCast, class T>
void to_string_from_buff(const T value, char buff[safe_buffer_size<T>::value], const char* fmt) {
    std::snprintf(buff, safe_buffer_size<T>::value, fmt, static_cast<TCast>(value));
}
#endif // !defined(__cpp_lib_to_chars)

#ifdef __cpp_lib_to_chars
template<class T>
void to_string_from_buff(const T value, char buff[safe_buffer_size<T>::value]) {
    std::to_chars(buff, buff + safe_buffer_size<T>::value, value);
}
#elif defined(__cpp_if_constexpr)
template<class T>
void to_string_from_buff(const T value, char buff[safe_buffer_size<T>::value]) {
    if constexpr (std::is_integral<T>::value) {
        if constexpr (std::is_signed<T>::value) {
            to_string_from_buff<long long>(value, buff, "%lld");
        }
        else {
            to_string_from_buff<unsigned long long>(value, buff, "%llu");
        }
        return;
    }
    else if constexpr (std::is_floating_point<T>::value) {
        to_string_from_buff<long double>(value, buff, "%Lf");
    }
}
#else
template<class T>
enable_if<std::is_integral<T>::value && std::is_signed<T>::value>
to_string_from_buff(const T value, char buff[safe_buffer_size<T>::value]) {
    to_string_from_buff<long long>(value, buff, "%lld");
}

template<class T>
enable_if<std::is_integral<T>::value && !std::is_signed<T>::value>
to_string_from_buff(const T value, char buff[safe_buffer_size<T>::value]) {
    to_string_from_buff<unsigned long long>(value, buff, "%llu");
}

template<class T>
enable_if<std::is_floating_point<T>::value> to_string_from_buff(const T value, char buff[safe_buffer_size<T>::value]) {
    to_string_from_buff<long double>(value, buff, "%Lf");
}
#endif
#endif // if defined(LZ_STANDALONE) && (!defined(LZ_HAS_FORMAT))
} // namespace detail
} // namespace lz

template<class Iterable, class Adaptor>
LZ_NODISCARD constexpr auto operator|(Iterable&& iterable, Adaptor&& adaptor)
    -> lz::detail::enable_if<lz::detail::is_adaptor<Adaptor>::value, decltype(adaptor(std::forward<Iterable>(iterable)))> {
    return std::forward<Adaptor>(adaptor)(std::forward<Iterable>(iterable));
}

#endif // LZ_DETAIL_PROCS_HPP