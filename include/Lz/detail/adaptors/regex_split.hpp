#pragma once

#ifndef LZ_REGEX_SPLIT_ADAPTOR_HPP
#define LZ_REGEX_SPLIT_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/iterables/regex_split.hpp>
#include <regex>

namespace lz {
namespace detail {
struct regex_split_adaptor {
    using adaptor = regex_split_adaptor;

    template<class String>
    using const_iter = typename String::const_iterator;

    template<class String>
    using regex_it = std::regex_token_iterator<const_iter<String>>;

    /**
     * @brief Splits a string based on a regex. The regex must be by reference. The `begin()` and `end()` types are different, but
     * `end()` is not an 'actual' sentinel. Rather, its the same type as its input iterator `end()` type. For std::regex, this
     * means `end() == std::regex_token_iterator<>{}` and `begin() = lz::regex_split_iterator`. It does not contain a .size()
     * method and its iterator category is forward. Example:
     * ```cpp
     * std::regex r1(R"(\s+)");
     * std::string s = "    Hello, world! How are you?";
     * auto splitter = lz::regex_split(s, r1); // { "Hello,", "world!", "How", "are", "you?" }
     * ```
     * @param s The string to split
     * @param regex The regex to use to split the string with
     */
    template<class String>
    LZ_NODISCARD regex_split_iterable<regex_it<String>, regex_it<String>>
    operator()(const String& s, const std::basic_regex<typename String::value_type>& regex) const {
        using token_iter = std::regex_token_iterator<const_iter<String>>;
        token_iter first(s.begin(), s.end(), regex, -1);
        return (*this)(std::move(first), token_iter{});
    }

    /**
     * @brief Splits a string based on a regex. The regex must be by reference. The `begin()` and `end()` types are different, but
     * `end()` is not an 'actual' sentinel. Rather, its the same type as its input iterator `end()` type. For std::regex, this
     * means `end() == std::regex_token_iterator<>{}` and `begin() = lz::regex_split_iterator`. It does not contain a .size()
     * method and its iterator category is forward. Example:
     * ```cpp
     * std::regex r1(R"(\s+)");
     * std::string s = "    Hello, world! How are you?";
     * auto splitter = s | lz::regex_split(r1); // { "Hello,", "world!", "How", "are", "you?" }
     * ```
     * @param regex The regex to use to split the string with
     */
    template<class CharT>
    LZ_NODISCARD fn_args_holder<adaptor, const std::basic_regex<CharT>&> operator()(const std::basic_regex<CharT>& regex) const {
        return { regex };
    }

    /**
     * @brief Splits a string based on a regex. The regex must be by reference. The `begin()` and `end()` types are different, but
     * `end()` is not an 'actual' sentinel. Rather, its the same type as its input iterator `end()` type. For std::regex, this
     * means `end() == std::regex_token_iterator<>{}` and `begin() = lz::regex_split_iterator`. It does not contain a .size()
     * method and its iterator category is forward. Example:
     * ```cpp
     * std::regex r1(R"(\s+)");
     * std::string s = "    Hello, world! How are you?";
     * auto begin = std::sregex_token_iterator(s.begin(), s.end(), r1, -1); // -1 means it will skip the matches (which is
     * // exactly what we want), rather than returning the matches
     * auto end = std::sregex_token_iterator<>();
     * auto splitter = lz::regex_split(begin, end); // { "Hello,", "world!", "How", "are", "you?" }
     * ```
     * @param first The first regex iterator
     * @param last The last regex iterator
     */
    template<class RegexTokenIter, class RegexTokenSentinel>
    LZ_NODISCARD regex_split_iterable<RegexTokenIter, RegexTokenSentinel>
    operator()(RegexTokenIter first, RegexTokenSentinel last) const {
        return { std::move(first), std::move(last) };
    }
};
} // namespace detail
} // namespace lz

#endif