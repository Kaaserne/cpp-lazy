#pragma once

#ifndef LZ_PROCS_TO_HPP
#define LZ_PROCS_TO_HPP

#include <Lz/algorithm/copy.hpp>
#include <Lz/algorithm/for_each.hpp>
#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/compiler_config.hpp>
#include <Lz/detail/traits/enable_if.hpp>
#include <Lz/detail/traits/first_arg.hpp>
#include <Lz/detail/traits/is_iterable.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <Lz/detail/traits/remove_ref.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>
#include <Lz/detail/traits/void.hpp>
#include <Lz/procs/size.hpp>

#ifdef LZ_HAS_CONCEPTS

#include <Lz/traits/concepts.hpp>

#endif

namespace lz {
namespace detail {

template<class Iterable, class Container, class = void>
struct prealloc_container {
    LZ_CONSTEXPR_CXX_14 void try_reserve(const Iterable&, const Container&) const noexcept {
    }
};

template<class Iterable, class Container>
struct prealloc_container<Iterable, Container,
                          void_t<decltype(lz::size(std::declval<Iterable>()), std::declval<Container>().reserve(0))>> {
    LZ_CONSTEXPR_CXX_20 void try_reserve(const Iterable& iterable, Container& container) const {
        container.reserve(lz::size(iterable));
    }
};

#ifndef LZ_HAS_CXX_17

template<class Container, class = void>
struct has_insert_after : std::false_type {};

template<class Container>
struct has_insert_after<Container,
                        void_t<decltype(std::declval<Container>().insert_after(std::declval<typename Container::const_iterator>(),
                                                                               std::declval<typename Container::value_type>()))>>
    : std::true_type {};

template<class Container, class = void>
struct has_insert : std::false_type {};

template<class Container>
struct has_insert<Container, void_t<decltype(std::declval<Container>().insert(std::declval<typename Container::iterator>(),
                                                                              std::declval<typename Container::value_type>()))>>
    : std::true_type {};

template<class Container, class = void>
struct has_push_back : std::false_type {};

template<class Container>
struct has_push_back<Container,
                     void_t<decltype(std::declval<Container>().push_back(std::declval<typename Container::value_type>()))>>
    : std::true_type {};

template<class Container, class = void>
struct has_push : std::false_type {};

template<class Container>
struct has_push<Container, void_t<decltype(std::declval<Container>().push(std::declval<typename Container::value_type>()))>>
    : std::true_type {};

#else

template<class T, class = void>
inline constexpr bool has_push_back_v = false;

template<class T>
inline constexpr bool has_push_back_v<T, void_t<decltype(std::declval<T>().push_back(std::declval<typename T::value_type>()))>> =
    true;

template<class T, class = void>
inline constexpr bool has_insert_v = false;

template<class T>
inline constexpr bool has_insert_v<
    T, void_t<decltype(std::declval<T>().insert(std::declval<typename T::iterator>(), std::declval<typename T::value_type>()))>> =
    true;

template<class T, class = void>
inline constexpr bool has_insert_after_v = false;

template<class T>
inline constexpr bool
    has_insert_after_v<T, void_t<decltype(std::declval<T>().insert_after(std::declval<typename T::const_iterator>(),
                                                                         std::declval<typename T::value_type>()))>> = true;

template<class T, class = void>
inline constexpr bool has_push_v = false;

template<class T>
inline constexpr bool has_push_v<T, void_t<decltype(std::declval<T>().push(std::declval<typename T::value_type>()))>> = true;

#endif

/**
 * @brief Converts an iterable to a container, given template parameter `Container`. Can be specialized for custom containers.
 * Example:
 * ```cpp
 * template<class T>
 * class my_container {
 *     void my_inserter(T t) { ... }
 * };
 *
 * template<class T>
 * lz::custom_copier_for<my_container<T>> {
 *     template<class Iterable>
 *     void copy(Iterable&& iterable, my_container<T>& container) {
 *         // Copy the iterable to the container, for example:
 *         // Container is not reserved if it contains a reserve member
 *         for (auto&& i : iterable) {
 *             container.my_inserter(i);
 *         }
 *     }
 * };
 *
 * // or you can use enable if
 * template<class T>
 * lz::custom_copier_for<my_container<T>, std::enable_if_t_t<...>> {
 *     // Same as above
 * };
 * ```
 */
LZ_MODULE_EXPORT template<class Container, class = void>
struct custom_copier_for {
    template<class Iterable>
    LZ_CONSTEXPR_CXX_14 void copy(Iterable&& iterable, Container& container) const {
        lz::copy(std::forward<Iterable>(iterable), container.begin());
    }
};

#ifdef LZ_HAS_CXX_17

template<class Iterable, class Container>
LZ_CONSTEXPR_CXX_20 void copy_to_container(Iterable&& iterable, Container& container) {
    if constexpr (has_push_back_v<Container>) {
        prealloc_container<Iterable, Container>{}.try_reserve(iterable, container);
        lz::copy(std::forward<Iterable>(iterable), std::back_inserter(container));
    }
    else if constexpr (has_insert_v<Container>) {
        prealloc_container<Iterable, Container>{}.try_reserve(iterable, container);
        lz::copy(std::forward<Iterable>(iterable), std::inserter(container, container.begin()));
    }
    else if constexpr (has_push_v<Container>) {
        using ref = ref_iterable_t<Iterable>;
        prealloc_container<Iterable, Container>{}.try_reserve(iterable, container);
        lz::for_each(std::forward<Iterable>(iterable), [&container](ref value) { container.push(value); });
    }
    else if constexpr (has_insert_after_v<Container>) {
        using ref = ref_iterable_t<Iterable>;
        prealloc_container<Iterable, Container>{}.try_reserve(iterable, container);
        auto it = container.before_begin();
        lz::for_each(std::forward<Iterable>(iterable),
                     [&container, it](ref value) mutable { it = container.insert_after(it, value); });
    }
    else {
        custom_copier_for<Container>{}.copy(std::forward<Iterable>(iterable), container);
    }
}

#else

// Container has:
// - push_back (use push_back)
// - insert
// - has_insert_after
template<class Iterable, class Container>
LZ_CONSTEXPR_CXX_20 enable_if_t<has_push_back<Container>::value && has_insert<Container>::value &&
                                has_insert_after<Container>::value && !has_push<Container>::value>
copy_to_container(Iterable&& iterable, Container& container) {
    prealloc_container<Iterable, Container>{}.try_reserve(iterable, container);
    lz::copy(std::forward<Iterable>(iterable), std::back_inserter(container));
}

// Container has:
// - push_back (use push_back)
// - insert
template<class Iterable, class Container>
LZ_CONSTEXPR_CXX_20 enable_if_t<has_push_back<Container>::value && has_insert<Container>::value &&
                                !has_insert_after<Container>::value && !has_push<Container>::value>
copy_to_container(Iterable&& iterable, Container& container) {
    prealloc_container<Iterable, Container>{}.try_reserve(iterable, container);
    lz::copy(std::forward<Iterable>(iterable), std::back_inserter(container));
}

// Container has:
// - insert (use insert)
// - insert_after
template<class Iterable, class Container>
LZ_CONSTEXPR_CXX_20 enable_if_t<!has_push_back<Container>::value && has_insert<Container>::value &&
                                has_insert_after<Container>::value && !has_push<Container>::value>
copy_to_container(Iterable&& iterable, Container& container) {
    prealloc_container<Iterable, Container>{}.try_reserve(iterable, container);
    lz::copy(std::forward<Iterable>(iterable), std::inserter(container, container.begin()));
}

// Container has:
// - insert_after (use insert_after)
template<class Iterable, class Container>
LZ_CONSTEXPR_CXX_20 enable_if_t<!has_push_back<Container>::value && !has_insert<Container>::value &&
                                has_insert_after<Container>::value && !has_push<Container>::value>
copy_to_container(Iterable&& iterable, Container& container) {
    using ref = ref_iterable_t<Iterable>;
    prealloc_container<Iterable, Container>{}.try_reserve(iterable, container);
    auto it = container.before_begin();
    lz::for_each(std::forward<Iterable>(iterable),
                 [&container, it](ref value) mutable { it = container.insert_after(it, value); });
}

// Container has:
// - insert (use insert)
template<class Iterable, class Container>
LZ_CONSTEXPR_CXX_20 enable_if_t<!has_push_back<Container>::value && has_insert<Container>::value &&
                                !has_insert_after<Container>::value && !has_push<Container>::value>
copy_to_container(Iterable&& iterable, Container& container) {
    prealloc_container<Iterable, Container>{}.try_reserve(iterable, container);
    lz::copy(std::forward<Iterable>(iterable), std::inserter(container, container.begin()));
}

// Container has:
// - push (use push)
template<class Iterable, class Container>
LZ_CONSTEXPR_CXX_20 enable_if_t<!has_push_back<Container>::value && !has_insert<Container>::value &&
                                !has_insert_after<Container>::value && has_push<Container>::value>
copy_to_container(Iterable&& iterable, Container& container) {
    using ref = ref_iterable_t<Iterable>;
    prealloc_container<Iterable, Container>{}.try_reserve(iterable, container);
    lz::for_each(std::forward<Iterable>(iterable), [&container](ref value) { container.push(value); });
}

// Last resort: Container has no push_back, insert, insert_after, or push, use custom copier
template<class Iterable, class Container>
LZ_CONSTEXPR_CXX_20 enable_if_t<!has_push_back<Container>::value && !has_insert<Container>::value &&
                                !has_insert_after<Container>::value && !has_push<Container>::value>
copy_to_container(Iterable&& iterable, Container& container) {
    custom_copier_for<Container>{}.copy(std::forward<Iterable>(iterable), container);
}

#endif // LZ_HAS_CXX_17

template<class Container>
struct container_constructor {
#ifdef LZ_HAS_CXX_17

    template<class Iterable, class... Args>
    [[nodiscard]] static constexpr Container construct(Iterable&& iterable, Args&&... args) {
        using it = iter_t<Iterable>;
        if constexpr (std::is_constructible<Container, Iterable, remove_cvref_t<Args>...>::value) {
            return Container{ std::forward<Iterable>(iterable), std::forward<Args>(args)... };
        }
        else if constexpr (std::is_constructible_v<Container, it, sentinel_t<Iterable>, remove_cvref_t<Args>...>) {
            return Container{ detail::begin(iterable), detail::end(iterable), std::forward<Args>(args)... };
        }
        else if constexpr (is_ra_v<it> && std::is_constructible_v<Container, it, it, remove_cvref_t<Args>...>) {
            auto iter = detail::begin(iterable);
            return Container{ iter, iter + (detail::end(iterable) - detail::begin(iterable)), std::forward<Args>(args)... };
        }
        else {
            Container container{ std::forward<Args>(args)... };
            copy_to_container(std::forward<Iterable>(iterable), container);
            return container;
        }
    }

#else

    template<class Iterable, class... Args>
    using constructible_from_it_sent =
        std::is_constructible<Container, iter_t<Iterable>, sentinel_t<Iterable>, remove_cvref_t<Args>...>;

    template<class Iterable, class... Args>
    using constructible_from_it_it =
        std::is_constructible<Container, iter_t<Iterable>, iter_t<Iterable>, remove_cvref_t<Args>...>;

    template<class Iterable, class... Args>
    using constructible_from_iterable = std::is_constructible<Container, Iterable, remove_cvref_t<Args>...>;

    template<class Iterable, class... Args>
    LZ_NODISCARD static constexpr enable_if_t<constructible_from_iterable<Iterable, Args...>::value, Container>
    construct(Iterable&& iterable, Args&&... args) {
        return Container{ std::forward<Iterable>(iterable), std::forward<Args>(args)... };
    }

    template<class Iterable, class... Args>
    LZ_NODISCARD static constexpr enable_if_t<
        !constructible_from_iterable<Iterable, Args...>::value && constructible_from_it_sent<Iterable, Args...>::value, Container>
    construct(Iterable&& iterable, Args&&... args) {
        return Container{ detail::begin(iterable), detail::end(iterable), std::forward<Args>(args)... };
    }

    template<class Iterable, class... Args>
    LZ_NODISCARD static LZ_CONSTEXPR_CXX_14
        enable_if_t<!constructible_from_iterable<Iterable, Args...>::value &&
                        !constructible_from_it_sent<Iterable, Args...>::value &&
                        constructible_from_it_it<Iterable, Args...>::value && is_ra<iter_t<Iterable>>::value,
                    Container>
        construct(Iterable&& iterable, Args&&... args) {
        auto it = detail::begin(iterable);
        return Container{ it, it + (detail::end(iterable) - detail::begin(iterable)), std::forward<Args>(args)... };
    }

    template<class Iterable, class... Args>
    LZ_NODISCARD static LZ_CONSTEXPR_CXX_14
        enable_if_t<!constructible_from_iterable<Iterable, Args...>::value &&
                        !constructible_from_it_sent<Iterable, Args...>::value &&
                        (!constructible_from_it_it<Iterable, Args...>::value || !is_ra<iter_t<Iterable>>::value),
                    Container>
        construct(Iterable&& iterable, Args&&... args) {
        Container container{ std::forward<Args>(args)... };
        copy_to_container(std::forward<Iterable>(iterable), container);
        return container;
    }

#endif
};

template<class Container>
struct to_adaptor {
    using adaptor = to_adaptor<Container>;

    template<class Iterable, class... Args>
    LZ_NODISCARD constexpr Container operator()(Iterable&& iterable, Args&&... args) const {
        return container_constructor<Container>::construct(std::forward<Iterable>(iterable), std::forward<Args>(args)...);
    }
};

template<template<class...> class Container>
struct template_combiner {
    using adaptor = template_combiner<Container>;

    template<class Iterable, class... Args>
    LZ_NODISCARD constexpr Container<val_iterable_t<Iterable>, typename std::decay<Args>::type...>
    operator()(Iterable&& iterable, Args&&... args) const {
        using C = Container<val_iterable_t<Iterable>, typename std::decay<Args>::type...>;
        return to_adaptor<C>{}(std::forward<Iterable>(iterable), std::forward<Args>(args)...);
    }
};
} // namespace detail
} // namespace lz

LZ_MODULE_EXPORT namespace lz {

    // clang-format off

#ifdef LZ_HAS_CONCEPTS

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
template<class Container, class... Args>
[[nodiscard]] constexpr detail::fn_args_holder<detail::to_adaptor<Container>, std::decay_t<Args>...>
to(Args&&... args)
    requires(!lz::iterable<detail::first_arg_t<Args...>>)
{
    using Closure = detail::fn_args_holder<detail::to_adaptor<Container>, std::decay_t<Args>...>;
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
template<template<class...> class Container, class... Args>
[[nodiscard]] constexpr detail::fn_args_holder<detail::template_combiner<Container>, std::decay_t<Args>...>
to(Args&&... args)
    requires(!lz::iterable<detail::first_arg_t<Args...>>)
{
    using Closure = detail::fn_args_holder<detail::template_combiner<Container>, std::decay_t<Args>...>;
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
 * In case you have a custom container, you can specialize `lz::custom_copier_for` to copy the elements to your container.
 * This is useful if you have a custom container that requires a specific way of copying elements. You will need to specialize
 * `lz::custom_copier_for` for your custom container if all of the following are true:
 * - Your custom container does not have a constructor matching:
 *   - `your_container(iterable, args...)`
 *   - `your_container(begin, end, args...)` where begin and end may or may not be the same type
 *   - `your_container(begin, begin + size, args...)` (only if the iterable is random access)
 * - Your custom container does not have a `push_back` method
 * - Your custom container does not have an `insert` method
 * - Your custom container does not have an `insert_after` method (implicitly also requires `before_begin`)
 * - Your custom container does not have a `push` method
 * - Your custom container is not std::array
 * ```cpp
 * template<class T>
 * class custom_container {
 *    std::vector<T> _vec;
 *  public:
 *      void reserve(size_t size) {
 *         _vec.reserve(size);
 *      }
 *
 *      std::vector<T>& vec() {
 *         return _vec;
 *      }
 * };
 *
 * // Specialize `lz::custom_copier_for` for your custom container
 * template<class T>
 * struct lz::custom_copier_for<custom_container<T>> {
 *    template<class Iterable>
 *    void copy(Iterable&& iterable, custom_container<T>& container) const {
 *      // Copy the contents of the iterable to the container. Container is not reserved
 *      // if it contains a reserve member
 *      lz::copy(std::forward<Iterable>(iterable), std::back_inserter(container.vec()));
 *    }
 * ```
 * @attention Containers like `std::array `cannot be passed as `to<std::array>` since it requires a size. Instead just use
 * `to<std::array<T, N>>(iterable)` or `iterable | to<std::array<T, N>>()`
 * @tparam Container The container to convert to
 * @param iterable The iterable to convert to a container
 * @param args Args passed directly to the constructor of Container, except begin and end
 * @return The `Container`
 */
template<class Container, class... Args, class Iterable>
[[nodiscard]] constexpr Container to(Iterable&& iterable, Args&&... args)
    requires(lz::iterable<Iterable>)
{
    return detail::to_adaptor<Container>{}(std::forward<Iterable>(iterable), std::forward<Args>(args)...);
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
 * In case you have a custom container, you can specialize `lz::custom_copier_for` to copy the elements to your container.
 * This is useful if you have a custom container that requires a specific way of copying elements. You will need to specialize
 * `lz::custom_copier_for` for your custom container if all of the following are true:
 * - Your custom container does not have a constructor matching:
 *   - `your_container(iterable, args...)`
 *   - `your_container(begin, end, args...)` where begin and end may or may not be the same type
 *   - `your_container(begin, begin + size, args...)` (only if the iterable is random access)
 * - Your custom container does not have a `push_back` method
 * - Your custom container does not have an `insert` method
 * - Your custom container does not have an `insert_after` method (implicitly also requires `before_begin`)
 * - Your custom container does not have a `push` method
 * - Your custom container is not std::array
 * ```cpp
 * template<class T>
 * class custom_container {
 *    std::vector<T> _vec;
 *  public:
 *      void reserve(size_t size) {
 *         _vec.reserve(size);
 *      }
 *
 *      std::vector<T>& vec() {
 *         return _vec;
 *      }
 * };
 *
 * // Specialize `lz::custom_copier_for` for your custom container
 * template<class T>
 * struct lz::custom_copier_for<custom_container<T>> {
 *    template<class Iterable>
 *    void copy(Iterable&& iterable, custom_container<T>& container) const {
 *      // Copy the contents of the iterable to the container. Container is not reserved
 *      // if it contains a reserve member
 *      lz::copy(std::forward<Iterable>(iterable), std::back_inserter(container.vec()));
 *    }
 * ```
 * @attention Containers like `std::array `cannot be passed as `to<std::array>` since it requires a size. Instead just use
 * `to<std::array<T, N>>(iterable)` or `iterable | to<std::array<T, N>>()`
 * @tparam Container The container to convert to
 * @param iterable The iterable to convert to a container
 * @param args Args passed directly to the constructor of Container, except begin and end
 * @return The `Container`
 */
template<template<class...> class Container, class... Args, class Iterable>
[[nodiscard]] constexpr Container<detail::val_iterable_t<Iterable>, Args...>
to(Iterable&& iterable, Args&&... args)
    requires(lz::iterable<Iterable>)
{
    using Cont = Container<detail::val_iterable_t<Iterable>, std::decay_t<Args>...>;
    return to<Cont>(std::forward<Iterable>(iterable), std::forward<Args>(args)...);
}

#else

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
template<class Container, class... Args>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14
detail::enable_if_t<!detail::is_iterable<detail::first_arg_t<Args...>>::value,
    detail::fn_args_holder<detail::to_adaptor<Container>, typename std::decay<Args>::type...>>
to(Args&&... args) {
    using Closure = detail::fn_args_holder<detail::to_adaptor<Container>, typename std::decay<Args>::type...>;
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
template<template<class...> class Container, class... Args>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14
detail::enable_if_t<!detail::is_iterable<detail::first_arg_t<Args...>>::value, 
    detail::fn_args_holder<detail::template_combiner<Container>,  typename std::decay<Args>::type...>> 
to(Args&&... args) {
    using Closure = detail::fn_args_holder<detail::template_combiner<Container>,  typename std::decay<Args>::type...>;
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
 * In case you have a custom container, you can specialize `lz::custom_copier_for` to copy the elements to your container.
 * This is useful if you have a custom container that requires a specific way of copying elements. You will need to specialize
 * `lz::custom_copier_for` for your custom container if all of the following are true:
 * - Your custom container does not have a constructor matching:
 *   - `your_container(iterable, args...)`
 *   - `your_container(begin, end, args...)` where begin and end may or may not be the same type
 *   - `your_container(begin, begin + size, args...)` (only if the iterable is random access)
 * - Your custom container does not have a `push_back` method
 * - Your custom container does not have an `insert` method
 * - Your custom container does not have an `insert_after` method (implicitly also requires `before_begin`)
 * - Your custom container does not have a `push` method
 * - Your custom container is not std::array
 * ```cpp
 * template<class T>
 * class custom_container {
 *    std::vector<T> _vec;
 *  public:
 *      void reserve(size_t size) {
 *         _vec.reserve(size);
 *      }
 *
 *      std::vector<T>& vec() {
 *         return _vec;
 *      }
 * };
 *
 * // Specialize `lz::custom_copier_for` for your custom container
 * template<class T>
 * struct lz::custom_copier_for<custom_container<T>> {
 *    template<class Iterable>
 *    void copy(Iterable&& iterable, custom_container<T>& container) const {
 *      // Copy the contents of the iterable to the container. Container is not reserved
 *      // if it contains a reserve member
 *      lz::copy(std::forward<Iterable>(iterable), std::back_inserter(container.vec()));
 *    }
 * ```
 * @attention Containers like `std::array `cannot be passed as `to<std::array>` since it requires a size. Instead just use
 * `to<std::array<T, N>>(iterable)` or `iterable | to<std::array<T, N>>()`
 * @tparam Container The container to convert to
 * @param iterable The iterable to convert to a container
 * @param args Args passed directly to the constructor of Container, except begin and end
 * @return The `Container`
 */
template<class Container, class... Args, class Iterable>
LZ_NODISCARD constexpr detail::enable_if_t<detail::is_iterable<Iterable>::value, Container>
to(Iterable&& iterable, Args&&... args) {
    return detail::to_adaptor<Container>{}(std::forward<Iterable>(iterable), std::forward<Args>(args)...);
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
 * In case you have a custom container, you can specialize `lz::custom_copier_for` to copy the elements to your container.
 * This is useful if you have a custom container that requires a specific way of copying elements. You will need to specialize
 * `lz::custom_copier_for` for your custom container if all of the following are true:
 * - Your custom container does not have a constructor matching:
 *   - `your_container(iterable, args...)`
 *   - `your_container(begin, end, args...)` where begin and end may or may not be the same type
 *   - `your_container(begin, begin + size, args...)` (only if the iterable is random access)
 * - Your custom container does not have a `push_back` method
 * - Your custom container does not have an `insert` method
 * - Your custom container does not have an `insert_after` method (implicitly also requires `before_begin`)
 * - Your custom container does not have a `push` method
 * - Your custom container is not std::array
 * ```cpp
 * template<class T>
 * class custom_container {
 *    std::vector<T> _vec;
 *  public:
 *      void reserve(size_t size) {
 *         _vec.reserve(size);
 *      }
 *
 *      std::vector<T>& vec() {
 *         return _vec;
 *      }
 * };
 *
 * // Specialize `lz::custom_copier_for` for your custom container
 * template<class T>
 * struct lz::custom_copier_for<custom_container<T>> {
 *    template<class Iterable>
 *    void copy(Iterable&& iterable, custom_container<T>& container) const {
 *      // Copy the contents of the iterable to the container. Container is not reserved
 *      // if it contains a reserve member
 *      lz::copy(std::forward<Iterable>(iterable), std::back_inserter(container.vec()));
 *    }
 * ```
 * @attention Containers like `std::array `cannot be passed as `to<std::array>` since it requires a size. Instead just use
 * `to<std::array<T, N>>(iterable)` or `iterable | to<std::array<T, N>>()`
 * @tparam Container The container to convert to
 * @param iterable The iterable to convert to a container
 * @param args Args passed directly to the constructor of Container, except begin and end
 * @return The `Container`
 */
template<template<class...> class Container, class... Args, class Iterable>
LZ_NODISCARD constexpr detail::enable_if_t<detail::is_iterable<Iterable>::value, Container<detail::val_iterable_t<Iterable>, Args...>>
to(Iterable&& iterable, Args&&... args) {
    using Cont = Container<detail::val_iterable_t<Iterable>, typename std::decay<Args>::type...>;
    return to<Cont>(std::forward<Iterable>(iterable), std::forward<Args>(args)...);
}

#endif

/**
 * @brief Converts an iterable to a container, given template parameter `Container`. Can be specialized for custom containers.
 * Example:
 * ```cpp
 * template<class T>
 * class my_container {
 *     void my_inserter(T t) { ... }
 * };
 *
 * template<class T>
 * lz::custom_copier_for<my_container<T>> {
 *     template<class Iterable>
 *     void copy(Iterable&& iterable, my_container<T>& container) {
 *         // Copy the iterable to the container, for example:
 *         // Container is not reserved if it contains a reserve member
 *         for (auto&& i : iterable) {
 *             container.my_inserter(i);
 *         }
 *     }
 * };
 *
 * // or you can use enable if
 * template<class T>
 * lz::custom_copier_for<my_container<T>, std::enable_if_t<...>> {
 *     // Same as above
 * };
 * ```
 */
using detail::custom_copier_for;

} // namespace lz

#endif // LZ_PROCS_TO_HPP
