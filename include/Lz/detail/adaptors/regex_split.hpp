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

    template<class String>
    LZ_NODISCARD regex_split_iterable<regex_it<String>, regex_it<String>>
    operator()(const String& s, const std::basic_regex<typename String::value_type>& regex) const {
        using token_iter = std::regex_token_iterator<const_iter<String>>;
        token_iter first(s.begin(), s.end(), regex, -1);
        return (*this)(std::move(first), token_iter{});
    }

    template<class CharT>
    LZ_NODISCARD fn_args_holder<adaptor, const std::basic_regex<CharT>&> operator()(const std::basic_regex<CharT>& regex) const {
        return { regex };
    }

    template<class RegexTokenIter, class RegexTokenSentinel>
    LZ_NODISCARD regex_split_iterable<RegexTokenIter, RegexTokenSentinel>
    operator()(RegexTokenIter first, RegexTokenSentinel last) const {
        return { std::move(first), std::move(last) };
    }
};
} // namespace detail
} // namespace lz

#endif