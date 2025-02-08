#pragma once

#ifndef LZ_REGEX_SPLIT_ITERABLE_HPP
#define LZ_REGEX_SPLIT_ITERABLE_HPP

#include <Lz/detail/iterators/regex_split.hpp>
#include <Lz/detail/ref_or_view.hpp>
#include <regex>

namespace lz {
namespace detail {
template<class RegexTokenIter, class RegexTokenSentinel>
class regex_split_iterable : public lazy_view {
    RegexTokenIter _begin;
    RegexTokenSentinel _end;

public:
    using iterator = regex_split_iterator<RegexTokenIter, RegexTokenSentinel>;
    using const_iterator = iterator;
    using value_type = typename RegexTokenIter::value_type;

    constexpr regex_split_iterable(RegexTokenIter begin, RegexTokenSentinel end) :
        _begin{ std::move(begin) },
        _end{ std::move(end) } {
    }

    constexpr regex_split_iterable() = default;

    LZ_NODISCARD constexpr iterator begin() const {
        return { _begin, _end };
    }

    LZ_NODISCARD constexpr RegexTokenSentinel end() const {
        return _end;
    }
};

LZ_MODULE_EXPORT_SCOPE_END

} // namespace detail
} // namespace lz

#endif