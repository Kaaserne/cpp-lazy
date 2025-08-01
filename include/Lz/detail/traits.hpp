#pragma once

#ifndef LZ_TRAITS_HPP
#define LZ_TRAITS_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <array>
#include <cstddef>
#include <iterator>
#include <type_traits>

LZ_MODULE_EXPORT namespace lz {
struct default_sentinel_t {
    LZ_NODISCARD friend constexpr bool operator==(default_sentinel_t, default_sentinel_t) noexcept {
        return true;
    }

    LZ_NODISCARD friend constexpr bool operator!=(default_sentinel_t, default_sentinel_t) noexcept {
        return false;
    }

    LZ_NODISCARD friend constexpr bool operator<(default_sentinel_t, default_sentinel_t) noexcept {
        return false;
    }

    LZ_NODISCARD friend constexpr bool operator>(default_sentinel_t, default_sentinel_t) noexcept {
        return false;
    }

    LZ_NODISCARD friend constexpr bool operator>=(default_sentinel_t, default_sentinel_t) noexcept {
        return true;
    }

    LZ_NODISCARD friend constexpr bool operator<=(default_sentinel_t, default_sentinel_t) noexcept {
        return true;
    }
};

/**
 * @brief Holds a default sentinel value that can be used in iterators. Example:
 * ```cpp
 * auto it = lz::repeat(20, 5).begin();
 * if (it == lz::default_sentinel) {
 *     // do something
 * }
 * ```
 */
LZ_INLINE_VAR constexpr default_sentinel_t default_sentinel{};

template<class Derived, class Reference, class Pointer, class DifferenceType, class IterCat, class S = Derived>
struct iterator;

template<class Derived, class Reference, class Pointer, class DifferenceType, class IterCat, class Sentinel>
constexpr bool
operator==(const Sentinel& sent, const iterator<Derived, Reference, Pointer, DifferenceType, IterCat, Sentinel>& it) {
    return it == sent;
}

template<class Derived, class Reference, class Pointer, class DifferenceType, class IterCat, class Sentinel>
constexpr bool
operator!=(const Sentinel& sent, const iterator<Derived, Reference, Pointer, DifferenceType, IterCat, Sentinel>& it) {
    return !(it == sent);
}

template<class Derived, class Reference, class Pointer, class DifferenceType, class IterCat, class Sentinel>
LZ_NODISCARD constexpr bool
operator<(const Sentinel& sent, const iterator<Derived, Reference, Pointer, DifferenceType, IterCat, Sentinel>& it) {
    return sent - it < 0;
}

template<class Derived, class Reference, class Pointer, class DifferenceType, class IterCat, class Sentinel>
LZ_NODISCARD constexpr bool
operator>(const Sentinel& sent, const iterator<Derived, Reference, Pointer, DifferenceType, IterCat, Sentinel>& it) {
    return it < sent;
}

template<class Derived, class Reference, class Pointer, class DifferenceType, class IterCat, class Sentinel>
LZ_NODISCARD constexpr bool
operator<=(const Sentinel& sent, const iterator<Derived, Reference, Pointer, DifferenceType, IterCat, Sentinel>& it) {
    return !(it < sent);
}

template<class Derived, class Reference, class Pointer, class DifferenceType, class IterCat, class Sentinel>
LZ_NODISCARD constexpr bool
operator>=(const Sentinel& sent, const iterator<Derived, Reference, Pointer, DifferenceType, IterCat, Sentinel>& it) {
    return !(sent < it);
}

template<class Derived, class Reference, class Pointer, class DifferenceType, class IterCat, class Sentinel>
LZ_NODISCARD constexpr typename Derived::difference_type
operator-(const Sentinel& sent, const iterator<Derived, Reference, Pointer, DifferenceType, IterCat, Sentinel>& it) {
    return -(it - sent);
}

struct lazy_view {};

namespace detail {
template<class>
using void_t = void;

template<bool B>
struct enable_if_impl {};

template<>
struct enable_if_impl<true> {
    template<class T>
    using type = T;
};

template<bool B, class T = void>
using enable_if = typename enable_if_impl<B>::template type<T>;

template<bool B>
struct conditional_impl;

template<>
struct conditional_impl<true> {
    template<class IfTrue, class /* IfFalse */>
    using type = IfTrue;
};

template<>
struct conditional_impl<false> {
    template<class /* IfTrue */, class IfFalse>
    using type = IfFalse;
};

template<bool B, class IfTrue, class IfFalse>
using conditional = typename conditional_impl<B>::template type<IfTrue, IfFalse>;

template<class... Ts>
struct conjunction : std::true_type {};

template<class T, class... Ts>
struct conjunction<T, Ts...> : conditional<T::value, conjunction<Ts...>, std::false_type> {};

#ifdef LZ_HAS_CXX_11

struct less {
    template<class T, class U>
    LZ_NODISCARD constexpr auto operator()(T&& lhs, U&& rhs) const
        noexcept(noexcept(static_cast<T&&>(lhs) < static_cast<U&&>(rhs)))
            -> decltype(static_cast<T&&>(lhs) < static_cast<U&&>(rhs)) {
        return static_cast<T&&>(lhs) < static_cast<U&&>(rhs);
    }
};

struct equal_to {
    template<class T, class U>
    LZ_NODISCARD constexpr auto operator()(T&& lhs, U&& rhs) const
        noexcept(noexcept(static_cast<T&&>(lhs) == static_cast<U&&>(rhs)))
            -> decltype(static_cast<T&&>(lhs) == static_cast<U&&>(rhs)) {
        return static_cast<T&&>(lhs) == static_cast<U&&>(rhs);
    }
};

struct plus {
    template<class T, class U>
    LZ_NODISCARD constexpr auto operator()(T&& lhs, U&& rhs) const
        noexcept(noexcept(static_cast<T&&>(lhs) + static_cast<U&&>(rhs)))
            -> decltype(static_cast<T&&>(lhs) + static_cast<U&&>(rhs)) {
        return static_cast<T&&>(lhs) + static_cast<U&&>(rhs);
    }
};

#define MAKE_BIN_PRED(OP) lz::detail::OP

template<class T>
using remove_ref = typename std::remove_reference<T>::type;

template<std::size_t... Is>
struct index_sequence {
    using type = index_sequence;
};

template<std::size_t N>
struct make_index_sequence_impl {
private:
    using left = typename make_index_sequence_impl<N / 2>::type;
    using right = typename make_index_sequence_impl<N - N / 2>::type;

    template<class L, class R>
    struct concat;

    template<std::size_t... Ls, std::size_t... Rs>
    struct concat<index_sequence<Ls...>, index_sequence<Rs...>> {
        using type = index_sequence<Ls..., (Rs + sizeof...(Ls))...>;
    };

public:
    using type = typename concat<left, right>::type;
};

template<>
struct make_index_sequence_impl<0> {
    using type = index_sequence<>;
};

template<>
struct make_index_sequence_impl<1> {
    using type = index_sequence<0>;
};

template<std::size_t N>
using make_index_sequence = typename make_index_sequence_impl<N>::type;

template<class T>
using decay_t = typename std::decay<T>::type;

template<std::size_t I, class T>
using tup_element = typename std::tuple_element<I, T>::type;

#else // ^^^ has cxx 11 vvv cxx > 11

template<class T>
using remove_ref = std::remove_reference_t<T>;

template<std::size_t... N>
using index_sequence = std::index_sequence<N...>;

template<std::size_t N>
using make_index_sequence = std::make_index_sequence<N>;

template<class T>
using decay_t = std::decay_t<T>;

template<std::size_t I, class T>
using tup_element = std::tuple_element_t<I, T>;

#define MAKE_BIN_PRED(OP) std::OP<>

#endif // LZ_HAS_CXX_11

#ifdef __cpp_lib_is_invocable

template<class Function, class... Args>
using is_invocable = std::is_invocable<Function, Args...>;

#else

template<class Function, class = void>
struct is_invocable_impl_no_args : std::false_type {};

template<class Function>
struct is_invocable_impl_no_args<Function, void_t<decltype(std::declval<Function>()())>> : std::true_type {};

template<class, class Function, class...>
struct is_invocable_impl_n_args : std::false_type {};

template<class Function, class... Args>
struct is_invocable_impl_n_args<void_t<decltype(std::declval<Function>()(std::declval<Args>()...))>, Function, Args...>
    : std::true_type {};

template<class F, class... Args>
using is_invocable = conditional<sizeof...(Args) == 0, is_invocable_impl_no_args<F>, is_invocable_impl_n_args<void, F, Args...>>;

#endif

#ifdef LZ_HAS_CXX_20

template<class T>
using remove_cvref = std::remove_cvref_t<T>;

#else // ^^^ has cxx 20 vvv cxx < 20

template<class T>
using remove_cvref = typename std::remove_cv<remove_ref<T>>::type;

#endif // LZ_HAS_CXX_20
template<class Iterable>
LZ_NODISCARD constexpr auto begin(Iterable&& c) noexcept(noexcept(std::forward<Iterable>(c).begin()))
    -> enable_if<!std::is_array<remove_ref<Iterable>>::value, decltype(std::forward<Iterable>(c).begin())> {
    return std::forward<Iterable>(c).begin();
}

template<class Iterable>
LZ_NODISCARD constexpr auto end(Iterable&& c) noexcept(noexcept(std::forward<Iterable>(c).end()))
    -> enable_if<!std::is_array<remove_ref<Iterable>>::value, decltype(std::forward<Iterable>(c).end())> {
    return std::forward<Iterable>(c).end();
}

template<class Iterable>
LZ_NODISCARD constexpr auto begin(Iterable&& c) noexcept(noexcept(std::begin(c)))
    -> enable_if<std::is_array<remove_ref<Iterable>>::value, decltype(std::begin(c))> {
    return std::begin(c);
}

template<class Iterable>
LZ_NODISCARD constexpr auto end(Iterable&& c) noexcept(noexcept(std::end(c)))
    -> enable_if<std::is_array<remove_ref<Iterable>>::value, decltype(std::end(c))> {
    return std::end(c);
}
} // namespace detail

#ifdef LZ_HAS_CXX_17

/**
 * @brief Returns the size of a container.
 *
 * @param c The container to get the size from.
 * @return The size of the container.
 */
template<class Iterable>
LZ_NODISCARD constexpr auto size(const Iterable& i) noexcept(noexcept(std::size(i))) -> decltype(std::size(i));

#else

/**
 * @brief Returns the size of a container.
 *
 * @param c The container to get the size from.
 * @return The size of the container.
 */
template<class Iterable>
LZ_NODISCARD constexpr auto size(const Iterable& i) noexcept(noexcept(i.size())) -> decltype(i.size());

/**
 * @brief Returns the size of a container.
 *
 * @param c The container to get the size from.
 * @return The size of the container.
 */
template<class T, size_t N>
LZ_NODISCARD constexpr std::size_t size(const T (&)[N]) noexcept;

#endif

/**
 * @brief Can be used to get the iterator type of an iterable. Example: `lz::iter_t<std::vector<int>>` will return
 * `std::vector<int>::iterator`.
 *
 * @tparam Iterable The iterable to get the iterator type from.
 */
template<class Iterable>
using iter_t = decltype(detail::begin(std::forward<Iterable>(std::declval<Iterable>())));

/**
 * @brief Can be used to get the sentinel type of an iterable. Example: `lz::sentinel_t<std::vector<int>>` will return
 * `std::vector<int>::iterator`.
 * @tparam Iterable The iterable to get the sentinel type from.
 */
template<class Iterable>
using sentinel_t = decltype(detail::end(std::forward<Iterable>(std::declval<Iterable>())));

/**
 * @brief Can be used to get the value type of an iterator. Example: `lz::val_t<std::vector<int>::iterator>` will return `int`.
 *
 * @tparam Iterator The iterator to get the value type from.
 */
template<class Iterator>
using val_t = typename std::iterator_traits<detail::remove_ref<Iterator>>::value_type;

/**
 * @brief Can be used to get the reference type of an iterator. Example: `lz::ref_t<std::vector<int>::iterator>` will return
 * `int&`.
 *
 * @tparam Iterator The iterator to get the reference type from.
 */
template<class Iterator>
using ref_t = typename std::iterator_traits<detail::remove_ref<Iterator>>::reference;

/**
 * @brief Can be used to get the pointer type of an iterator. Example: `lz::ptr_t<std::vector<int>::iterator>` will return
 * `int*`.
 *
 * @tparam Iterator The iterator to get the pointer type from.
 */
template<class Iterator>
using ptr_t = typename std::iterator_traits<detail::remove_ref<Iterator>>::pointer;

/**
 * @brief Can be used to get the difference type of an iterator. Example: `lz::diff_t<std::vector<int>::iterator>` will return
 * `std::ptrdiff_t`.
 *
 * @tparam Iterator The iterator to get the difference type from.
 */
template<class Iterator>
using diff_type = typename std::iterator_traits<detail::remove_ref<Iterator>>::difference_type;

/**
 * @brief Can be used to get the iterator category of an iterator. Example: `lz::iter_cat_t<std::vector<int>::iterator>` will
 * return `std::random_access_iterator_tag`.
 *
 * @tparam Iterator The iterator to get the iterator category from.
 */
template<class Iterator>
using iter_cat_t = typename std::iterator_traits<detail::remove_ref<Iterator>>::iterator_category;

/**
 * @brief Can be used to get the value type of an iterable. Example: `lz::val_iterable_t<std::vector<int>>` will return `int`.
 *
 * @tparam Iterable The iterable to get the value type from.
 */
template<class Iterable>
using val_iterable_t = typename std::iterator_traits<iter_t<Iterable>>::value_type;

/**
 * @brief Can be used to get the difference type of an iterable. Example: `lz::diff_iterable_t<std::vector<int>>` will return
 * `std::ptrdiff_t`.
 *
 * @tparam Iterable
 */
template<class Iterable>
using diff_iterable_t = typename std::iterator_traits<iter_t<Iterable>>::difference_type;

/**
 * @brief Can be used to get the reference type of an iterable. Example: `lz::ref_iterable_t<std::vector<int>>` will return
 * `int&`.
 *
 * @tparam Iterable The iterable to get the reference type from.
 */
template<class Iterable>
using ref_iterable_t = typename std::iterator_traits<iter_t<Iterable>>::reference;

/**
 * @brief Can be used to get the pointer type of an iterable. Example: `lz::ptr_iterable_t<std::vector<int>>` will return `int*`.
 *
 * @tparam Iterable The iterable to get the pointer type from.
 */
template<class Iterable>
using ptr_iterable_t = typename std::iterator_traits<iter_t<Iterable>>::pointer;

/**
 * @brief Can be used to get the iterator category of an iterable. Example: `lz::iter_cat_iterable_t<std::vector<int>>` will
 * return `std::random_access_iterator_tag`.
 *
 * @tparam Iterable The iterable to get the iterator category from.
 */
template<class Iterable>
using iter_cat_iterable_t = typename std::iterator_traits<iter_t<Iterable>>::iterator_category;
}

namespace lz {
namespace detail {

LZ_MODULE_EXPORT template<class T, class = void>
struct sized : std::false_type {};

LZ_MODULE_EXPORT template<class T>
struct sized<T, void_t<decltype(lz::size(std::declval<T>()))>> : std::true_type {};

template<class, class = void>
struct is_iterable : std::false_type {};

template<class T>
struct is_iterable<T, void_t<decltype(std::begin(std::declval<T>()), std::end(std::declval<T>()))>> : std::true_type {};

template<class T, std::size_t N>
struct is_iterable<T[N]> : std::true_type {};

template<class, class = void>
struct is_adaptor : std::false_type {};

template<class T>
struct is_adaptor<T, void_t<typename T::adaptor>> : std::true_type {};

template<class... Args>
struct first_arg_helper {};

template<class Arg, class... Rest>
struct first_arg_helper<Arg, Rest...> {
    using type = Arg;
};

template<>
struct first_arg_helper<> {
    using type = void;
};

template<class... Args>
using first_arg = typename first_arg_helper<Args...>::type;

template<class Function, class... Args>
using func_ret_type = decltype(std::declval<Function>()(std::declval<Args>()...));

template<class Function, class Iterator>
using func_ret_type_iter = decltype(std::declval<Function>()(*std::declval<Iterator>()));

template<class Function, class... Iterator>
using func_ret_type_iters = decltype(std::declval<Function>()(*(std::declval<Iterator>())...));

template<class... Ts>
using common_type = typename std::common_type<Ts...>::type;

template<class IterTag>
using is_ra_tag = std::is_convertible<IterTag, std::random_access_iterator_tag>;

template<class IterTag>
using is_bidi_tag = std::is_convertible<IterTag, std::bidirectional_iterator_tag>;

template<class IterTag>
using is_fwd_tag = std::is_convertible<IterTag, std::forward_iterator_tag>;

template<class Iterator>
using is_ra = is_ra_tag<iter_cat_t<Iterator>>;

template<class Iterator>
using is_bidi = is_bidi_tag<iter_cat_t<Iterator>>;

template<class Iterator>
using is_fwd = std::is_convertible<iter_cat_t<Iterator>, std::forward_iterator_tag>;

template<class Iterator, class S>
using is_sentinel = std::integral_constant<bool, !std::is_same<Iterator, S>::value>;

template<class Iterable>
using has_sentinel = std::integral_constant<bool, is_sentinel<iter_t<Iterable>, sentinel_t<Iterable>>::value>;
} // namespace detail

/**
 * @brief Helper to check whether a type is sized i.e. contains a .size() method. Example:
 * ```cpp
 * std::vector<int> v;
 * static_assert(lz::sized<decltype(v)>::value, "Vector is sized");
 *
 * auto not_sized = lz::cstring("Hello");
 * static_assert(!lz::sized<decltype(not_sized)>::value, "C string is not sized"); // C strings are not sized
 * ```
 * @tparam T The type to check.
 */
template<class T>
using sized = detail::sized<T>;
} // namespace lz

#endif // LZ_TRAITS_HPP
