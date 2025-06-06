#pragma once

#ifndef LZ_FILTER_ITERABLE_HPP
#define LZ_FILTER_ITERABLE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/func_container.hpp>
#include <Lz/detail/iterators/filter.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {
template<class Iterable, class UnaryPredicate>
class filter_iterable : public lazy_view {
    ref_or_view<Iterable> _iterable;
    func_container<UnaryPredicate> _predicate;

public:
    using iterator = filter_iterator<iter_t<Iterable>, sentinel_t<Iterable>, func_container<UnaryPredicate>>;
    using const_iterator = iterator;
    using sentinel = typename iterator::sentinel;
    using value_type = typename iterator::value_type;

#ifdef LZ_HAS_CONCEPTS

    constexpr filter_iterable()
        requires std::default_initializable<Iterable> && std::default_initializable<UnaryPredicate>
    = default;

#else

    template<class I = decltype(_iterable),
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<UnaryPredicate>::value>>
    constexpr filter_iterable() {
    }

#endif

    template<class I>
    constexpr filter_iterable(I&& iterable, UnaryPredicate predicate) :
        _iterable{ std::forward<I>(iterable) },
        _predicate{ std::move(predicate) } {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { std::begin(_iterable), std::begin(_iterable), std::end(_iterable), _predicate };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, iterator> begin() && {
        return { std::begin(_iterable), std::begin(_iterable), detail::end(std::move(_iterable)), std::move(_predicate) };
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (is_bidi_tag<typename iterator::iterator_category>::value) {
            return iterator{ std::end(_iterable), std::begin(_iterable), std::end(_iterable), _predicate };
        }
        else {
            return default_sentinel{};
        }
    }

#else

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, iterator> end() const {
        return { std::end(_iterable), std::begin(_iterable), std::end(_iterable), _predicate };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD constexpr enable_if<!is_bidi_tag<I>::value, default_sentinel> end() const noexcept {
        return {};
    }

#endif
};
} // namespace detail
} // namespace lz

#endif // end LZ_FILTER_ITERABLE_HPP