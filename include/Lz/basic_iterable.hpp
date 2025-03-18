#pragma once

#ifndef LZ_BASIC_ITERATOR_VIEW_HPP
#define LZ_BASIC_ITERATOR_VIEW_HPP

#include <Lz/algorithm.hpp>
#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterators/take.hpp>
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
    using decayed_iterator = decay_t<I>;
    using decayed_sentinel = decay_t<S>;

    decayed_iterator _begin;
    decayed_sentinel _end;

public:
    using iterator = decayed_iterator;
    using const_iterator = decayed_iterator;
    using value_type = val_t<decayed_iterator>;
    using sentinel = S;

    constexpr basic_iterable_impl() = default;

    template<LZ_CONCEPT_ITERABLE Iterable>
    constexpr basic_iterable_impl(Iterable&& iterable) :
        basic_iterable_impl{ detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)) } {
    }

    constexpr basic_iterable_impl(decayed_iterator begin, decayed_sentinel end) :
        _begin{ std::move(begin) },
        _end{ std::move(end) } {
    }

    template<class It = decayed_iterator>
    LZ_NODISCARD constexpr enable_if<is_ra<It>::value, std::size_t> size() const noexcept {
        const auto result = _end - _begin;
        return result < 0 ? static_cast<std::size_t>(-result) : static_cast<std::size_t>(result);
    }

    template<class Rhs>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend enable_if<is_iterable<decay_t<Rhs>>::value, bool>
    operator==(const basic_iterable_impl& lhs, Rhs&& rhs) {
        return lz::equal(lhs, std::forward<Rhs>(rhs));
    }

    template<class Rhs>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend enable_if<is_iterable<decay_t<Rhs>>::value, bool>
    operator!=(const basic_iterable_impl& lhs, Rhs&& rhs) {
        return !(lhs == std::forward<Rhs>(rhs));
    }

    LZ_NODISCARD constexpr decayed_iterator begin() const& {
        return _begin;
    }

    LZ_NODISCARD constexpr decayed_sentinel end() const& {
        return _end;
    }

    LZ_NODISCARD constexpr decayed_iterator begin() && {
        return std::move(_begin);
    }

    LZ_NODISCARD constexpr decayed_sentinel end() && {
        return std::move(_end);
    }
};

template<class I, class S = I>
class sized_iterable_impl : public lazy_view {
    using decayed_iterator = decay_t<I>;
    using decayed_sentinel = decay_t<S>;

    using sentinel_type =
        conditional<std::is_same<decayed_iterator, decayed_sentinel>::value, take_iterator<decayed_iterator>, decayed_sentinel>;

    take_iterator<decayed_iterator> _begin;
    sentinel_type _end;

    using difference_type = diff_type<decayed_iterator>;

public:
    using iterator = take_iterator<decayed_iterator>;
    using const_iterator = decayed_iterator;
    using value_type = val_t<decayed_iterator>;
    using sentinel = sentinel_type;

    constexpr sized_iterable_impl() = default;

    template<class Cat = typename decayed_iterator::iterator_category, enable_if<is_ra_tag<Cat>::value, int> = 0>
    constexpr sized_iterable_impl(decayed_iterator begin, decayed_iterator end) :
        _begin{ std::move(begin), end - begin },
        _end{ std::move(end), 0 } {
    }

    template<class Cat = typename decayed_iterator::iterator_category, enable_if<!is_ra_tag<Cat>::value, int> = 0>
    constexpr sized_iterable_impl(decayed_iterator begin, decayed_sentinel end) = delete; // Must be random access to get size

    constexpr sized_iterable_impl(decayed_iterator begin, std::size_t size) :
        _begin{ std::move(begin), static_cast<difference_type>(size) } {
    }

    template<class It = decayed_iterator>
    LZ_NODISCARD constexpr enable_if<std::is_same<It, decayed_sentinel>::value, std::size_t> size() const  {
        const auto result = begin()._n - end()._n;
        return result < 0 ? static_cast<std::size_t>(-result) : static_cast<std::size_t>(result);
    }

    template<class It = decayed_iterator>
    LZ_NODISCARD constexpr enable_if<!std::is_same<It, decayed_sentinel>::value, std::size_t> size() const {
        return static_cast<std::size_t>(begin()._n);
    }

    template<class Rhs>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend enable_if<is_iterable<decay_t<Rhs>>::value, bool>
    operator==(const sized_iterable_impl& lhs, Rhs&& rhs) {
        return lz::equal(lhs, std::forward<Rhs>(rhs));
    }

    template<class Rhs>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend enable_if<is_iterable<decay_t<Rhs>>::value, bool>
    operator!=(const sized_iterable_impl& lhs, Rhs&& rhs) {
        return !(lhs == std::forward<Rhs>(rhs));
    }

    LZ_NODISCARD constexpr iterator begin() const& {
        return _begin;
    }

    LZ_NODISCARD constexpr sentinel end() const& {
        return _end;
    }

    LZ_NODISCARD constexpr iterator begin() && {
        return std::move(_begin);
    }

    LZ_NODISCARD constexpr sentinel end() && {
        return std::move(_end);
    }
};
} // namespace detail

LZ_MODULE_EXPORT_SCOPE_BEGIN

/**
 * @brief A class that can be converted to any container. It is a view over an iterable, meaning it does not own the data. It
 * can be used in pipe expressions, converted to a container with `to<Container>()`, used in algorithms, for-each loops, etc...
 * It contains the size of the iterable.
 * @tparam It The iterator or iterable type.
 * @tparam S The sentinel type, if using an iterator/sentinel pair. Otherwise leave as is.
 */
template<class It, class S = It>
using sized_iterable = detail::sized_iterable_impl<It, S>;

/**
 * @brief A class that can be converted to any container. It is a view over an iterable, meaning it does not own the data. It
 * can be used in pipe expressions, converted to a container with `to<Container>()`, used in algorithms, for-each loops, etc...
 * It *may* contain the size of the iterable, depending on the iterator category or if the iterable is sized.
 * @tparam It The iterator or iterable type.
 * @tparam S The sentinel type, if using an iterator/sentinel pair. Otherwise leave as is.
 */
template<class It, class S = It>
using basic_iterable = detail::basic_iterable_impl<It, S>;

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