#pragma once

#ifndef LZ_BASIC_ITERATOR_VIEW_HPP
#define LZ_BASIC_ITERATOR_VIEW_HPP

#include <Lz/algorithm.hpp>
#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/procs.hpp>
#include <Lz/detail/traits.hpp>

namespace lz {
namespace detail {
template<class I, class S = I>
class basic_iterable_impl : public lazy_view {
    // we decay I to decay it to a pointer, if it is an array
    using iterator_type = decay_t<I>;
    using sentinel_type = decay_t<S>;

    iterator_type _begin{};
    sentinel_type _end{};

    using traits = std::iterator_traits<iterator_type>;

public:
    using iterator = iterator_type;
    using const_iterator = iterator_type;
    using value_type = val_t<iterator_type>;
    using sentinel = sentinel_type;

    constexpr basic_iterable_impl(const basic_iterable_impl&) = default;
    LZ_CONSTEXPR_CXX_14 basic_iterable_impl& operator=(const basic_iterable_impl&) = default;

#ifdef LZ_HAS_CONCEPTS

    constexpr basic_iterable_impl()
        requires(std::default_initializable<iterator_type> && std::default_initializable<sentinel_type>)
    = default;

#else

    template<class B = decltype(_begin),
             class = enable_if_t<std::is_default_constructible<B>::value && std::is_default_constructible<sentinel_type>::value>>
    constexpr basic_iterable_impl() noexcept(std::is_nothrow_default_constructible<B>::value &&
                                             std::is_nothrow_default_constructible<sentinel_type>::value) {
    }

#endif

    template<class Iterable>
    explicit constexpr basic_iterable_impl(Iterable&& iterable) :
        basic_iterable_impl{ detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)) } {
    }

    constexpr basic_iterable_impl(iterator_type begin, sentinel_type end) : _begin{ std::move(begin) }, _end{ std::move(end) } {
    }

#ifdef LZ_HAS_CONCEPTS

    [[nodiscard]] constexpr size_t size() const
        requires(is_ra_tag_v<typename traits::iterator_category>)
    {
        LZ_ASSERT(_end - _begin >= 0, "Incompatible iterator");
        return static_cast<size_t>(_end - _begin);
    }

#else

    template<class Tag = typename traits::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<is_ra_tag<Tag>::value, size_t> size() const {
        LZ_ASSERT(_end - _begin >= 0, "Incompatible iterator");
        return static_cast<size_t>(_end - _begin);
    }

#endif

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
    size_t _size{};

    friend struct common_adaptor;

    constexpr sized_iterable_impl(iterator_type begin, sentinel_type end, size_t size) :
        _begin{ std::move(begin) },
        _end{ std::move(end) },
        _size{ size } {
    }

public:
    using iterator = I;
    using const_iterator = iterator_type;
    using value_type = val_t<iterator_type>;
    using sentinel = S;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr sized_iterable_impl()
        requires(std::default_initializable<iterator_type> && std::default_initializable<sentinel_type>)
    = default;

#else

    template<class B = decltype(_begin),
             class = enable_if_t<std::is_default_constructible<B>::value && std::is_default_constructible<sentinel_type>::value>>
    constexpr sized_iterable_impl() noexcept(std::is_nothrow_default_constructible<B>::value &&
                                             std::is_nothrow_default_constructible<sentinel_type>::value) {
    }

#endif

    constexpr sized_iterable_impl(iterator_type begin, sentinel_type end) :
        _begin{ std::move(begin) },
        _end{ std::move(end) },
        _size{ static_cast<size_t>(_end - _begin) } {
    }

    template<class Iterable>
    explicit constexpr sized_iterable_impl(Iterable&& iterable) :
        _begin{ detail::begin(std::forward<Iterable>(iterable)) },
        _end{ detail::end(std::forward<Iterable>(iterable)) },
        _size{ lz::size(iterable) } {
    }

    LZ_NODISCARD constexpr size_t size() const noexcept {
        return _size;
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
} // namespace detail
} // namespace lz

// clang-format off
LZ_MODULE_EXPORT namespace lz {
/**
 * @brief A class that can be converted to any container. It only contains the iterators and
 * can be used in pipe expressions, converted to a container with `to<Container>()`, used in algorithms, for-each loops,
 * etc... It contains the size of the iterable.
 * @tparam It The iterator type.
 * @tparam S The sentinel type.
 */
template<class It, class S = It>
using sized_iterable = detail::sized_iterable_impl<It, S>;

/**
 * @brief A class that can be converted to any container. It only contains the iterators and
 * can be used in pipe expressions, converted to a container with `to<Container>()`, used in algorithms, for-each loops,
 * etc... It *may* contain the size of the iterable, depending on the iterator category (needs to be random access).
 * @tparam It The iterator type.
 * @tparam S The sentinel type.
 */
template<class It, class S = It>
using basic_iterable = detail::basic_iterable_impl<It, S>;

/**
 * @brief Creates a basic_iterable from an iterator and a sentinel. It can be used to create an iterable from a pair of
 * iterators.
 *
 * @param begin The begin iterator.
 * @param end The end sentinel.
 * @return A basic_iterable object
 */
template<class I, class S>
LZ_NODISCARD constexpr basic_iterable<I, S> make_basic_iterable(I begin, S end) {
    return { std::move(begin), std::move(end) };
}
    // clang-format on
}

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
            return Container{ detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                              std::forward<Args>(args)... };
        }
        else if constexpr (is_ra_v<it> && std::is_constructible_v<Container, it, it, remove_cvref_t<Args>...>) {
            auto iter = std::begin(iterable);
            return Container{ iter, iter + (std::end(iterable) - std::begin(iterable)), std::forward<Args>(args)... };
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
        return Container{ detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                          std::forward<Args>(args)... };
    }

    template<class Iterable, class... Args>
    LZ_NODISCARD static LZ_CONSTEXPR_CXX_14
        enable_if_t<!constructible_from_iterable<Iterable, Args...>::value &&
                        !constructible_from_it_sent<Iterable, Args...>::value &&
                        constructible_from_it_it<Iterable, Args...>::value && is_ra<iter_t<Iterable>>::value,
                    Container>
        construct(Iterable&& iterable, Args&&... args) {
        auto it = std::begin(iterable);
        return Container{ it, it + (std::end(iterable) - std::begin(iterable)), std::forward<Args>(args)... };
    }

    template<class Iterable, class... Args>
    LZ_NODISCARD static constexpr enable_if_t<
        !constructible_from_iterable<Iterable, Args...>::value && !constructible_from_it_sent<Iterable, Args...>::value &&
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
    LZ_NODISCARD constexpr Container<val_iterable_t<Iterable>, detail::decay_t<Args>...>
    operator()(Iterable&& iterable, Args&&... args) const {
        using C = Container<val_iterable_t<Iterable>, detail::decay_t<Args>...>;
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
[[nodiscard]] constexpr detail::fn_args_holder<detail::to_adaptor<Container>, detail::decay_t<Args>...>
to(Args&&... args)
    requires(!lz::iterable<detail::first_arg_t<Args...>>)
{
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
[[nodiscard]] constexpr detail::fn_args_holder<detail::template_combiner<Container>, detail::decay_t<Args>...>
to(Args&&... args)
    requires(!lz::iterable<detail::first_arg_t<Args...>>)
{
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
[[nodiscard]] constexpr Container<val_iterable_t<Iterable>, Args...>
to(Iterable&& iterable, Args&&... args)
    requires(lz::iterable<Iterable>)
{
    using Cont = Container<val_iterable_t<Iterable>, detail::decay_t<Args>...>;
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
detail::enable_if_t<!detail::is_iterable<detail::first_arg_t<Args...>>::value, 
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
LZ_NODISCARD constexpr detail::enable_if_t<detail::is_iterable<Iterable>::value, Container<val_iterable_t<Iterable>, Args...>>
to(Iterable&& iterable, Args&&... args) {
    using Cont = Container<val_iterable_t<Iterable>, detail::decay_t<Args>...>;
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
 * lz::custom_copier_for<my_container<T>, std::enable_if_t_t<...>> {
 *     // Same as above
 * };
 * ```
 */
using detail::custom_copier_for;

} // namespace lz

// clang-format on

#ifdef LZ_HAS_CONCEPTS

LZ_MODULE_EXPORT template<class Iterable, class Adaptor>
    requires(lz::adaptor<lz::detail::remove_cref_t<Adaptor>> && lz::iterable<lz::detail::remove_ref<Iterable>>)
[[nodiscard]] constexpr auto
operator|(Iterable&& iterable, Adaptor&& adaptor) -> decltype(std::forward<Adaptor>(adaptor)(std::forward<Iterable>(iterable))) {
    return std::forward<Adaptor>(adaptor)(std::forward<Iterable>(iterable));
}

#else

LZ_MODULE_EXPORT template<class Iterable, class Adaptor>
LZ_NODISCARD constexpr auto operator|(Iterable&& iterable, Adaptor&& adaptor)
    -> lz::detail::enable_if_t<lz::detail::is_adaptor<lz::detail::remove_cref_t<Adaptor>>::value &&
                                   lz::detail::is_iterable<lz::detail::remove_ref_t<Iterable>>::value,
                               decltype(std::forward<Adaptor>(adaptor)(std::forward<Iterable>(iterable)))> {
    return std::forward<Adaptor>(adaptor)(std::forward<Iterable>(iterable));
}

#endif

#endif // LZ_BASIC_ITERATOR_VIEW_HPP
