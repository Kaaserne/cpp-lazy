#pragma once

#ifndef LZ_SPLIT_ADAPTOR_HPP
#define LZ_SPLIT_ADAPTOR_HPP

#include <Lz/c_string.hpp>
#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterables/split.hpp>
#include <Lz/string_view.hpp>

namespace lz {

/**
 * @brief Wrapper for lz::copied_iterable<lz::basic_string_view<CharT>>.
 *
 * @tparam CharT The character type of the string view.
 */
template<class CharT>
using copied_basic_sv = lz::copied_iterable<lz::basic_string_view<CharT>>;

/**
 * @brief Wrapper for lz::copied_iterable<lz::string_view>.
 */
using copied_sv = copied_basic_sv<char>;

namespace detail {
// TODO add @return everywhere

template<class ValueType>
struct split_adaptor {
    using adaptor = split_adaptor<ValueType>;

    /**
     * @brief Splits an iterable on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and
     * the iterable does not contain a .size() method. Returns `ValueType` as iterator is dereferenced. Example:
     * ```cpp
     * std::array<int, 5> arr = { 1, 2, 3, 4, 5 };
     * using iterable = lz::basic_iterable<decltype(arr.begin)>;
     * auto splitted = lz::t_split<iterable>(arr, 2); // { { 1 }, { 3, 4, 5 } } where value_type =
     * // lz::basic_iterable<decltype(arr.begin)>
     * // which is equal to lz::split(arr, 2)
     * ```
     * @param iterable The iterable to split. Must be an actual reference.
     * @param delimiter The delimiter to split on. Doesn't have to be a reference
     */
    template<class Iterable, class T>
    constexpr enable_if<!is_iterable<T>::value, split_iterable<ValueType, remove_ref<Iterable>, T>>
    operator()(Iterable&& iterable, T delimiter) const {
        return { std::forward<Iterable>(iterable), std::forward<T>(delimiter) };
    }

    /**
     * @brief Splits an iterable on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel
     * and the iterable does not contain a .size() method. Returns `ValueType` as iterator is dereferenced. Example:
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
     * @param delimiter The delimiter to split on for c-strings
     */
    template<class Iterable, class CharT>
    constexpr split_iterable<ValueType, remove_ref<Iterable>, copied_basic_sv<CharT>>
    operator()(Iterable&& iterable, const CharT* delimiter) const {
        return (*this)(std::forward<Iterable>(iterable), lz::string_view(delimiter));
    }

    /**
     * @brief Splits an iterable on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel
     * and the iterable does not contain a .size() method. Returns `ValueType` as iterator is dereferenced. Example:
     * ```cpp
     * std::array<char, 5> arr = { 'h', 'e', 'l', 'l', 'o' };
     * using iterable = lz::basic_iterable<decltype(arr.begin)>;
     * auto splitted = lz::t_split<iterable>(arr, lz::string_view("ll")); // {{'h', 'e'}, {'o'}} where value_type =
     * // lz::basic_iterable<decltype(arr.begin)>
     * ```
     * @param iterable The iterable to split. Must be an actual reference.
     * @param delimiter The string_view delimiter, it's copied and therefore does not have to be a reference.
     */
    template<class Iterable, class CharT>
    constexpr split_iterable<ValueType, remove_ref<Iterable>, copied_basic_sv<CharT>>
    operator()(Iterable&& iterable, const basic_string_view<CharT> delimiter) const {
        return (*this)(std::forward<Iterable>(iterable), copied_basic_sv<CharT>(delimiter));
    }

    // clang-format off

    /**
     * @brief Splits an iterable on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and
     * the iterable does not contain a .size() method. Returns `ValueType` as iterator is dereferenced. Example:
     * ```cpp
     * std::array<int, 5> arr = { 1, 2, 3, 4, 5 };
     * std::vector<int> to_split = { 3, 4 }; // must be by reference
     * using iterable = lz::basic_iterable<decltype(arr.begin)>;
     * auto splitted = lz::t_split<iterable>(arr, to_split); // {{1, 2}, {5}} where value_type =
     * // lz::basic_iterable<decltype(arr.begin)>
     * // which is equal to lz::split(arr, to_split)
     * ```
     * @param iterable The iterable to split. Must be an actual reference.
     * @param delimiter The delimiter to split on. Must be an actual reference
     */
    template<class Iterable, class Iterable2>
    LZ_NODISCARD constexpr
    enable_if<is_iterable<Iterable2>::value, split_iterable<ValueType, remove_ref<Iterable>, remove_ref<Iterable2>>>
    operator()(Iterable&& iterable, Iterable2&& delimiter) const {
        return { std::forward<Iterable>(iterable), std::forward<Iterable2>(delimiter) };
    }

    // clang-format on

    /**
     * @brief Splits an iterable on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and
     * the iterable does not contain a .size() method. Returns `ValueType` as iterator is dereferenced. Example:
     * ```cpp
     * std::array<int, 5> arr = { 1, 2, 3, 4, 5 };
     * std::vector<int> to_split = { 3, 4 }; // must be by reference
     * using iterable = lz::basic_iterable<decltype(arr.begin)>;
     * auto splitted = arr | lz::t_split<iterable>(to_split); // {{1, 2}, {5}} where value_type =
     * // lz::basic_iterable<decltype(arr.begin)>
     * // which is equal to arr | lz::split(to_split)
     * ```
     * @param delimiter The iterable to split. Must be an actual reference.
     */
    template<class T>
    LZ_CONSTEXPR_CXX_14 enable_if<is_iterable<T>::value, fn_args_holder<adaptor, T>> operator()(T&& delimiter) const {
        return { std::forward<T>(delimiter) };
    }

    /**
     * @brief Splits an iterable on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and
     * the iterable does not contain a .size() method. Returns `ValueType` as iterator is dereferenced. Example:
     * ```cpp
     * std::array<int, 5> arr = { 1, 2, 3, 4, 5 };
     * using iterable = lz::basic_iterable<decltype(arr.begin)>;
     * auto splitted = arr | lz::t_split<iterable>(3); // {{1, 2}, {4, 5}} where value_type =
     * // lz::basic_iterable<decltype(arr.begin)>
     * // which is equal to arr | lz::split(3)
     * ```
     * @param delimiter The iterable to split.
     */
    template<class T>
    LZ_CONSTEXPR_CXX_14 enable_if<!is_iterable<T>::value, fn_args_holder<adaptor, T>> operator()(T delimiter) const {
        return { std::move(delimiter) };
    }

    /**
     * @brief Splits an iterable on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and
     * the iterable does not contain a .size() method. Returns `ValueType` as iterator is dereferenced. Example:
     * ```cpp
     * std::array<char, 5> arr = { 'h', 'e', 'l', 'l', 'o' };
     * auto splitted = arr | lz::t_split<std::vector<char>>("ll"); // {{'h', 'e'}, {'o'}}
     * ```
     * @param delimiter The delimiter to split on, for c-strings.
     */
    template<class CharT>
    LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, copied_basic_sv<CharT>> operator()(const CharT* delimiter) const {
        return (*this)(lz::string_view(delimiter));
    }

    /**
     * @brief Splits an iterable on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel
     * and the iterable does not contain a .size() method. Returns `ValueType` as iterator is dereferenced. Example:
     * ```cpp
     * std::array<char, 5> arr = { 'h', 'e', 'l', 'l', 'o' };
     * auto splitted = arr | lz::t_split<std::vector<char>>(lz::string_view("ll")); // {{'h', 'e'}, {'o'}}
     * ```
     * @param delimiter The string_view delimiter, it's copied and therefore does not have to be a reference.
     */
    template<class CharT>
    LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, copied_basic_sv<CharT>>
    operator()(const basic_string_view<CharT> delimiter) const {
        return (*this)(copied_basic_sv<CharT>(delimiter));
    }
};

template<>
struct split_adaptor<void> {
    using adaptor = split_adaptor<void>;

    template<class Iterable, class Iterble2>
    using splitter_iterable =
        split_iterable<basic_iterable<iter_t<Iterable>, sentinel_t<Iterable>>, remove_ref<Iterable>, remove_ref<Iterble2>>;

    template<class Iterable, class T>
    using splitter_t_iterable = split_iterable<basic_iterable<iter_t<Iterable>, sentinel_t<Iterable>>, remove_ref<Iterable>, T>;

    // clang-format off

    /**
     * @brief Splits an iterable on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and
     * the iterable does not contain a .size() method. Returns `ValueType` as iterator is dereferenced. Example:
     * ```cpp
     * std::array<int, 5> arr = { 1, 2, 3, 4, 5 };
     * auto splitted = lz::split(arr, 2); // { { 1 }, { 3, 4, 5 } } where value_type = lz::basic_iterable<decltype(arr.begin)>
     * ```
     * @param iterable The iterable to split. Must be an actual reference.
     * @param delimiter The delimiter to split on. Doesn't have to be a reference
     */
    template<class Iterable, class T>
    LZ_NODISCARD constexpr enable_if<!is_iterable<T>::value, splitter_t_iterable<Iterable, T>>
    operator()(Iterable&& iterable, T delimiter) const {
        return { std::forward<Iterable>(iterable), std::move(delimiter) };
    }

    /**
     * @brief Splits an iterable on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and
     * the iterable does not contain a .size() method. Returns an iterable of `lz::basic_iterable` as iterator is dereferenced.
     * Example:
     * ```cpp
     * std::array<char, 5> arr = { 'h', 'e', 'l', 'l', 'o' };
     * auto splitted = lz::split(arr, "ll"); // {{'h', 'e'}, {'o'}} where value_type = lz::basic_iterable<decltype(arr.begin)>
     * ```
     * @param iterable The iterable to split. Must be an actual reference.
     * @param delimiter The delimiter to split on, for c-strings.
     */
    template<class Iterable, class CharT>
    LZ_NODISCARD constexpr splitter_iterable<Iterable, copied_basic_sv<CharT>>
    operator()(Iterable&& iterable, const CharT* delimiter) const {
        return (*this)(std::forward<Iterable>(iterable), lz::string_view(delimiter));
    }

    /**
     * @brief Splits an iterable on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and
     * the iterable does not contain a .size() method. Returns an iterable of `lz::basic_iterable` as iterator is dereferenced.
     * Example:
     * ```cpp
     * std::array<char, 5> arr = { 'h', 'e', 'l', 'l', 'o' };
     * auto splitted = lz::split(arr, lz::string_view("ll")); // {{'h', 'e'}, {'o'}} 
     * // where value_type = lz::basic_iterable<decltype(arr.begin)>
     * ```
     * @param iterable The iterable to split. Must be an actual reference.
     * @param delimiter The string_view delimiter, it's copied and therefore does not have to be a reference.
     */
    template<class Iterable, class CharT>
    LZ_NODISCARD constexpr splitter_iterable<Iterable, copied_basic_sv<CharT>>
    operator()(Iterable&& iterable, const basic_string_view<CharT> delimiter) const {
        return (*this)(std::forward<Iterable>(iterable), copied_basic_sv<CharT>(delimiter));
    }

    /**
     * @brief Splits an iterable on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and
     * the iterable does not contain a .size() method. Returns an iterable of `lz::basic_iterable` as iterator is dereferenced.
     * Example:
     * ```cpp
     * std::array<int, 5> arr = { 1, 2, 3, 4, 5 };
     * std::vector<int> to_split = { 3, 4 }; // must be by reference
     * auto splitted = lz::split(arr, to_split); // {{1, 2}, {5}} where value_type = lz::basic_iterable<decltype(arr.begin)>
     * ```
     * @param iterable The iterable to split. Must be an actual reference.
     * @param delimiter The delimiter to split on. Must be an actual reference
     */
    template<class Iterable, class Iterable2>
    LZ_NODISCARD constexpr
    enable_if<is_iterable<Iterable2>::value, splitter_iterable<Iterable, Iterable2>>
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
     * std::vector<int> to_split = { 3, 4 }; // must be by reference
     * auto splitted = arr | lz::split(to_split); // {{1, 2}, {5}} where value_type = lz::basic_iterable<decltype(arr.begin)>
     * ```
     * @param delimiter The iterable to split. Must be an actual reference.
     */
    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_iterable<Iterable>::value, fn_args_holder<adaptor, Iterable>>
    operator()(Iterable&& delimiter) const {
        using a = split_adaptor<basic_iterable<iter_t<Iterable>, sentinel_t<Iterable>>>;
        return a{}(std::forward<Iterable>(delimiter));
    }

    /**
     * @brief Splits an iterable on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and
     * the iterable does not contain a .size() method. Returns `ValueType` as iterator is dereferenced. Example:
     * ```cpp
     * std::array<int, 5> arr = { 1, 2, 3, 4, 5 };
     * using iterable = lz::basic_iterable<decltype(arr.begin)>;
     * auto splitted = arr | lz::t_split<iterable>(3); // {{1, 2}, {4, 5}} where value_type =
     * // lz::basic_iterable<decltype(arr.begin)>
     * // which is equal to arr | lz::split(3)
     * ```
     * @param delimiter The iterable to split.
     */
    template<class T>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_iterable<T>::value, fn_args_holder<adaptor, T>> operator()(T delimiter) const {
        return { std::move(delimiter) };
    }

    /**
     * @brief Splits an iterable on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and
     * the iterable does not contain a .size() method. Returns an iterable of `lz::basic_iterable` as iterator is dereferenced.
     * Example:
     * ```cpp
     * std::array<char, 5> arr = { 'h', 'e', 'l', 'l', 'o' };
     * auto splitted = arr | lz::split("ll"); // {{'h', 'e'}, {'o'}} where value_type = lz::basic_iterable<decltype(arr.begin)>
     * ```
     * @param delimiter The delimiter to split on, for c-strings.
     */
    template<class CharT>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, copied_basic_sv<CharT>> operator()(const CharT* delimiter) const {
        return (*this)(lz::string_view(delimiter));
    }

    /**
     * @brief Splits an iterable on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and
     * the iterable does not contain a .size() method. Returns an iterable of `lz::basic_iterable` as iterator is dereferenced.
     * Example:
     * ```cpp
     * std::array<char, 5> arr = { 'h', 'e', 'l', 'l', 'o' };
     * auto splitted = arr | lz::split(lz::basic_string_view("ll")); // {{'h', 'e'}, {'o'}} where value_type =
     * lz::basic_iterable<decltype(arr.begin)>
     * ```
     * @param delimiter The delimiter to split on. Doesn't have to be a reference.
     */
    template<class CharT>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, copied_basic_sv<CharT>>
    operator()(const basic_string_view<CharT> delimiter) const {
        return (*this)(copied_basic_sv<CharT>(delimiter));
    }
};

} // namespace detail
} // namespace lz

#endif