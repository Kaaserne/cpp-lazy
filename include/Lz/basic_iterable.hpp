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

namespace lz {
namespace detail {
template<class I, class S = I>
class basic_iterable_impl : public lazy_view {
    // we decay I to decay it to a pointer, if it is an array
    using iterator_type = decay_t<I>;
    using sentinel_type = decay_t<S>;

    iterator_type _begin;
    sentinel_type _end;

    using traits = std::iterator_traits<iterator_type>;

public:
    using iterator = iterator_type;
    using const_iterator = iterator_type;
    using value_type = val_t<iterator_type>;
    using sentinel = sentinel_type;

    template<LZ_CONCEPT_ITERABLE Iterable>
    constexpr basic_iterable_impl(Iterable&& iterable) :
        basic_iterable_impl{ detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)) } {
    }

    constexpr basic_iterable_impl(I begin, S end) : _begin{ std::move(begin) }, _end{ std::move(end) } {
    }

    template<class Tag = typename traits::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_ra_tag<Tag>::value, std::size_t> size() const {
        LZ_ASSERT(_end - _begin >= 0, "Incompatible iterator");
        return static_cast<std::size_t>(_end - _begin);
    }

    LZ_NODISCARD constexpr iterator begin() const& {
        return _begin;
    }

    LZ_NODISCARD constexpr sentinel end() const& {
        return _end;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return std::move(_begin);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 sentinel end() && {
        return std::move(_end);
    }
};

template<class I, class S = I>
class sized_iterable_impl : public lazy_view {
    // we decay I to decay it to a pointer, if it is an array
    using iterator_type = decay_t<I>;
    using sentinel_type = decay_t<S>;

    I _begin;
    S _end;
    std::size_t _size{};

public:
    using iterator = bounded_take_iterator<iterator_type, S>;
    using const_iterator = iterator_type;
    using value_type = val_t<iterator_type>;
    using sentinel = S;

private:
    using diff_type = typename iterator::difference_type;

public:
    constexpr sized_iterable_impl(iterator_type begin, S end) :
        _begin{ begin },
        _end{ end },
        _size{ static_cast<std::size_t>(end - begin) } {
    }

    template<class Iterable>
    constexpr sized_iterable_impl(Iterable&& iterable) :
        _begin{ detail::begin(std::forward<Iterable>(iterable)) },
        _end{ detail::end(iterable) },
        _size{ lz::size(iterable) } {
    }

    LZ_NODISCARD constexpr std::size_t size() const noexcept {
        return _size;
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
 * @brief A class that can be converted to any container. It only contains the iterators and
 * can be used in pipe expressions, converted to a container with `to<Container>()`, used in algorithms, for-each loops, etc...
 * It contains the size of the iterable.
 * @tparam It The iterator type.
 * @tparam S The sentinel type.
 */
template<class It, class S = It>
using sized_iterable = detail::sized_iterable_impl<It, S>;

/**
 * @brief A class that can be converted to any container. It only contains the iterators and
 * can be used in pipe expressions, converted to a container with `to<Container>()`, used in algorithms, for-each loops, etc...
 * It *may* contain the size of the iterable, depending on the iterator category (needs to be random access).
 * @tparam It The iterator type.
 * @tparam S The sentinel type.
 */
template<class It, class S = It>
using basic_iterable = detail::basic_iterable_impl<It, S>;

LZ_MODULE_EXPORT_SCOPE_END

namespace detail {
template<LZ_CONCEPT_ITERABLE Iterable, class Container, class = void>
struct prealloc_container {
    LZ_CONSTEXPR_CXX_14 void try_reserve(const Iterable&, const Container&) const noexcept {
    }
};

template<LZ_CONCEPT_ITERABLE Iterable, class Container>
struct prealloc_container<Iterable, Container,
                          void_t<decltype(lz::size(std::declval<Iterable>()), std::declval<Container>().reserve(0))>> {
    LZ_CONSTEXPR_CXX_20 void try_reserve(const Iterable& iterable, Container& container) const {
        container.reserve(lz::size(iterable));
    }
};

template<class Container, class = void>
struct has_insert_after : std::false_type {};

template<class Container>
struct has_insert_after<
    Container, void_t<decltype(0, std::declval<Container>().insert_after(std::declval<typename Container::const_iterator>(),
                                                                         std::declval<typename Container::value_type>()))>>
    : std::true_type {};

template<class Container, class = void>
struct has_insert : std::false_type {};

template<class Container>
struct has_insert<Container,
                  void_t<decltype(0, std::declval<Container>().insert(std::declval<typename Container::iterator>(),
                                                                      std::declval<typename Container::value_type>()))>>
    : std::true_type {};

template<class Container, class = void>
struct has_push_back : std::false_type {};

template<class Container>
struct has_push_back<Container,
                     void_t<decltype(0, std::declval<Container>().push_back(std::declval<typename Container::value_type>()))>>
    : std::true_type {};

template<class Container, class = void>
struct has_push : std::false_type {};

template<class Container>
struct has_push<Container, void_t<decltype(0, std::declval<Container>().push(std::declval<typename Container::value_type>()))>>
    : std::true_type {};

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
template<class Container, class = void>
struct custom_copier_for;

// std::array doesnt have push_back, insert, insert_after... etc, so just use copy
template<class T, std::size_t N>
struct custom_copier_for<std::array<T, N>> {
    template<class Iterable>
    LZ_CONSTEXPR_CXX_14 void copy(Iterable&& iterable, std::array<T, N>& container) const {
        lz::copy(std::forward<Iterable>(iterable), container.begin());
    }
};

#ifdef LZ_HAS_CXX_17

template<class Iterable, class Container>
LZ_CONSTEXPR_CXX_20 void copy_to_container(Iterable&& iterable, Container& container) {
    if constexpr (has_push_back<Container>::value) {
        prealloc_container<Iterable, Container>{}.try_reserve(iterable, container);
        lz::copy(std::forward<Iterable>(iterable), std::back_inserter(container));
    }
    else if constexpr (has_insert<Container>::value) {
        prealloc_container<Iterable, Container>{}.try_reserve(iterable, container);
        lz::copy(std::forward<Iterable>(iterable), std::inserter(container, container.begin()));
    }
    else if constexpr (has_insert_after<Container>::value) {
        using ref = ref_iterable_t<Iterable>;
        prealloc_container<Iterable, Container>{}.try_reserve(iterable, container);
        auto it = container.before_begin();
        lz::for_each(iterable, [&container, it](ref value) mutable { it = container.insert_after(it, value); });
    }
    else if constexpr (has_push<Container>::value) {
        using ref = ref_iterable_t<Iterable>;
        prealloc_container<Iterable, Container>{}.try_reserve(iterable, container);
        lz::for_each(iterable, [&container](ref value) { container.push(value); });
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
LZ_CONSTEXPR_CXX_20 enable_if<has_push_back<Container>::value && has_insert<Container>::value &&
                              has_insert_after<Container>::value && !has_push<Container>::value>
copy_to_container(Iterable&& iterable, Container& container) {
    prealloc_container<Iterable, Container>{}.try_reserve(iterable, container);
    lz::copy(std::forward<Iterable>(iterable), std::back_inserter(container));
}

// Container has:
// - push_back (use push_back)
// - insert
template<class Iterable, class Container>
LZ_CONSTEXPR_CXX_20 enable_if<has_push_back<Container>::value && has_insert<Container>::value &&
                              !has_insert_after<Container>::value && !has_push<Container>::value>
copy_to_container(Iterable&& iterable, Container& container) {
    prealloc_container<Iterable, Container>{}.try_reserve(iterable, container);
    lz::copy(std::forward<Iterable>(iterable), std::back_inserter(container));
}

// Container has:
// - insert (use insert)
// - insert_after
template<class Iterable, class Container>
LZ_CONSTEXPR_CXX_20 enable_if<!has_push_back<Container>::value && has_insert<Container>::value &&
                              has_insert_after<Container>::value && !has_push<Container>::value>
copy_to_container(Iterable&& iterable, Container& container) {
    prealloc_container<Iterable, Container>{}.try_reserve(iterable, container);
    lz::copy(std::forward<Iterable>(iterable), std::inserter(container, container.begin()));
}

// Container has:
// - insert_after (use insert_after)
template<class Iterable, class Container>
LZ_CONSTEXPR_CXX_20 enable_if<!has_push_back<Container>::value && !has_insert<Container>::value &&
                              has_insert_after<Container>::value && !has_push<Container>::value>
copy_to_container(Iterable&& iterable, Container& container) {
    using ref = ref_iterable_t<Iterable>;
    prealloc_container<Iterable, Container>{}.try_reserve(iterable, container);
    auto it = container.before_begin();
    lz::for_each(iterable, [&container, it](ref value) mutable { it = container.insert_after(it, value); });
}

// Container has:
// - insert (use insert)
template<class Iterable, class Container>
LZ_CONSTEXPR_CXX_20 enable_if<!has_push_back<Container>::value && has_insert<Container>::value &&
                              !has_insert_after<Container>::value && !has_push<Container>::value>
copy_to_container(Iterable&& iterable, Container& container) {
    prealloc_container<Iterable, Container>{}.try_reserve(iterable, container);
    lz::copy(std::forward<Iterable>(iterable), std::inserter(container, container.begin()));
}

// Container has:
// - push (use push)
template<class Iterable, class Container>
LZ_CONSTEXPR_CXX_20 enable_if<!has_push_back<Container>::value && !has_insert<Container>::value &&
                              !has_insert_after<Container>::value && has_push<Container>::value>
copy_to_container(Iterable&& iterable, Container& container) {
    using ref = ref_iterable_t<Iterable>;
    prealloc_container<Iterable, Container>{}.try_reserve(iterable, container);
    lz::for_each(iterable, [&container](ref value) { container.push(value); });
}

// Last resort: Container has no push_back, insert, insert_after, or push, use custom copier
template<class Iterable, class Container>
LZ_CONSTEXPR_CXX_20 enable_if<!has_push_back<Container>::value && !has_insert<Container>::value &&
                              !has_insert_after<Container>::value && !has_push<Container>::value>
copy_to_container(Iterable&& iterable, Container& container) {
    custom_copier_for<Container>{}.copy(std::forward<Iterable>(iterable), container);
}

#endif // LZ_HAS_CXX_17

template<class Container>
struct container_constructor {
    template<LZ_CONCEPT_ITERABLE Iterable, class... Args>
    using can_construct = std::is_constructible<Container, iter_t<Iterable>, sentinel_t<Iterable>, Args...>;

#ifdef LZ_HAS_CXX_17

    template<LZ_CONCEPT_ITERABLE Iterable, class... Args>
    [[nodiscard]] constexpr Container construct(Iterable&& iterable, Args&&... args) const {
        if constexpr (can_construct<Iterable, Args...>::value) {
            return Container(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                             std::forward<Args>(args)...);
        }
        else {
            Container container(std::forward<Args>(args)...);
            copy_to_container(std::forward<Iterable>(iterable), container);
            return container;
        }
    }

#else

    template<LZ_CONCEPT_ITERABLE Iterable, class... Args>
    LZ_NODISCARD constexpr enable_if<can_construct<Iterable, Args...>::value, Container>
    construct(Iterable&& iterable, Args&&... args) const {
        return Container(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                         std::forward<Args>(args)...);
    }

    template<LZ_CONCEPT_ITERABLE Iterable, class... Args>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 enable_if<!can_construct<Iterable, Args...>::value, Container>
    construct(Iterable&& iterable, Args&&... args) const {
        Container container(std::forward<Args>(args)...);
        copy_to_container(std::forward<Iterable>(iterable), container);
        return container;
    }

#endif
};

template<class Container>
struct to_adaptor {
    using adaptor = to_adaptor<Container>;

    template<LZ_CONCEPT_ITERABLE Iterable, class... Args>
    LZ_NODISCARD constexpr Container operator()(Iterable&& iterable, Args&&... args) const {
        return detail::container_constructor<Container>{}.construct(std::forward<Iterable>(iterable),
                                                                    std::forward<Args>(args)...);
    }
};

template<template<class...> class Container>
struct template_combiner {
    using adaptor = template_combiner<Container>;

    template<LZ_CONCEPT_ITERABLE Iterable, class... Args>
    LZ_NODISCARD constexpr Container<val_iterable_t<Iterable>, detail::decay_t<Args>...>
    operator()(Iterable&& iterable, Args&&... args) const {
        using C = Container<val_iterable_t<Iterable>, detail::decay_t<Args>...>;
        return to_adaptor<C>{}(std::forward<Iterable>(iterable), std::forward<Args>(args)...);
    }
};
} // namespace detail

LZ_MODULE_EXPORT_SCOPE_BEGIN

// clang-format off

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
detail::enable_if<!detail::is_iterable<detail::first_arg<Args...>>::value,
    detail::fn_args_holder<detail::to_adaptor<Container>, detail::decay_t<Args>...>>
to(Args&&... args) {
    using Closure = detail::fn_args_holder<detail::to_adaptor<Container>, detail::decay_t<Args>...>;
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
detail::enable_if<!detail::is_iterable<detail::first_arg<Args...>>::value, 
    detail::fn_args_holder<detail::template_combiner<Container>, detail::decay_t<Args>...>> 
to(Args&&... args) {
    using Closure = detail::fn_args_holder<detail::template_combiner<Container>, detail::decay_t<Args>...>;
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
 *      void reserve(std::size_t size) {
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
template<class Container, class... Args, LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD constexpr detail::enable_if<detail::is_iterable<Iterable>::value, Container>
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
 *      void reserve(std::size_t size) {
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
template<template<class...> class Container, class... Args, LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD constexpr detail::enable_if<detail::is_iterable<Iterable>::value, Container<val_iterable_t<Iterable>, Args...>>
to(Iterable&& iterable, Args&&... args) {
    using Cont = Container<val_iterable_t<Iterable>, detail::decay_t<Args>...>;
    return to<Cont>(std::forward<Iterable>(iterable), std::forward<Args>(args)...);
}

// clang-format on

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

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<LZ_CONCEPT_ITERABLE Iterable, class Adaptor>
constexpr auto operator|(Iterable&& iterable, Adaptor&& adaptor)
    -> lz::detail::enable_if<lz::detail::is_adaptor<lz::detail::remove_cvref<Adaptor>>::value &&
                                 lz::detail::is_iterable<Iterable>::value,
                             decltype(std::forward<Adaptor>(adaptor)(std::forward<Iterable>(iterable)))> {
    return std::forward<Adaptor>(adaptor)(std::forward<Iterable>(iterable));
}

LZ_MODULE_EXPORT_SCOPE_END

#endif // LZ_BASIC_ITERATOR_VIEW_HPP
