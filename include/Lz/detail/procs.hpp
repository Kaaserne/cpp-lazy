#pragma once

#ifndef LZ_DETAIL_PROCS_HPP
#define LZ_DETAIL_PROCS_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/traits.hpp>
#include <cstddef>
#include <iterator>
#include <limits>
#include <tuple>

#ifndef NDEBUG
#include <exception>
#endif // NDEBUG

#if defined(__cpp_lib_stacktrace) && LZ_HAS_INCLUDE(<stacktrace>)
#include <stacktrace>
#endif

#include <exception>

namespace lz {
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
LZ_NODISCARD constexpr auto begin(Iterable&& c) noexcept
    -> enable_if<!std::is_array<remove_reference_t<Iterable>>::value, decltype(std::forward<Iterable>(c).begin())> {
    return std::forward<Iterable>(c).begin();
}

template<class Iterable>
LZ_NODISCARD constexpr auto end(Iterable&& c) noexcept
    -> enable_if<!std::is_array<remove_reference_t<Iterable>>::value, decltype(std::forward<Iterable>(c).end())> {
    return std::forward<Iterable>(c).end();
}

template<class Iterable>
LZ_NODISCARD constexpr auto begin(Iterable&& c) noexcept
    -> enable_if<std::is_array<remove_reference_t<Iterable>>::value, decltype(std::begin(c))> {
    return std::begin(c);
}

template<class Iterable>
LZ_NODISCARD constexpr auto end(Iterable&& c) noexcept
    -> enable_if<std::is_array<remove_reference_t<Iterable>>::value, decltype(std::end(c))> {
    return std::end(c);
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
    Fn _fn;

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
} // namespace detail

#ifdef LZ_HAS_CXX_17

template<class Iterable>
LZ_NODISCARD constexpr auto size(Iterable&& c) noexcept(noexcept(std::size(c))) {
    static_assert(detail::sized<Iterable>::value, "Iterable must be sized/contain a .size() method");
    return std::size(c);
}

#else

template<class Iterable>
LZ_NODISCARD constexpr auto size(const Iterable& c) noexcept(noexcept(c.size())) -> decltype(c.size()) {
    static_assert(detail::sized<Iterable>::value, "Iterable must be sized/contain a .size() method");
    return c.size();
}

template<class T, size_t N>
LZ_NODISCARD constexpr std::size_t size(const T (&)[N]) noexcept {
    return N;
}

#endif
} // namespace lz
#endif // LZ_DETAIL_PROCS_HPP