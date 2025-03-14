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
#include <iostream>

#if !defined(LZ_STANDALONE)

#include <fmt/format.h>

#elif defined(LZ_HAS_FORMAT)

#include <cstdio>
#include <format>

#else

#include <iostream>
#include <sstream>

#endif

namespace lz {
namespace detail {
template<class I, class S = I>
class basic_iterable_impl : public lazy_view {
    I _begin;
    S _end;

public:
    using iterator = I;
    using const_iterator = I;
    using value_type = val_t<I>;
    using sentinel = S;

    constexpr basic_iterable_impl() = default;

    constexpr basic_iterable_impl(I begin, S end) : _begin{ std::move(begin) }, _end{ std::move(end) } {
    }

    template<class It = I>
    LZ_NODISCARD constexpr detail::enable_if<detail::is_ra<It>::value, std::size_t> size() const noexcept {
        return static_cast<std::size_t>(_end - _begin);
    }

    template<class Rhs>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend detail::enable_if<detail::is_iterable<detail::decay_t<Rhs>>::value, bool>
    operator==(const basic_iterable_impl& lhs, Rhs&& rhs) {
        return lz::equal(lhs, std::forward<Rhs>(rhs));
    }

    template<class Rhs>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend detail::enable_if<detail::is_iterable<detail::decay_t<Rhs>>::value, bool>
    operator!=(const basic_iterable_impl& lhs, Rhs&& rhs) {
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

template<class I, class S>
class sized_iterable : public basic_iterable_impl<I, S> {
    using base = basic_iterable_impl<I, S>;
    std::size_t _size{};

public:
    constexpr sized_iterable() = default;

    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_CONSTEXPR_CXX_20 sized_iterable(Iterable&& iterable) :
        base{ std::forward<Iterable>(iterable) },
        _size{ static_cast<std::size_t>(lz::size(iterable)) } {
    }

    constexpr sized_iterable(I begin, S end, std::size_t size) : base{ std::move(begin), std::move(end) }, _size{ size } {
    }

    LZ_NODISCARD constexpr std::size_t size() const noexcept {
        return _size;
    }
};

template<bool>
struct iterable_spec_choice;

template<>
struct iterable_spec_choice<true /* is iterable */> {
    template<LZ_CONCEPT_ITERABLE Iterable, class>
    using type = conditional<sized<Iterable>::value, sized_iterable<iter_t<Iterable>, sentinel_t<Iterable>>,
                             basic_iterable_impl<iter_t<Iterable>, sentinel_t<Iterable>>>;
};

template<>
struct iterable_spec_choice<false /* is iterable */> {
    template<class I, class S>
    using type = basic_iterable_impl<I, S>;
};
} // namespace detail

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class It, class S = It>
using basic_iterable = typename detail::iterable_spec_choice<detail::is_iterable<It>::value>::template type<It, S>;

/**
 * @brief Returns a sized iterable (if `Iterator` is random access) to another iterable or container if `Iterator` is random
 * access. Example:
 * ```cpp
 * int arr[] = { 1, 2, 3, 4, 5 };
 * auto iterable = lz::to_iterable(std::begin(arr), std::end(arr));
 * // iterable contains method .size()
 *
 * std::list<int> lst = { 1, 2, 3, 4, 5 };
 * auto iterable = lz::to_iterable(lst.begin(), lst.end());
 * // iterable does not contain method .size()
 * ```
 * @param begin The beginning of the 'iterable'.
 * @param end The ending of the 'iterable'.
 * @return An iterable object that can be converted to an arbitrary container. Can be used in pipe expressions, converted to a
 * container with `to<Container>()`, used in algorithms, for-each loops, etc...
 */
template<LZ_CONCEPT_ITERATOR Iterator, class S>
LZ_NODISCARD constexpr basic_iterable<detail::decay_t<Iterator>, detail::decay_t<S>> to_iterable(Iterator&& begin, S&& end) {
    return { std::forward<Iterator>(begin), std::forward<S>(end) };
}

/**
 * @brief Returns a non sized iterable to another iterable or container.
 * Example:
 * ```cpp
 * int arr[] = { 1, 2, 3, 4, 5 };
 * auto f = lz::filter(arr, [](int i) { return i % 2 == 0; });
 * auto iterable = lz::to_iterable(f);
 * // iterable does not contain a .size() method
 * ```
 * @param iterable The container or iterable to convert to an iterable.
 * @return An iterable object that can be converted to an arbitrary container. Can be used in pipe expressions, converted to a
 * container with `to<Container>()`, used in algorithms, for-each loops, etc...
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD constexpr detail::enable_if<!sized<Iterable>::value, basic_iterable<Iterable>> to_iterable(Iterable&& iterable) {
    return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)) };
}

/**
 * @brief Returns a sized iterable to another iterable or container.
 * Example:
 * ```cpp
 * int arr[] = { 1, 2, 3, 4, 5 };
 * auto iterable = lz::to_iterable(arr);
 * // iterable contains a .size() method
 * ```
 * @param iterable The container or iterable to convert to an iterable.
 * @return An iterable object that can be converted to an arbitrary container. Can be used in pipe expressions, converted to a
 * container with `to<Container>()`, used in algorithms, for-each loops, etc...
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD constexpr detail::enable_if<sized<Iterable>::value, basic_iterable<Iterable>> to_iterable(Iterable&& iterable) {
    return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)), lz::size(iterable) };
}

LZ_MODULE_EXPORT_SCOPE_END

namespace detail {
template<LZ_CONCEPT_ITERABLE Iterable, class Container, class = void>
struct prealloc_container {
    constexpr void try_reserve(const Iterable&, const Container&) const noexcept {
    }
};

template<LZ_CONCEPT_ITERABLE Iterable, class Container>
struct prealloc_container<Iterable, Container,
                          void_t<decltype(lz::size(std::declval<Iterable>()), std::declval<Container>().reserve(0))>> {
    LZ_CONSTEXPR_CXX_20 void try_reserve(const Iterable& iterable, Container& container) const {
        container.reserve(lz::size(iterable));
    }
};

template<class Container>
struct container_constructor {
    template<class Container, LZ_CONCEPT_ITERABLE Iterable, class... Args>
    using can_construct = std::is_constructible<Container, iter_t<Iterable>, sentinel_t<Iterable>, Args...>;

    template<LZ_CONCEPT_ITERABLE Iterable, class... Args>
    LZ_NODISCARD constexpr enable_if<can_construct<Container, Iterable, Args...>::value, Container>
    construct(Iterable&& iterable, Args&&... args) const {
        return Container(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                         std::forward<Args>(args)...);
    }

    template<LZ_CONCEPT_ITERABLE Iterable, class... Args>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 enable_if<!can_construct<Container, Iterable, Args...>::value, Container>
    construct(Iterable&& iterable, Args&&... args) const {
        Container container(std::forward<Args>(args)...);
        prealloc_container<Iterable, Container>{}.try_reserve(iterable, container);
        lz::copy(std::forward<Iterable>(iterable), std::inserter(container, std::begin(container)));
        return container;
    }
};

template<class ValueType, std::size_t N>
struct container_constructor<std::array<ValueType, N>> {
    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 std::array<ValueType, N> construct(Iterable&& iterable) const {
        std::array<ValueType, N> arr{};
        lz::copy(std::forward<Iterable>(iterable), arr.begin());
        return arr;
    }
};

struct iterable_formatter {
    using adaptor = iterable_formatter;

#if !defined(LZ_STANDALONE) || defined(LZ_HAS_FORMAT)

    template<LZ_CONCEPT_ITERABLE Iterable>
    std::ostream&
    operator()(const Iterable& iterable, std::ostream& stream, const char* separator = ", ", const char* format = "{}") const {
        return stream << (*this)(iterable, separator, format);
    }

#else

    template<LZ_CONCEPT_ITERABLE Iterable>
    std::ostream& operator()(const Iterable& iterable, std::ostream& stream, const char* separator = ", ") const {
        auto begin = std::begin(iterable);
        auto end = std::end(iterable);
        if (begin == end) {
            return stream;
        }
        stream << *begin++;
        for (; begin != end; ++begin) {
            stream << separator << *begin;
        }

        return stream;
    }

#endif

#if !defined(LZ_STANDALONE) || defined(LZ_HAS_FORMAT)

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, const char*, const char*>
    operator()(const char* separator = ", ", const char* format = "{}") const {
        return { separator, format };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, std::ostream&, const char*, const char*>
    operator()(std::ostream& stream, const char* separator = ", ", const char* format = "{}") const {
        return { stream, separator, format };
    }

#else

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, const char*> operator()(const char* separator = ", ") const {
        return { separator };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, std::ostream&, const char*>
    operator()(std::ostream& stream, const char* separator = ", ") const {
        return { stream, separator };
    }

#endif

#if !defined(LZ_STANDALONE)

    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD std::string operator()(const Iterable& iterable, const char* separator = ", ", const char* format = "{}") const {
#if FMT_VERSION >= 80000

        return fmt::format(fmt::runtime(format), fmt::join(iterable, separator));

#else

        return fmt::format(format, fmt::join(iterable, separator));

#endif
    }

#elif defined(LZ_HAS_FORMAT)

    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD std::string operator()(const Iterable& iterable, const char* separator = ", ", const char* format = "{}") const {
        auto begin = std::begin(iterable);
        auto end = std::end(iterable);
        if (begin == end) {
            return "";
        }

        std::string result;
        auto back_inserter = std::back_inserter(result);

        std::vformat_to(back_inserter, format, std::make_format_args(*begin));
        ++begin;

        std::string_view fmt{ format };
        std::string_view sep{ separator };

        for (; begin != end; ++begin) {
            std::vformat_to(back_inserter, sep, std::make_format_args());
            std::vformat_to(back_inserter, fmt, std::make_format_args(*begin));
        }

        return result;
    }

#else

    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD std::string operator()(const Iterable& iterable, const char* separator = ", ") const {
        std::ostringstream oss;
        (*this)(iterable, oss, separator);
        return oss.str();
    }

#endif
};

struct iterable_printer {
    using adaptor = iterable_printer;

#if !defined(LZ_STANDALONE) || defined(LZ_HAS_FORMAT)

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, const char*, const char*>
    operator()(const char* separator = ", ", const char* format = "{}") const {
        return { separator, format };
    }

#else

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, const char*> operator()(const char* separator = ", ") const {
        return { separator };
    }

#endif

#if !defined(LZ_STANDALONE)

    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD void operator()(const Iterable& iterable, const char* separator = ", ", const char* format = "{}") const {
#if FMT_VERSION >= 80000

        fmt::print(fmt::runtime(format), fmt::join(iterable, separator));

#else

        fmt::print(format, fmt::join(iterable, separator));

#endif
    }

#elif defined(LZ_HAS_FORMAT)

    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD void operator()(const Iterable& iterable, const char* separator = ", ", const char* format = "{}") const {
        constexpr iterable_formatter iter_formatter{};
        std::string result = iter_formatter(iterable, separator, format);
        std::fputs(result.c_str(), stdout);
    }

#else

    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD void operator()(const Iterable& iterable, const char* separator = ", ") const {
        constexpr iterable_formatter formatter{};
        formatter(iterable, std::cout, separator);
    }

#endif
};
} // namespace detail

template<class Container>
struct to_adaptor {
    using adaptor = to_adaptor<Container>;

    template<LZ_CONCEPT_ITERABLE Iterable, class... Args>
    LZ_NODISCARD constexpr Container operator()(Iterable&& iterable, Args&&... args) const {
        constexpr detail::container_constructor<Container> ctor;
        return ctor.construct(std::forward<Iterable>(iterable), std::forward<Args>(args)...);
    }
};

template<template<class...> class Container>
struct template_combiner {
    using adaptor = template_combiner<Container>;

    template<LZ_CONCEPT_ITERABLE Iterable, class... Args, class Cont = Container<val_iterable_t<Iterable>, detail::decay_t<Args>...>>
    LZ_NODISCARD constexpr Cont operator()(Iterable&& iterable, Args&&... args) const {
        constexpr to_adaptor<Cont> to_adaptor{};
        return to_adaptor(std::forward<Iterable>(iterable), std::forward<Args>(args)...);
    }
};

LZ_MODULE_EXPORT_SCOPE_BEGIN

// TODO docs

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
 * auto list = vec | lz::to<std::list>(); // { 1, 2, 3, 4, 5 }
 * // or
 * auto list = vec | lz::to<std::list>>(std::allocator<int>{}); // { 1, 2, 3, 4, 5 }
 * // or
 * auto list = vec | lz::to<std::list<int>>(); // { 1, 2, 3, 4, 5 }
 * // or
 * auto list = vec | lz::to<std::list<int>>(std::allocator<int>{}); // { 1, 2, 3, 4, 5 }
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
 * auto list = vec | lz::to<std::list>(); // { 1, 2, 3, 4, 5 }
 * // or
 * auto list = vec | lz::to<std::list>>(std::allocator<int>{}); // { 1, 2, 3, 4, 5 }
 * // or
 * auto list = vec | lz::to<std::list<int>>(); // { 1, 2, 3, 4, 5 }
 * // or
 * auto list = vec | lz::to<std::list<int>>(std::allocator<int>{}); // { 1, 2, 3, 4, 5 }
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
 * auto list = lz::to<std::list>(vec); // { 1, 2, 3, 4, 5 }
 * // or
 * auto list = lz::to<std::list>(vec, std::allocator<int>{}); // { 1, 2, 3, 4, 5 }
 * // or
 * auto list = lz::to<std::list<int>>(vec); // { 1, 2, 3, 4, 5 }
 * // or
 * auto list = lz::to<std::list<int>>(vec, std::allocator<int>{}); // { 1, 2, 3, 4, 5 }
 * // etc...
 * ```
 * @attention Containers like `std::array `cannot be passed as `to<std::array>` since it requires a size. Instead just use
 * `to<std::array<T, N>>(iterable)` or `iterable | to<std::array<T, N>>()`
 * @tparam Container The container to convert to
 * @param iterable The iterable to convert to a container
 * @param args Args passed directly to the constructor of Container, except begin and end
 * @return The `Container`
 */
template<class Container, class... Args, LZ_CONCEPT_ITERABLE Iterable,
         class = detail::enable_if<detail::is_iterable<Iterable>::value>>
LZ_NODISCARD constexpr Container to(Iterable&& iterable, Args&&... args) {
    constexpr to_adaptor<Container> to{};
    return to(std::forward<Iterable>(iterable), std::forward<Args>(args)...);
}

/**
 * @brief Converts an iterable to a container, given template parameter `Container`.
 * Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto list = lz::to<std::list>(vec); // { 1, 2, 3, 4, 5 }
 * // or
 * auto list = lz::to<std::list>(vec, std::allocator<int>{}); // { 1, 2, 3, 4, 5 }
 * // or
 * auto list = lz::to<std::list<int>>(vec); // { 1, 2, 3, 4, 5 }
 * // or
 * auto list = lz::to<std::list<int>>(vec, std::allocator<int>{}); // { 1, 2, 3, 4, 5 }
 * // etc...
 * ```
 * @attention Containers like `std::array `cannot be passed as `to<std::array>` since it requires a size. Instead just use
 * `to<std::array<T, N>>(iterable)` or `iterable | to<std::array<T, N>>()`
 * @tparam Container The container to convert to
 * @param iterable The iterable to convert to a container
 * @param args Args passed directly to the constructor of Container, except begin and end
 * @return The `Container`
 */
template<template<class...> class Container, class... Args, LZ_CONCEPT_ITERABLE Iterable,
         class = detail::enable_if<detail::is_iterable<Iterable>::value>,
         class Cont = Container<val_iterable_t<Iterable>, Args...>>
LZ_NODISCARD constexpr Cont to(Iterable&& iterable, Args&&... args) {
    return to<Cont>(std::forward<Iterable>(iterable), std::forward<Args>(args)...);
}

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

LZ_MODULE_EXPORT_SCOPE_BEGIN

/**
 * @brief Streams a `lz` iterable to an output stream. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto filter = lz::filter(vec, [](int i) { return i % 2 == 0; });
 * std::cout << filter; // 2, 4
 * ```
 *
 * @param stream The stream to output to
 * @param iterable The `lz` iterable to output
 */
template<LZ_CONCEPT_ITERABLE Iterable>
lz::detail::enable_if<std::is_base_of<lz::lazy_view, Iterable>::value, std::ostream&>
operator<<(std::ostream& stream, const Iterable& iterable) {
    return lz::format(iterable, stream);
}

template<LZ_CONCEPT_ITERABLE Iterable, class Adaptor>
constexpr auto operator|(Iterable&& iterable, Adaptor&& adaptor)
    -> lz::detail::enable_if<lz::detail::is_adaptor<Adaptor>::value && lz::detail::is_iterable<Iterable>::value,
                             decltype(std::forward<Adaptor>(adaptor)(std::forward<Iterable>(iterable)))> {
    return std::forward<Adaptor>(adaptor)(std::forward<Iterable>(iterable));
}

LZ_MODULE_EXPORT_SCOPE_END

#endif // LZ_BASIC_ITERATOR_VIEW_HPP