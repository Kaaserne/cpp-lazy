#pragma once

#ifndef LZ_REGEX_SPLIT_ITERATOR_HPP
#define LZ_REGEX_SPLIT_ITERATOR_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/iterator.hpp>
#include <Lz/detail/sentinel_with.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>
#include <Lz/util/string_view.hpp>

namespace lz {
namespace detail {
template<class RegexTokenIter>
using regex_split_value_type = basic_string_view<typename RegexTokenIter::regex_type::value_type>;

template<class RegexTokenIter>
using regex_split_val = typename val_t<RegexTokenIter>::string_type::value_type;

template<class RegexTokenIter, class RegexTokenSentinel>
class regex_split_iterator : public iterator<regex_split_iterator<RegexTokenIter, RegexTokenSentinel>,
                                             basic_string_view<regex_split_val<RegexTokenIter>>,
                                             fake_ptr_proxy<basic_string_view<regex_split_val<RegexTokenIter>>>,
                                             diff_type<RegexTokenIter>, std::forward_iterator_tag, sentinel_with<RegexTokenSentinel>> {
public:
    using value_type = basic_string_view<regex_split_val<RegexTokenIter>>;
    using difference_type = typename RegexTokenIter::difference_type;
    using pointer = fake_ptr_proxy<value_type>;
    using reference = value_type;

private:
    RegexTokenIter _current{};

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr regex_split_iterator()
        requires(std::default_initializable<RegexTokenIter>)
    = default;

#else

    template<class R = RegexTokenIter, class = enable_if_t<std::is_default_constructible<R>::value>>
    constexpr regex_split_iterator() noexcept(std::is_nothrow_default_constructible<R>::value) {
    }

#endif

    regex_split_iterator(RegexTokenIter first, RegexTokenSentinel last) : _current{ std::move(first) } {
        while (_current != last && _current->length() == 0) {
            ++_current;
        }
    }

    LZ_CONSTEXPR_CXX_14 regex_split_iterator& operator=(sentinel_with<RegexTokenSentinel> end) {
        _current = std::move(end.value);
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        ++_current;
    }

    constexpr value_type dereference() const {
        return value_type(&*_current->first, static_cast<size_t>(_current->length()));
    }

    constexpr pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    constexpr bool eq(const regex_split_iterator& other) const {
        return _current == other._current;
    }

    constexpr bool eq(sentinel_with<RegexTokenSentinel> end) const {
        return _current == end.value;
    }
};

} // namespace detail
} // namespace lz

#endif
