#ifndef LZ_ITER_TOOLS_ADAPTORS_HPP
#define LZ_ITER_TOOLS_ADAPTORS_HPP

#pragma once

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/tuple_helpers.hpp>
#include <Lz/map.hpp>
#include <Lz/split.hpp>
#include <Lz/zip.hpp>

namespace lz {
namespace detail {
template<class To>
struct convert_fn {
    template<class From>
    constexpr To operator()(From&& f) const noexcept(noexcept(static_cast<To>(f))) {
        return static_cast<To>(f);
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
    LZ_NODISCARD constexpr map_iterable<Iterable, convert_fn<T>> operator()(Iterable&& iterable) const {
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
    using pairwise_n_sized_object =
        zip_iterable<decay_t<decltype((N, std::declval<sized_iterable<iter_t<Iterable>, sentinel_t<Iterable>>>()))>...>;

    template<class Iterable, std::size_t... N>
    using pairwise_n_object =
        zip_iterable<decay_t<decltype((N, std::declval<basic_iterable_impl<iter_t<Iterable>, sentinel_t<Iterable>>>()))>...>;

    template<std::size_t N, class Iterator>
    LZ_CONSTEXPR_CXX_14 enable_if<N == 0, decay_t<Iterator>> cached_next(Iterator&& iterator) const {
        return std::forward<Iterator>(iterator);
    }

    template<std::size_t N, class Iterator>
    LZ_CONSTEXPR_CXX_14 enable_if<N != 0, decay_t<Iterator>> cached_next(Iterator&& iterator) const {
        return std::next(cached_next<N - 1>(std::forward<Iterator>(iterator)), 1);
    }

    template<class Iterable, std::size_t... N>
    LZ_CONSTEXPR_CXX_14 enable_if<sized<Iterable>::value, pairwise_n_sized_object<Iterable, N...>>
    pairwise_n_construct(Iterable&& iterable, index_sequence_helper<N...>) const {
        auto new_begin = std::make_tuple(cached_next<N>(std::begin(iterable))...);
        auto ends = std::make_tuple(std::end((N, iterable))...);

        return lz::zip(sized_iterable<decay_t<decltype(std::get<N>(new_begin))>, decay_t<decltype(std::get<N>(ends))>>(
            std::get<N>(new_begin), std::get<N>(ends), static_cast<std::size_t>(lz::size(iterable)) - N)...);
    }

    template<class Iterable, std::size_t... N>
    LZ_CONSTEXPR_CXX_14 enable_if<!sized<Iterable>::value, pairwise_n_object<Iterable, N...>>
    pairwise_n_construct(Iterable&& iterable, index_sequence_helper<N...>) const {
        auto new_begin = std::make_tuple(cached_next<N>(std::begin(iterable))...);
        auto ends = std::make_tuple(std::end((N, iterable))...);

        return lz::zip(basic_iterable_impl<decay_t<decltype(std::get<N>(new_begin))>, decay_t<decltype(std::get<N>(ends))>>(
            std::get<N>(new_begin), std::get<N>(ends))...);
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
} // namespace detail

enum class size_hint : std::uint8_t {
    dynamic
};
} // namespace lz

#endif // LZ_ITER_TOOLS_ADAPTORS_HPP
