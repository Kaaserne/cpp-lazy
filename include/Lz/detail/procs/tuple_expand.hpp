#pragma once

#ifndef LZ_DETAIL_PROCS_TUPLE_EXPAND_HPP
#define LZ_DETAIL_PROCS_TUPLE_EXPAND_HPP

#include <Lz/detail/traits/index_sequence.hpp>
#include <Lz/detail/traits/remove_ref.hpp>

#ifndef LZ_HAS_CONCEPTS
#include <Lz/detail/traits/enable_if.hpp>
#endif

#include <utility>
#include <type_traits>

namespace lz {
namespace detail {

using std::get;

template<class Fn>
class tuple_expand {
    Fn _fn;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr tuple_expand()
        requires std::default_initializable<Fn>
    = default;

#else

    template<class F, class = enable_if_t<std::is_default_constructible<F>::value>>
    constexpr tuple_expand() noexcept(std::is_nothrow_default_constructible<F>::value) {
    }

#endif

    template<class F>
    explicit constexpr tuple_expand(F&& fn) : _fn{ std::forward<F>(fn) } {
    }

private:
    template<class Tuple, size_t... I>
    LZ_CONSTEXPR_CXX_14 auto call(Tuple&& tuple, index_sequence<I...>) -> decltype(_fn(get<I>(std::forward<Tuple>(tuple))...)) {
        return _fn(get<I>(std::forward<Tuple>(tuple))...);
    }

    template<class Tuple, size_t... I>
    LZ_CONSTEXPR_CXX_14 auto
    call(Tuple&& tuple, index_sequence<I...>) const -> decltype(_fn(get<I>(std::forward<Tuple>(tuple))...)) {
        return _fn(get<I>(std::forward<Tuple>(tuple))...);
    }

public:
    template<class Tuple>
    LZ_CONSTEXPR_CXX_14 auto
    operator()(Tuple&& tuple) -> decltype(call(std::forward<Tuple>(tuple),
                                               make_index_sequence<std::tuple_size<remove_cvref_t<Tuple>>::value>{})) {
        return call(std::forward<Tuple>(tuple), make_index_sequence<std::tuple_size<remove_cvref_t<Tuple>>::value>{});
    }

    template<class Tuple>
    LZ_CONSTEXPR_CXX_14 auto
    operator()(Tuple&& tuple) const -> decltype(call(std::forward<Tuple>(tuple),
                                                     make_index_sequence<std::tuple_size<remove_cvref_t<Tuple>>::value>{})) {
        return call(std::forward<Tuple>(tuple), make_index_sequence<std::tuple_size<remove_cvref_t<Tuple>>::value>{});
    }
};

template<class Fn>
constexpr tuple_expand<typename std::decay<Fn>::type> make_expand_fn(Fn&& fn) {
    return tuple_expand<typename std::decay<Fn>::type>{ std::forward<Fn>(fn) };
}

} // namespace detail
} // namespace lz

#endif
