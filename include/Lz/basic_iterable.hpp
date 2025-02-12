#pragma once

#ifndef LZ_BASIC_ITERATOR_VIEW_HPP
#define LZ_BASIC_ITERATOR_VIEW_HPP

#include <Lz/algorithm.hpp>
#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/concepts.hpp>
#include <Lz/detail/procs.hpp>
#include <Lz/detail/traits.hpp>
#include <Lz/string_view.hpp>
#include <array>
#include <ostream>

#if !defined(LZ_STANDALONE)
#include <fmt/format.h>
#endif

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class I, class S = I>
class basic_iterable : public lazy_view {
    I _begin;
    S _end;

public:
    using iterator = I;
    using const_iterator = I;
    using value_type = val_t<I>;
    using sentinel = S;

    constexpr basic_iterable() = default;

    constexpr basic_iterable(I begin, S end) : _begin{ std::move(begin) }, _end{ std::move(end) } {
    }

    template<class It = I>
    LZ_NODISCARD constexpr detail::enable_if<detail::is_ra<It>::value, std::size_t> size() const noexcept {
        return static_cast<std::size_t>(_end - _begin);
    }

    template<class Rhs>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend detail::enable_if<detail::is_iterable<detail::decay_t<Rhs>>::value, bool>
    operator==(const basic_iterable& lhs, Rhs&& rhs) {
        return lz::equal(lhs, std::forward<Rhs>(rhs));
    }

    template<class Rhs>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend detail::enable_if<detail::is_iterable<detail::decay_t<Rhs>>::value, bool>
    operator!=(const basic_iterable& lhs, Rhs&& rhs) {
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
template<class Iterator, class S>
LZ_NODISCARD constexpr basic_iterable<detail::decay_t<Iterator>, detail::decay_t<S>>
to_iterable(Iterator&& begin, S&& end) noexcept {
    return { std::forward<Iterator>(begin), std::forward<S>(end) };
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
                          void_t<decltype(lz::size(std::declval<Iterable>()), std::declval<Container>().reserve(0))>> {
    LZ_CONSTEXPR_CXX_20 void reserve(const Iterable& iterable, Container& container) const {
        container.reserve(lz::size(iterable));
    }
};

template<class Container>
struct container_constructor {
    template<class Container, class Iterable, class... Args>
    using can_construct = std::is_constructible<Container, iter_t<Iterable>, sentinel_t<Iterable>, Args...>;

    template<class Iterable, class... Args>
    LZ_NODISCARD constexpr enable_if<can_construct<Container, Iterable, Args...>::value, Container>
    construct(Iterable&& iterable, Args&&... args) const {
        return Container(std::begin(iterable), std::end(iterable), std::forward<Args>(args)...);
    }

    template<class Iterable, class... Args>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 enable_if<!can_construct<Container, Iterable, Args...>::value, Container>
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

struct iterable_formatter {
    using adaptor = iterable_formatter;

#if !defined(LZ_STANDALONE)

    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20
        enable_if<!std::is_same<Iterable, lz::string_view>::value && !std::is_array<Iterable>::value, std::string>
        operator()(const Iterable& iterable, lz::string_view formatter = "{}", lz::string_view delimiter = ", ") const {
        return fmt::format(formatter, fmt::join(iterable, delimiter));
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 fn_args_holder<adaptor, lz::string_view, lz::string_view>
    operator()(lz::string_view formatter = "{}", lz::string_view delimiter = ", ") const {
        return { formatter, delimiter };
    }

#elif

    // TODO

#endif
};

struct iterable_printer {
    using adaptor = iterable_printer;

#if !defined(LZ_STANDALONE)

    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 enable_if<!std::is_same<Iterable, lz::string_view>::value>
    operator()(const Iterable& iterable, lz::string_view formatter = "{}", lz::string_view delimiter = ", ") const {
        fmt::print(formatter, fmt::join(iterable, delimiter));
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 fn_args_holder<adaptor, lz::string_view, lz::string_view>
    operator()(lz::string_view formatter = "{}", lz::string_view delimiter = ", ") const {
        return { formatter, delimiter };
    }

#elif

    // TODO

#endif
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

#ifdef LZ_HAS_CXX_11

static const detail::iterable_printer print{};

static const detail::iterable_formatter format{};

#else

LZ_INLINE_VAR constexpr detail::iterable_printer print{};

LZ_INLINE_VAR constexpr detail::iterable_formatter format{};

#endif

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

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterable>
lz::detail::enable_if<std::is_base_of<lz::lazy_view, Iterable>::value, std::ostream&>
operator<<(std::ostream& stream, const Iterable& iterable) {
    auto begin = iterable.begin();
    auto end = iterable.end();

    if (begin == end) {
        return stream;
    }

    stream << *begin;
    ++begin;

    while (begin != end) {
        stream << ", " << *begin;
        ++begin;
    }

    return stream;
}

template<class Iterable, class Adaptor>
LZ_NODISCARD constexpr auto operator|(Iterable&& iterable, Adaptor&& adaptor)
    -> lz::detail::enable_if<lz::detail::is_adaptor<Adaptor>::value, decltype(adaptor(std::forward<Iterable>(iterable)))> {
    return std::forward<Adaptor>(adaptor)(std::forward<Iterable>(iterable));
}

LZ_MODULE_EXPORT_SCOPE_END

#endif // LZ_BASIC_ITERATOR_VIEW_HPP