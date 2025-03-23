#pragma once

#ifndef LZ_SPLIT_ADAPTOR_HPP
#define LZ_SPLIT_ADAPTOR_HPP

#include <Lz/c_string.hpp>
#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterables/split.hpp>

namespace lz {
namespace detail {
// TODO add single argument for std::find usage

template<class ValueType>
struct split_adaptor {
    using adaptor = split_adaptor<ValueType>;

#ifdef LZ_HAS_CXX_11

    static constexpr adaptor t_split{};

    static constexpr split_adaptor<lz::string_view> sv_split{};

    static constexpr split_adaptor<std::string> s_split{};

#endif

    /**
     * @brief Splits an iterable on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and
     * the iterable does not contain a .size() method. Returns `ValueType` as iterator is dereferenced. Example:
     * ```cpp
     * std::array<char, 5> arr = { 'h', 'e', 'l', 'l', 'o' };
     * using iterable = lz::basic_iterable<decltype(arr.begin)>;
     * auto splitted = lz::t_split<iterable>(arr, "ll"); // {{'h', 'e'}, {'o'}} where value_type =
     * // lz::basic_iterable<decltype(arr.begin)>
     * // which is equal as lz::split(arr, "ll")
     *
     * auto splitted = lz::t_split<std::string>(arr, "ll"); // {"he", "o"} where value_type = std::string
     * ```
     * @param iterable The iterable to split. Must be an actual reference.
     * @param delimiter The delimiter to split on. Must be an actual reference, such as a raw c string `","`
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class CharT>
    constexpr split_iterable<ValueType, remove_ref<Iterable>, c_string_iterable<const CharT*>>
    operator()(Iterable&& iterable, const CharT* delimiter) const {
        return (*this)(std::forward<Iterable>(iterable), c_string(delimiter));
    }

    /**
     * @brief Splits an iterable on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and
     * the iterable does not contain a .size() method. Returns `ValueType` as iterator is dereferenced. Example:
     * ```cpp
     * std::array<int, 5> arr = { 1, 2, 3, 4, 5 };
     * auto to_split = { 3, 4 }; // must be by reference
     * using iterable = lz::basic_iterable<decltype(arr.begin)>;
     * auto splitted = lz::t_split<iterable>(arr, to_split); // {{1, 2}, {5}} where value_type =
     * // lz::basic_iterable<decltype(arr.begin)>
     * // which is equal to lz::split(arr, to_split)
     * ```
     * @param iterable The iterable to split. Must be an actual reference.
     * @param delimiter The delimiter to split on. Must be an actual reference
     */
    template<LZ_CONCEPT_ITERABLE Iterable, LZ_CONCEPT_ITERABLE Iterable2>
    constexpr split_iterable<ValueType, remove_ref<Iterable>, remove_ref<Iterable2>>
    operator()(Iterable&& iterable, Iterable2&& delimiter) const {
        return { std::forward<Iterable>(iterable), std::forward<Iterable2>(delimiter) };
    }

    /**
     * @brief Splits an iterable on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and
     * the iterable does not contain a .size() method. Returns `ValueType` as iterator is dereferenced. Example:
     * ```cpp
     * std::array<int, 5> arr = { 1, 2, 3, 4, 5 };
     * auto to_split = { 3, 4 }; // must be by reference
     * using iterable = lz::basic_iterable<decltype(arr.begin)>;
     * auto splitted = arr | lz::t_split<iterable>(to_split); // {{1, 2}, {5}} where value_type =
     * // lz::basic_iterable<decltype(arr.begin)>
     * // which is equal to arr | lz::split(to_split)
     * ```
     * @param delimiter The iterable to split. Must be an actual reference.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, ref_or_view<remove_ref<Iterable>>> operator()(Iterable&& delimiter) const {
        return { std::forward<Iterable>(delimiter) };
    }

    /**
     * @brief Splits an iterable on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and
     * the iterable does not contain a .size() method. Returns `ValueType` as iterator is dereferenced. Example:
     * ```cpp
     * std::array<char, 5> arr = { 'h', 'e', 'l', 'l', 'o' };
     * auto splitted = arr | lz::split("ll"); // {{'h', 'e'}, {'o'}}
     * ```
     * @param delimiter The delimiter to split on. Must be an actual reference, such as a raw c string `","`
     */
    template<class CharT>
    LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, ref_or_view<c_string_iterable<const CharT*>>>
    operator()(const CharT* delimiter) const {
        return (*this)(c_string(delimiter));
    }
};

template<>
struct split_adaptor<void> {
    using adaptor = split_adaptor<void>;

#ifdef LZ_HAS_CXX_11

    static constexpr adaptor split{};

#endif
    // clang-format off
    /**
     * @brief Splits an iterable on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and
     * the iterable does not contain a .size() method. Returns an iterable of `lz::basic_iterable` as iterator is dereferenced.
     * Example:
     * ```cpp
     * std::array<char, 5> arr = { 'h', 'e', 'l', 'l', 'o' };
     * auto splitted = lz::split(arr, "ll"); // {{'h', 'e'}, {'o'}} where value_type = lz::basic_iterable<decltype(arr.begin)>
     * ```
     * @param iterable The iterable to split. Must be an actual reference.
     * @param delimiter The delimiter to split on. Must be an actual reference, such as a raw c string `","`
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class CharT>
    LZ_NODISCARD constexpr
    split_iterable<basic_iterable<iter_t<Iterable>, sentinel_t<Iterable>>, remove_ref<Iterable>, c_string_iterable<const CharT*>>
    operator()(Iterable&& iterable, const CharT* delimiter) const {
        return (*this)(std::forward<Iterable>(iterable), c_string(delimiter));
    }

    /**
     * @brief Splits an iterable on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and
     * the iterable does not contain a .size() method. Returns an iterable of `lz::basic_iterable` as iterator is dereferenced.
     * Example:
     * ```cpp
     * std::array<int, 5> arr = { 1, 2, 3, 4, 5 };
     * auto to_split = { 3, 4 }; // must be by reference
     * auto splitted = lz::split(arr, to_split); // {{1, 2}, {5}} where value_type = lz::basic_iterable<decltype(arr.begin)>
     * ```
     * @param iterable The iterable to split. Must be an actual reference.
     * @param delimiter The delimiter to split on. Must be an actual reference
     */
    template<LZ_CONCEPT_ITERABLE Iterable, LZ_CONCEPT_ITERABLE Iterable2>
    LZ_NODISCARD constexpr
    split_iterable<basic_iterable<iter_t<Iterable>, sentinel_t<Iterable>>, remove_ref<Iterable>, remove_ref<Iterable2>>
    operator()(Iterable&& iterable, Iterable2&& delimiter) const {
        using a = split_adaptor<basic_iterable<iter_t<Iterable>, sentinel_t<Iterable>>>;
        return a{}(std::forward<Iterable>(iterable), std::forward<Iterable2>(delimiter));
    }

    // clang-format on

    /**
     * @brief Splits an iterable on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and
     * the iterable does not contain a .size() method. Returns an iterable of `lz::basic_iterable` as iterator is dereferenced.
     * Example:
     * ```cpp
     * std::array<int, 5> arr = { 1, 2, 3, 4, 5 };
     * auto to_split = { 3, 4 }; // must be by reference
     * auto splitted = arr | lz::split(to_split); // {{1, 2}, {5}} where value_type = lz::basic_iterable<decltype(arr.begin)>
     * ```
     * @param delimiter The iterable to split. Must be an actual reference.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, ref_or_view<remove_ref<Iterable>>>
    operator()(Iterable&& delimiter) const {
        using a = split_adaptor<basic_iterable<iter_t<Iterable>, sentinel_t<Iterable>>>;
        return a{}(std::forward<Iterable>(delimiter));
    }

    /**
     * @brief Splits an iterable on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and
     * the iterable does not contain a .size() method. Returns an iterable of `lz::basic_iterable` as iterator is dereferenced.
     * Example:
     * ```cpp
     * std::array<char, 5> arr = { 'h', 'e', 'l', 'l', 'o' };
     * auto splitted = arr | lz::split("ll"); // {{'h', 'e'}, {'o'}} where value_type = lz::basic_iterable<decltype(arr.begin)>
     * ```
     * @param delimiter The delimiter to split on. Must be an actual reference, such as a raw c string `","`
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class CharT>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, ref_or_view<c_string_iterable<const CharT*>>>
    operator()(const CharT* delimiter) const {
        return (*this)(c_string(delimiter));
    }
};

} // namespace detail

using detail::split_adaptor;
} // namespace lz

#endif