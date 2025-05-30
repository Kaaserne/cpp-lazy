#pragma once

#ifndef LZ_ITER_TOOLS_ADAPTORS_HPP
#define LZ_ITER_TOOLS_ADAPTORS_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/tuple_helpers.hpp>
#include <Lz/drop.hpp>
#include <Lz/drop_while.hpp>
#include <Lz/filter.hpp>
#include <Lz/map.hpp>
#include <Lz/reverse.hpp>
#include <Lz/split.hpp>
#include <Lz/string_view.hpp>
#include <Lz/zip.hpp>
#include <cctype>

namespace lz {
namespace detail {
template<class To>
struct convert_fn {
    template<class From>
    LZ_NODISCARD constexpr To operator()(From&& f) const noexcept(noexcept(static_cast<To>(f))) {
        return static_cast<To>(f);
    }
};

template<std::size_t N>
struct get_fn {
    template<class T>
    LZ_NODISCARD constexpr auto operator()(T&& gettable) const noexcept -> decltype(std::get<N>(std::forward<T>(gettable))) {
        return std::get<N>(std::forward<T>(gettable));
    }
};

struct trim_fn {
    template<class CharT>
    LZ_NODISCARD constexpr bool operator()(const CharT c) const noexcept {
        return static_cast<bool>(std::isspace(static_cast<unsigned char>(c)));
    }
};

template<class Iterable, class Predicate>
using unzip_with_iterable = map_iterable<Iterable, tuple_expand<Predicate>>;

struct unzip_with_adaptor {
    using adaptor = unzip_with_adaptor;

    /**
     * @brief Unzips an iterable of tuple-like elements using the provided predicate function. The predicate function should take
     * the same number of arguments as the number of elements in the tuples and return a value. Example:
     * ```cpp
     * std::vector<std::tuple<int, int>> zipped = { std::make_tuple(1, 6), std::make_tuple(2, 7), std::make_tuple(3, 8) };
     * auto unzipped = lz::unzip_with(zipped, [](int a, int b) { return a + b; }); // {7, 9, 11}
     * ```
     *
     * @param iterable The iterable to unzip.
     * @param predicate The function that will be applied to each tuple element.
     * @return An iterable that contains the results of applying the predicate to each tuple element.
     */
    template<class Iterable, class Predicate>
    LZ_NODISCARD constexpr unzip_with_iterable<remove_ref<Iterable>, Predicate>
    operator()(Iterable&& iterable, Predicate predicate) const {
        return lz::map(std::forward<Iterable>(iterable), make_expand_fn(std::move(predicate)));
    }

    /**
     * @brief Unzips an iterable of tuple-like elements using the provided predicate function. The predicate function should take
     * the same number of arguments as the number of elements in the tuples and return a value. Example:
     * ```cpp
     * std::vector<std::tuple<int, int>> zipped = { std::make_tuple(1, 6), std::make_tuple(2, 7), std::make_tuple(3, 8) };
     * auto unzipped = zipped | lz::unzip_with([](int a, int b) { return a + b; }); // {7, 9, 11}
     * ```
     *
     * @param iterable The iterable to unzip.
     * @param predicate The function that will be applied to each tuple element.
     * @return An iterable that contains the results of applying the predicate to each tuple element.
     */
    template<class Predicate>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, Predicate> operator()(Predicate predicate) const {
        return { std::move(predicate) };
    }
};

template<class CharT, class Iterable>
using lines_iterable = split_iterable<basic_string_view<CharT>, Iterable, CharT>;

struct lines_adaptor {
    using adaptor = lines_adaptor;

    /**
     * @brief Returns a split_iterable, that splits the string on `'\n'` using `lz::sv_split`. Returns string_views to its
     * substrings. Example:
     * ```cpp
     * std::string str = "Hello\nWorld\n!";
     * auto splitted = lz::lines(str); // {"Hello", "World", "!"}
     * // or
     * auto splitted = str | lz::lines; // {"Hello", "World", "!"}
     * // or, little bit slower since lz::c_string is used here, which is not random access
     * auto splitted = lz::lines(lz::c_string("Hello\nWorld\n!")); // {"Hello", "World", "!"}
     * // or with string_view
     * lz::string_view str = "Hello\nWorld\n!";
     * auto splitted = lz::lines(str); // {"Hello", "World", "!"}
     * ```
     * @param string The string to split on "\n".
     */
    template<LZ_CONCEPT_ITERABLE String>
    LZ_NODISCARD constexpr lines_iterable<val_iterable_t<String>, remove_ref<String>> operator()(String&& string) const {
        using char_type = val_iterable_t<String>;
        return lz::sv_split(std::forward<String>(string), static_cast<char_type>('\n'));
    }

    /**
     * @brief Returns a split_iterable, that splits the string on `'\n'` using `lz::sv_split`. Returns string_views to its
     * substrings. Example:
     * ```cpp
     * lz::basic_string str = "Hello\nWorld\n!";
     * auto splitted = lz::lines(str); // {"Hello", "World", "!"}. `str` will not be by reference, because string_view is easy to
     * // copy
     * ```
     * @param string The string to split on '\n'.
     */
    template<class CharT>
    LZ_NODISCARD constexpr lines_iterable<CharT, copied_basic_sv<CharT>> operator()(const basic_string_view<CharT> string) const {
        return lz::sv_split(copied_basic_sv<CharT>(string), static_cast<CharT>('\n'));
    }
};

template<class Iterable, class T>
using as_iterable = map_iterable<Iterable, convert_fn<T>>;

template<class T>
struct as_adaptor {
    using adaptor = as_adaptor;

    /**
     * @brief Returns an iterable that converts the elements in the given container to the type @p `T` using `lz::map`.
     * @tparam T The type to convert the elements to.
     * Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5 };
     * auto floats = lz::as<float>(vec); // { 1.f, 2.f, 3.f, 4.f, 5.f }
     * // or
     * auto floats = vec | lz::as<float>; // { 1.f, 2.f, 3.f, 4.f, 5.f }
     * ```
     */
    template<class Iterable>
    LZ_NODISCARD constexpr as_iterable<remove_ref<Iterable>, T> operator()(Iterable&& iterable) const {
        return lz::map(std::forward<Iterable>(iterable), convert_fn<T>{});
    }
};

template<std::size_t Neighbours>
struct pairwise_n_adaptor {
    using adaptor = pairwise_n_adaptor<Neighbours>;

private:
    template<class Iterable, std::size_t... N>
    using pairwise_n_object = zip_iterable<drop_iterable<remove_ref<decltype(N, std::declval<Iterable>())>>...>;

    template<class Iterable, std::size_t... N>
    LZ_CONSTEXPR_CXX_14 pairwise_n_object<Iterable, N...> pairwise_n_construct(Iterable&& iterable, index_sequence<N...>) const {
        return lz::zip(lz::drop(std::forward<Iterable>(iterable), N)...);
    }

public:
    /**
     * @brief Iterates over the elements in the given iterable and returns a tuple of `N` adjacent elements using `lz::zip`.
     * Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5 };
     * auto iterable = lz::pairwise_n<2>(vec); // { {1, 2}, {2, 3}, {3, 4}, {4, 5} }
     * // or
     * auto iterable = vec | lz::pairwise_n<2>; // { {1, 2}, {2, 3}, {3, 4}, {4, 5} }
     * // or
     * auto iterable = lz::pairwise(vec); // { {1, 2}, {2, 3}, {3, 4}, {4, 5} }
     * // or
     * auto iterable = vec | lz::pairwise; // { {1, 2}, {2, 3}, {3, 4}, {4, 5} }
     * ```
     * @param iterable The iterable to iterate over.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD constexpr auto
    operator()(Iterable&& iterable) const -> decltype(pairwise_n_construct(std::forward<Iterable>(iterable),
                                                                           make_index_sequence<Neighbours>())) {
        return pairwise_n_construct(std::forward<Iterable>(iterable), make_index_sequence<Neighbours>());
    }
};

template<class Iterable, std::size_t N>
using pairwise_n_iterable = decltype(std::declval<pairwise_n_adaptor<N>>()(std::declval<Iterable>()));

template<class Iterable, std::size_t N>
using get_nth_iterable = map_iterable<Iterable, get_fn<N>>;

template<std::size_t N>
struct get_n_adaptor {
    using adaptor = get_n_adaptor<N>;

    /**
     * @brief Gets the nth element from a std::get-able container, using `std::get<N>` and `lz::map`. Example:
     * ```cpp
     * std::vector<std::tuple<int, int, int>> three_tuple_vec = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } };
     * auto actual = lz::get_nth<2>(three_tuple_vec); // {3, 6, 9}
     * // or
     * auto actual = three_tuple_vec | lz::get_nth<2>; // {3, 6, 9} (for cxx11 use lz::get_nth<2>{} instead of lz::get_nth<2>)
     * ```
     *
     * @param iterable The iterable to get the nth element from.
     * @return A map iterable that can be iterated over, containing the nth elements of the tuples in the iterable.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 get_nth_iterable<remove_ref<Iterable>, N> operator()(Iterable&& iterable) const {
        return lz::map(std::forward<Iterable>(iterable), get_fn<N>{});
    }
};

template<class Iterable, std::size_t... N>
using get_nths_iterable = zip_iterable<get_nth_iterable<Iterable, N>...>;

template<std::size_t... N>
struct get_nths_adaptor {
    using adaptor = get_nths_adaptor<N...>;

    /**
     * @brief Gets nths indexes from a std::get-able container, using `std::get<N>` and `lz::zip`. Example:
     * ```cpp
     * std::vector<std::tuple<int, int, int>> three_tuple_vec = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } };
     * auto actual = lz::get_nths<0, 2>(three_tuple_vec); // { {1, 3}, {4, 6}, {7, 9} }
     * // or
     * auto actual = three_tuple_vec | lz::get_nths<0, 2>; // { {1, 3}, {4, 6}, {7, 9} }
     * // (for cxx11 use lz::get_nths<0, 2>{}() instead of lz::get_nths<0, 2>())
     * ```
     * @param iterable The iterable to get the nth elements from.
     * @return A zip iterable that can be iterated over, containing the nth elements of the tuples in the iterable.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 get_nths_iterable<remove_ref<Iterable>, N...> operator()(Iterable&& iterable) const {
        return lz::zip(get_n_adaptor<N>{}(std::forward<Iterable>(iterable))...);
    }
};

template<class Iterable, class UnaryFilterPredicate, class UnaryMapOp>
using filter_map_iterable = map_iterable<filter_iterable<Iterable, UnaryFilterPredicate>, UnaryMapOp>;

struct filter_map_adaptor {
    using adaptor = filter_map_adaptor;

    /**
     * @brief Filters an iterable using `predicate` and then maps those elements using `fn`, using `lz::filter` and `lz::map`.
     * Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5 };
     * auto fm = lz::filter_map(vec, [](int i) { return i % 2 == 0; }, [](int i) { return i * 2; }); // {4, 8}
     * ```
     * @param iterable The iterable to filter and map.
     * @param predicate The predicate to filter the elements with.
     * @param unary_op The function to map the filtered elements with.
     * @return A filter_map_iterable that can be iterated over, containing the mapped elements.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class UnaryFilterPredicate, class UnaryMapOp>
    LZ_NODISCARD constexpr filter_map_iterable<remove_ref<Iterable>, UnaryFilterPredicate, UnaryMapOp>
    operator()(Iterable&& iterable, UnaryFilterPredicate predicate, UnaryMapOp unary_op) const {
        return lz::map(lz::filter(std::forward<Iterable>(iterable), std::move(predicate)), std::move(unary_op));
    }

    /**
     * @brief Filters an iterable using `predicate` and then maps those elements using `fn`, using `lz::filter` and `lz::map`.
     * Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5 };
     * auto fm = vec | lz::filter_map([](int i) { return i % 2 == 0; }, [](int i) { return i * 2; }); // {4, 8}
     * ```
     * @param predicate The predicate to filter the elements with.
     * @param unary_op The function to map the filtered elements with.
     * @return A filter_map_iterable that can be iterated over, containing the mapped elements.
     */
    template<class UnaryFilterPredicate, class UnaryMapOp>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, UnaryFilterPredicate, UnaryMapOp>
    operator()(UnaryFilterPredicate filterPredicate, UnaryMapOp mapOp) const {
        return { std::move(filterPredicate), std::move(mapOp) };
    }
};

template<class Iterable, class SelectorIterable>
using select_iterable = filter_map_iterable<zip_iterable<Iterable, SelectorIterable>, detail::get_fn<1>, detail::get_fn<0>>;

struct select_adaptor {
    using adaptor = select_adaptor;

    /**
     * @brief Selects elements from the first iterable if the corresponding element in the second iterable is `true`, using
     * `lz::filter`, `lz::zip` and `lz::map`. Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5 };
     * std::vector<bool> bools = { true, false, true, false, true };
     * auto selected = lz::select(vec, bools); // {1, 3, 5}
     * ```
     * @param iterable The iterable to select elements from.
     * @param selectors The iterable of selectors (booleans).
     * @return A select_iterable that can be iterated over, containing the selected elements.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, LZ_CONCEPT_ITERABLE SelectorIterable>
    LZ_NODISCARD constexpr select_iterable<remove_ref<Iterable>, remove_ref<SelectorIterable>>
    operator()(Iterable&& iterable, SelectorIterable&& selectors) const {
        return filter_map_adaptor{}(lz::zip(std::forward<Iterable>(iterable), std::forward<SelectorIterable>(selectors)),
                                    detail::get_fn<1>(), detail::get_fn<0>());
    }

    /**
     * @brief Selects elements from the first iterable if the corresponding element in the second iterable is `true`, using
     * `lz::filter`, `lz::zip` and `lz::map`. Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5 };
     * std::vector<bool> bools = { true, false, true, false, true };
     * auto selected = vec | lz::select(bools); // {1, 3, 5}
     * ```
     * @param selectors The iterable of selectors (booleans).
     * @return A select_iterable that can be iterated over, containing the selected elements.
     */
    template<class SelectorIterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, SelectorIterable> operator()(SelectorIterable&& selectors) const {
        return { std::forward<SelectorIterable>(selectors) };
    }
};

template<class Iterable, class UnaryPredicate>
using drop_back_iterable = reverse_iterable<drop_while_iterable<reverse_iterable<Iterable>, UnaryPredicate>>;

struct drop_back_while_adaptor {
    using adaptor = drop_back_while_adaptor;

    /**
     * @brief Drops elements from the back of the iterable as long as `predicate` returns `true`, then reverses the iterable
     * again. Essentially turning the predicate into a not function, using `lz::reverse`, `lz::drop_while` followed by another
     * `lz::reverse`. Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5 };
     * auto dropped = lz::drop_back_while(vec, [](int i) { return i > 3; }); // {1, 2, 3}
     * ```
     * @param iterable The iterable to drop elements from the back.
     * @param predicate The predicate to determine which elements to drop from the back.
     * @return A drop_back_iterable that can be iterated over, containing the elements
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
    LZ_NODISCARD constexpr drop_back_iterable<remove_ref<Iterable>, UnaryPredicate>
    operator()(Iterable&& iterable, UnaryPredicate predicate) const {
        return lz::reverse(lz::drop_while(lz::reverse(std::forward<Iterable>(iterable)), std::move(predicate)));
    }

    /**
     * @brief Drops elements from the back of the iterable as long as `predicate` returns `true`, then reverses the iterable
     * again. Essentially turning the predicate into a not function, using `lz::reverse`, `lz::drop_while` followed by another
     * `lz::reverse`. Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5 };
     * auto dropped = vec | lz::drop_back_while([](int i) { return i > 3; }); // {1, 2, 3}
     * ```
     * @param predicate The predicate to determine which elements to drop from the back.
     * @return A drop_back_iterable that can be iterated over, containing the elements
     */
    template<class UnaryPredicate>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, UnaryPredicate> operator()(UnaryPredicate predicate) const {
        return { std::move(predicate) };
    }
};

template<class Iterable, class UnaryPredicateFirst, class UnaryPredicateLast>
using trim_iterable = drop_back_iterable<drop_while_iterable<Iterable, UnaryPredicateFirst>, UnaryPredicateLast>;

struct trim_adaptor {
    using adaptor = trim_adaptor;

    /**
     * @brief Trims the front and back of the iterable using the provided predicates. The first predicate is used to drop elements
     * from the front, and the second predicate is used to drop elements from the back. Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5 };
     * auto trimmed = lz::trim(vec, [](int i) { return i < 3; }, [](int i) { return i > 4; }); // {3, 4}
     * ```
     * @param iterable The iterable to trim.
     * @param first The predicate to determine which elements to drop from the front.
     * @param last The predicate to determine which elements to drop from the back.
     * @return A trim_iterable that can be iterated over, containing the trimmed elements.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicateFirst, class UnaryPredicateLast>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 trim_iterable<remove_ref<Iterable>, UnaryPredicateFirst, UnaryPredicateLast>
    operator()(Iterable&& iterable, UnaryPredicateFirst first, UnaryPredicateLast last) const {
        auto taken_first = lz::drop_while(std::forward<Iterable>(iterable), std::move(first));
        return drop_back_while_adaptor{}(std::move(taken_first), std::move(last));
    }

    /**
     * @brief Trims whitespaces on the front and back of the given string.
     * @param iterable The string to trim.
     * ```cpp
     * std::string str = "  hello  ";
     * auto trimmed = lz::trim(str); // "hello"
     * // or
     * auto trimmed = str | lz::trim; // "hello"
     */
    template<class CharT>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 trim_iterable<const std::basic_string<CharT>, trim_fn, trim_fn>
    operator()(const std::basic_string<CharT>& iterable) const {
        return (*this)(iterable, trim_fn{}, trim_fn{});
    }

    /**
     * @brief Trims whitespaces on the front and back of the given string.
     * @param iterable The string to trim.
     * ```cpp
     * std::string str = "  hello  ";
     * auto trimmed = lz::trim(str); // "hello"
     * // or
     * auto trimmed = str | lz::trim; // "hello"
     */
    template<class CharT>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 trim_iterable<copied_basic_sv<CharT>, trim_fn, trim_fn>
    operator()(lz::basic_string_view<CharT> iterable) const {
        return (*this)(copied_basic_sv<CharT>(iterable), trim_fn{}, trim_fn{});
    }

    /**
     * @brief Trims the front and back of the iterable using the provided predicates. The first predicate is used to drop elements
     * from the front, and the second predicate is used to drop elements from the back. Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5 };
     * auto trimmed = vec | lz::trim([](int i) { return i < 3; }, [](int i) { return i > 4; }); // {3, 4}
     * ```
     * @param first The predicate to determine which elements to drop from the front.
     * @param last The predicate to determine which elements to drop from the back.
     * @return A trim_iterable that can be iterated over, containing the trimmed elements.
     */
    template<class UnaryPredicateFirst, class UnaryPredicateLast>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, UnaryPredicateFirst, UnaryPredicateLast>
    operator()(UnaryPredicateFirst first, UnaryPredicateLast last) const {
        return { std::move(first), std::move(last) };
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_ITER_TOOLS_ADAPTORS_HPP
