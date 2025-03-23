#ifndef LZ_ITER_TOOLS_ADAPTORS_HPP
#define LZ_ITER_TOOLS_ADAPTORS_HPP

#pragma once

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/tuple_helpers.hpp>
#include <Lz/drop.hpp>
#include <Lz/drop_while.hpp>
#include <Lz/filter.hpp>
#include <Lz/map.hpp>
#include <Lz/reverse.hpp>
#include <Lz/split.hpp>
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
    LZ_NODISCARD constexpr bool operator()(CharT c) const {
        return static_cast<bool>(std::isspace(static_cast<unsigned char>(c)));
    }
};

struct lines_adaptor {
    using adaptor = lines_adaptor;

#ifdef LZ_HAS_CXX_11

    static constexpr adaptor lines{};

#endif

    /**
     * @brief Returns a split_iterable, that splits the string on `'\n'` using `lz::split`. Returns string_views to its
     * substrings. Example:
     * ```cpp
     * std::string str = "Hello\nWorld\n!";
     * auto splitted = lz::lines(str); // {"Hello", "World", "!"}
     * // or
     * auto splitted = str | lz::lines; // {"Hello", "World", "!"}
     * // or, little bit slower since lz::c_string is not random access
     * auto splitted = lz::lines(lz::c_string("Hello\nWorld\n!")); // {"Hello", "World", "!"}
     * // or with string_view
     * auto splitted = lz::lines(lz::string_view("Hello\nWorld\n!")); // {"Hello", "World", "!"}
     * // dangling reference, do not do this
     * // auto splitted = lz::lines("Hello\nWorld\n!");
     * ```
     * @param string The string to split on "\n".
     */
    template<LZ_CONCEPT_ITERABLE String>
    LZ_NODISCARD constexpr auto operator()(String&& string) const -> decltype(lz::sv_split(std::forward<String>(string), "\n")) {
        return lz::sv_split(std::forward<String>(string), "\n");
    }
};

template<class T>
struct as_adaptor {
    using adaptor = as_adaptor;

#ifdef LZ_HAS_CXX_11

    static constexpr adaptor as{};

#endif

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
    LZ_NODISCARD constexpr map_iterable<remove_ref<Iterable>, convert_fn<T>> operator()(Iterable&& iterable) const {
        return lz::map(std::forward<Iterable>(iterable), convert_fn<T>{});
    }
};

template<std::size_t Neighbours>
struct pairwise_n_adaptor {
    using adaptor = pairwise_n_adaptor<Neighbours>;

#ifdef LZ_HAS_CXX_11

    static constexpr pairwise_n_adaptor<2> pairwise{};

    static constexpr adaptor pairwise_n{};

#endif

private:
    template<class Iterable, std::size_t... N>
    using pairwise_n_object = zip_iterable<drop_iterable<remove_ref<decltype(N, std::declval<Iterable>())>>...>;

    template<class Iterable, std::size_t... N>
    LZ_CONSTEXPR_CXX_14 pairwise_n_object<Iterable, N...>
    pairwise_n_construct(Iterable&& iterable, index_sequence_helper<N...>) const {
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
    LZ_NODISCARD constexpr auto operator()(Iterable&& iterable) const
        -> decltype(pairwise_n_construct(std::forward<Iterable>(iterable), make_index_sequence<Neighbours>())) {
        return pairwise_n_construct(std::forward<Iterable>(iterable), make_index_sequence<Neighbours>());
    }
};

template<std::size_t N>
struct get_n_adaptor {
    using adaptor = get_n_adaptor<N>;

#ifdef LZ_HAS_CXX_11

    static constexpr adaptor get_nth{};

    static constexpr get_n_adaptor<0> keys{};

    static constexpr get_n_adaptor<1> values{};

#endif

    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 map_iterable<remove_ref<Iterable>, get_fn<N>> operator()(Iterable&& iterable) const {
        return lz::map(std::forward<Iterable>(iterable), get_fn<N>{});
    }
};

template<class Iterable, class UnaryFilterPredicate, class UnaryMapOp>
using filter_map_iterable = map_iterable<filter_iterable<Iterable, UnaryFilterPredicate>, UnaryMapOp>;

struct filter_map_adaptor {
    using adaptor = filter_map_adaptor;

#ifdef LZ_HAS_CXX_11

    static constexpr adaptor filter_map{};

#endif

    template<LZ_CONCEPT_ITERABLE Iterable, class UnaryFilterPredicate, class UnaryMapOp>
    LZ_NODISCARD constexpr filter_map_iterable<remove_ref<Iterable>, UnaryFilterPredicate, UnaryMapOp>
    operator()(Iterable&& iterable, UnaryFilterPredicate filterPredicate, UnaryMapOp mapOp) const {
        return lz::map(lz::filter(std::forward<Iterable>(iterable), std::move(filterPredicate)), std::move(mapOp));
    }

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

#ifdef LZ_HAS_CXX_11

    static constexpr adaptor select{};

#endif

    template<LZ_CONCEPT_ITERABLE Iterable, LZ_CONCEPT_ITERABLE SelectorIterable>
    LZ_NODISCARD constexpr select_iterable<remove_ref<Iterable>, remove_ref<SelectorIterable>>
    operator()(Iterable&& iterable, SelectorIterable&& selectors) const {
        return filter_map_adaptor{}(lz::zip(std::forward<Iterable>(iterable), std::forward<SelectorIterable>(selectors)),
                                    detail::get_fn<1>(), detail::get_fn<0>());
    }

    template<class SelectorIterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, remove_ref<SelectorIterable>>
    operator()(SelectorIterable&& selectors) const {
        return { std::forward<SelectorIterable>(selectors) };
    }
};

template<class Iterable, class UnaryPredicate>
using drop_back_iterable = reverse_iterable<drop_while_iterable<reverse_iterable<Iterable>, UnaryPredicate>>;

struct drop_back_while_adaptor {
    using adaptor = drop_back_while_adaptor;

#ifdef LZ_HAS_CXX_11

    static constexpr adaptor drop_back_while{};

#endif

    template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 drop_back_iterable<remove_ref<Iterable>, UnaryPredicate>
    operator()(Iterable&& iterable, UnaryPredicate predicate) const {
        return lz::reverse(lz::drop_while(lz::reverse(std::forward<Iterable>(iterable)), std::move(predicate)));
    }

    template<class UnaryPredicate>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, UnaryPredicate> operator()(UnaryPredicate predicate) const {
        return { std::move(predicate) };
    }
};

template<class Iterable, class UnaryPredicateFirst, class UnaryPredicateLast>
using trim_iterable = drop_back_iterable<drop_while_iterable<Iterable, UnaryPredicateFirst>, UnaryPredicateLast>;

struct trim_adaptor {
    using adaptor = trim_adaptor;

#ifdef LZ_HAS_CXX_11

    static constexpr adaptor trim{};

#endif
    // clang-format off

    template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicateFirst, class UnaryPredicateLast>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14
    trim_iterable<remove_ref<Iterable>, UnaryPredicateFirst, UnaryPredicateLast>
        operator()(Iterable&& iterable, UnaryPredicateFirst first, UnaryPredicateLast last) const {
        auto taken_first = lz::drop_while(std::forward<Iterable>(iterable), std::move(first));
        return drop_back_while_adaptor{}(std::move(taken_first), std::move(last));
    }

    // clang-format on

    template<class CharT>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 trim_iterable<const std::basic_string<CharT>, trim_fn, trim_fn>
    operator()(const std::basic_string<CharT>& iterable) const {
        return (*this)(iterable, trim_fn{}, trim_fn{});
    }

    template<class CharT>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 drop_back_iterable<drop_while_iterable<lz::basic_string_view<CharT>, trim_fn>, trim_fn>
    operator()(lz::basic_string_view<CharT> iterable) const {
        return (*this)(iterable, trim_fn{}, trim_fn{});
    }

    template<class UnaryPredicateFirst, class UnaryPredicateLast>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, UnaryPredicateFirst, UnaryPredicateLast>
    operator()(UnaryPredicateFirst first, UnaryPredicateLast last) const {
        return { std::move(first), std::move(last) };
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_ITER_TOOLS_ADAPTORS_HPP
