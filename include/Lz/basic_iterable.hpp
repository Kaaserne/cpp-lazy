#pragma once

#ifndef LZ_BASIC_ITERATOR_VIEW_HPP
#define LZ_BASIC_ITERATOR_VIEW_HPP

#include <Lz/algorithm.hpp>
#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/concepts.hpp>
#include <Lz/detail/procs.hpp>
#include <Lz/detail/traits.hpp>
#include <Lz/string_view.hpp>
#include <array>

#if defined(LZ_STANDALONE)
#ifdef LZ_HAS_FORMAT
#include <format>
#else
#include <sstream>
#endif // LZ_HAS_FORMAT
#else
#include <fmt/ostream.h>
#endif

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class I, class S = I>
class basic_iterable {
    I _begin;
    S _end;

public:
    using iterator = I;
    using sentinel = S;

    constexpr basic_iterable() = default;

    template<class It, class Se>
    constexpr basic_iterable(It&& begin, Se&& end) : _begin{ std::forward<It>(begin) }, _end{ std::forward<Se>(end) } {
    }

    template<class It = I>
    LZ_NODISCARD constexpr detail::enable_if<detail::is_ra<It>::value, std::size_t> size() const noexcept {
        return static_cast<std::size_t>(_end - _begin);
    }

    template<class Rhs>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend bool operator==(const basic_iterable& lhs, Rhs&& rhs) {
        return lz::equal(lhs, std::forward<Rhs>(rhs));
    }

    template<class Rhs>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend bool operator!=(const basic_iterable& lhs, Rhs&& rhs) {
        return !(lhs == std::forward<Rhs>(rhs));
    }

    LZ_NODISCARD constexpr I begin() const& {
        return _begin;
    }

    LZ_NODISCARD constexpr S end() const& {
        return _end;
    }

    LZ_NODISCARD constexpr I begin() && {
        return std::move(_begin);
    }

    LZ_NODISCARD constexpr S end() && {
        return std::move(_end);
    }
};

/**
 * @brief Returns an iterable to another iterable or container.
 * Example:
 * ```cpp
 * int arr[] = { 1, 2, 3, 4, 5 };
 * auto iterable = lz::to_iterable(std::begin(arr), std::end(arr));
 * ```
 * @param begin The beginning of the 'iterable'.
 * @param end The ending of the 'iterable'.
 * @return An iterable object that can be converted to an arbitrary container. Can be used in pipe expressions, converted to a
 * container with `to<Container>()`, used in algorithms, for-each loops, etc...
 */
template<class Iterator, class Sentinel>
LZ_NODISCARD constexpr basic_iterable<Iterator, Sentinel> to_iterable(Iterator&& begin, Sentinel&& end) noexcept {
    return { std::forward<Iterator>(begin), std::forward<Sentinel>(end) };
}

LZ_MODULE_EXPORT_SCOPE_END

namespace detail {
template<class Iterable, class Container, class = void>
struct prealloc_container {
    constexpr void reserve(const Iterable&, const Container&) const noexcept {
    }
};

template<class Iterable, class Container>
struct prealloc_container<Iterable, Container,
                          void_t<decltype(std::declval<Iterable>().size(), std::declval<Container>().reserve(0))>> {
    LZ_CONSTEXPR_CXX_20 void reserve(const Iterable& iterable, Container& container) const {
        container.reserve(iterable.size());
    }
};

template<class Container>
struct container_constructor {
    template<class Iterable, class... Args>
    LZ_NODISCARD constexpr enable_if<is_ra<iter_t<Iterable>>::value, Container>
    construct(Iterable&& iterable, Args&&... args) const {
        return Container(std::forward<Iterable>(iterable).begin(), std::forward<Iterable>(iterable).end(),
                         std::forward<Args>(args)...);
    }

    template<class Iterable, class... Args>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 enable_if<!is_ra<iter_t<Iterable>>::value, Container>
    construct(Iterable&& iterable, Args&&... args) const {
        Container container(std::forward<Args>(args)...);
        constexpr prealloc_container<Iterable, Container> reserver{};
        reserver.reserve(iterable, container);
        lz::copy(std::forward<Iterable>(iterable), std::inserter(container, std::begin(container)));
        return container;
    }
};

template<class ValueType, std::size_t N>
struct container_constructor<std::array<ValueType, N>> {
    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 std::array<ValueType, N> construct(Iterable&& iterable) const {
        std::array<ValueType, N> arr{};
        lz::copy(std::forward<Iterable>(iterable), arr.begin());
        return arr;
    }
};
} // namespace detail

template<class Container>
struct to_adaptor {
    using adaptor = to_adaptor<Container>;

    template<class Iterable, class... Args>
    LZ_NODISCARD constexpr Container operator()(Iterable&& iterable, Args&&... args) const {
        constexpr detail::container_constructor<Container> ctor;
        return ctor.construct(std::forward<Iterable>(iterable), std::forward<Args>(args)...);
    }
};

template<template<class...> class Container>
struct template_combiner {
    using adaptor = template_combiner<Container>;

    template<class Iterable, class... Args, class Cont = Container<val_iterable_t<Iterable>, detail::decay_t<Args>...>>
    LZ_NODISCARD constexpr Cont operator()(Iterable&& iterable, Args&&... args) const {
        constexpr to_adaptor<Cont> to_adaptor{};
        return to_adaptor(std::forward<Iterable>(iterable), std::forward<Args>(args)...);
    }
};

LZ_MODULE_EXPORT_SCOPE_BEGIN

/**
 * @brief Converts an iterable to a container, given template parameter `Container`. Can be used in a pipe expression.
 * Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto list = vec | lz::to<std::list>(); // arr = { 1, 2, 3, 4, 5 }
 * // or
 * auto list = vec | lz::to<std::list>>(std::allocator<int>{}); // arr = { 1, 2, 3, 4, 5 }
 * // or
 * auto list = vec | lz::to<std::list<int>>(); // arr = { 1, 2, 3, 4, 5 }
 * // or
 * auto list = vec | lz::to<std::list<int>>(std::allocator<int>{}); // arr = { 1, 2, 3, 4, 5 }
 * // etc...
 * ```
 * @attention Containers like `std::array `cannot be passed as `to<std::array>` since it requires a size. Instead just use
 * `to<std::array<T, N>>(iterable)` or `iterable | to<std::array<T, N>>()`
 * @tparam Container The container to convert to
 * @param args Args passed directly to the constructor of Container, except begin and end
 * @return The `Container`
 */
template<class Container, class... Args, class = detail::enable_if<!detail::is_iterable<detail::first_arg<Args...>>::value>,
         class Closure = detail::fn_args_holder<to_adaptor<Container>, detail::decay_t<Args>...>>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 Closure to(Args&&... args) {
    return Closure{ std::forward<Args>(args)... };
}

/**
 * @brief Converts an iterable to a container, given template parameter `Container`. Can be used in a pipe expression.
 * Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto list = vec | lz::to<std::list>(); // arr = { 1, 2, 3, 4, 5 }
 * // or
 * auto list = vec | lz::to<std::list>>(std::allocator<int>{}); // arr = { 1, 2, 3, 4, 5 }
 * // or
 * auto list = vec | lz::to<std::list<int>>(); // arr = { 1, 2, 3, 4, 5 }
 * // or
 * auto list = vec | lz::to<std::list<int>>(std::allocator<int>{}); // arr = { 1, 2, 3, 4, 5 }
 * // etc...
 * ```
 * @attention Containers like `std::array `cannot be passed as `to<std::array>` since it requires a size. Instead just use
 * `to<std::array<T, N>>(iterable)` or `iterable | to<std::array<T, N>>()`
 * @tparam Container The container to convert to
 * @param args Args passed directly to the constructor of Container, except begin and end
 * @return The `Container`
 */
template<template<class...> class Container, class... Args,
         class = detail::enable_if<!detail::is_iterable<detail::first_arg<Args...>>::value>,
         class Closure = detail::fn_args_holder<template_combiner<Container>, detail::decay_t<Args>...>>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 Closure to(Args&&... args) {
    return Closure{ std::forward<Args>(args)... };
}

/**
 * @brief Converts an iterable to a container, given template parameter `Container`.
 * Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto list = lz::to<std::list>(vec); // arr = { 1, 2, 3, 4, 5 }
 * // or
 * auto list = lz::to<std::list>(vec, std::allocator<int>{}); // arr = { 1, 2, 3, 4, 5 }
 * // or
 * auto list = lz::to<std::list<int>>(vec); // arr = { 1, 2, 3, 4, 5 }
 * // or
 * auto list = lz::to<std::list<int>>(vec, std::allocator<int>{}); // arr = { 1, 2, 3, 4, 5 }
 * // etc...
 * ```
 * @attention Containers like `std::array `cannot be passed as `to<std::array>` since it requires a size. Instead just use
 * `to<std::array<T, N>>(iterable)` or `iterable | to<std::array<T, N>>()`
 * @tparam Container The container to convert to
 * @param iterable The iterable to convert to a container
 * @param args Args passed directly to the constructor of Container, except begin and end
 * @return The `Container`
 */
template<class Container, class... Args, class Iterable, class = detail::enable_if<detail::is_iterable<Iterable>::value>>
LZ_NODISCARD constexpr Container to(Iterable&& iterable, Args&&... args) {
    constexpr to_adaptor<Container> to{};
    return to(std::forward<Iterable>(iterable), std::forward<Args>(args)...);
    // return to<Container>(std::forward<Args>(args)...)(std::forward<Iterable>(iterable));
}

/**
 * @brief Converts an iterable to a container, given template parameter `Container`.
 * Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto list = lz::to<std::list>(vec); // arr = { 1, 2, 3, 4, 5 }
 * // or
 * auto list = lz::to<std::list>(vec, std::allocator<int>{}); // arr = { 1, 2, 3, 4, 5 }
 * // or
 * auto list = lz::to<std::list<int>>(vec); // arr = { 1, 2, 3, 4, 5 }
 * // or
 * auto list = lz::to<std::list<int>>(vec, std::allocator<int>{}); // arr = { 1, 2, 3, 4, 5 }
 * // etc...
 * ```
 * @attention Containers like `std::array `cannot be passed as `to<std::array>` since it requires a size. Instead just use
 * `to<std::array<T, N>>(iterable)` or `iterable | to<std::array<T, N>>()`
 * @tparam Container The container to convert to
 * @param iterable The iterable to convert to a container
 * @param args Args passed directly to the constructor of Container, except begin and end
 * @return The `Container`
 */
template<template<class...> class Container, class... Args, class Iterable,
         class = detail::enable_if<detail::is_iterable<Iterable>::value>,
         class Cont = Container<val_iterable_t<Iterable>, Args...>>
LZ_NODISCARD constexpr Cont to(Iterable&& iterable, Args&&... args) {
    return to<Cont>(std::forward<Iterable>(iterable), std::forward<Args>(args)...);
}

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

// TODO add formatters

// #if defined(LZ_HAS_FORMAT) && defined(LZ_STANDALONE)
// template<class Iterable>
// struct std::formatter<Iterable,
//                       lz::detail::enable_if< // Enable if Iterable is base of BasicIterable
//                           std::is_base_of<lz::detail::BasicIterable<lz::iter_t<Iterable>>, Iterable>::value, char>>
//     : std::formatter<std::string> {
//     using InnerIter = lz::detail::BasicIterable<lz::iter_t<Iterable>>;

//     template<class FormatCtx>
//     auto format(const InnerIter& it, FormatCtx& ctx) const -> decltype(ctx.out()) {
//         return std::formatter<std::string>::format(it.to_string(), ctx);
//     }
// };
// #elif !defined(LZ_STANDALONE) && !defined(LZ_HAS_FORMAT)
// template<class Iterable>
// struct fmt::formatter<
//     Iterable, lz::detail::enable_if<std::is_base_of<lz::detail::basic_iterable<lz::iter_t<Iterable>>, Iterable>::value, char>>
//     : fmt::ostream_formatter {};
// #endif

LZ_MODULE_EXPORT_SCOPE_END

#endif // LZ_BASIC_ITERATOR_VIEW_HPP