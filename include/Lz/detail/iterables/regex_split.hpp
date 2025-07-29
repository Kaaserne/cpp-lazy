#pragma once

#ifndef LZ_REGEX_SPLIT_ITERABLE_HPP
#define LZ_REGEX_SPLIT_ITERABLE_HPP

#include <Lz/detail/iterators/regex_split.hpp>
#include <Lz/detail/maybe_owned.hpp>

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

#ifdef LZ_HAS_CONCEPTS

    constexpr regex_split_iterable()
        requires std::default_initializable<RegexTokenIter> && std::default_initializable<RegexTokenSentinel>
    = default;

#else

    template<class I = RegexTokenIter, class = enable_if<std::is_default_constructible<I>::value &&
                                                         std::is_default_constructible<RegexTokenSentinel>::value>>
    constexpr regex_split_iterable() noexcept(std::is_nothrow_default_constructible<RegexTokenIter>::value &&
                                              std::is_nothrow_default_constructible<RegexTokenSentinel>::value) {
    }

#endif

    constexpr regex_split_iterable(RegexTokenIter begin, RegexTokenSentinel end) :
        _begin{ std::move(begin) },
        _end{ std::move(end) } {
    }

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