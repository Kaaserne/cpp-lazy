#pragma once

#ifndef LZ_TAKE_WHILE_ITERABLE_HPP
#define LZ_TAKE_WHILE_ITERABLE_HPP

#include <Lz/detail/func_container.hpp>
#include <Lz/detail/iterators/take_while.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {
template<class Iterable, class UnaryPredicate>
class take_while_iterable : public lazy_view {
    ref_or_view<Iterable> _iterable;
    func_container<UnaryPredicate> _unary_predicate;

    using inner_iter = iter_t<Iterable>;
    using inner_sentinel = sentinel_t<Iterable>;

public:
    using iterator = take_while_iterator<inner_iter, inner_sentinel, func_container<UnaryPredicate>>;
    using sentinel = typename iterator::sentinel;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

#ifdef LZ_HAS_CONCEPTS

    constexpr take_while_iterable()
        requires std::default_initializable<Iterable> && std::default_initializable<UnaryPredicate>
    = default;

#else

    template<class I = decltype(_iterable),
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<UnaryPredicate>::value>>
    constexpr take_while_iterable() noexcept(std::is_nothrow_default_constructible<Iterable>::value &&
                                             std::is_nothrow_default_constructible<UnaryPredicate>::value) {
    }

#endif

    template<class I>
    constexpr take_while_iterable(I&& iterable, UnaryPredicate unary_predicate) :
        _iterable{ std::forward<I>(iterable) },
        _unary_predicate{ std::move(unary_predicate) } {
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr iterator begin() const& {
        if constexpr (is_bidi_tag<typename iterator::iterator_category>::value) {
            return { std::begin(_iterable), std::begin(_iterable), std::end(_iterable), _unary_predicate };
        }
        else {
            return { std::begin(_iterable), std::end(_iterable), _unary_predicate };
        }
    }

#else

    template<class I = typename iterator::iterator_category>
    LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, iterator> begin() const& {
        return { std::begin(_iterable), std::end(_iterable), _unary_predicate };
    }

    template<class I = typename iterator::iterator_category>
    LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, iterator> begin() const& {
        return { std::begin(_iterable), std::begin(_iterable), std::end(_iterable), _unary_predicate };
    }

#endif

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, iterator> begin() && {
        return { detail::begin(std::move(_iterable)), detail::end(std::move(_iterable)), std::move(_unary_predicate) };
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (is_bidi_tag<typename iterator::iterator_category>::value) {
            return iterator{ std::end(_iterable), std::begin(_iterable), std::end(_iterable), _unary_predicate };
        }
        else {
            return default_sentinel{};
        }
    }

#else

    template<class I = typename iterator::iterator_category>
    LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, iterator> end() const {
        return { std::end(_iterable), std::begin(_iterable), std::end(_iterable), _unary_predicate };
    }

    template<class I = typename iterator::iterator_category>
    constexpr enable_if<!is_bidi_tag<I>::value, default_sentinel> end() const noexcept {
        return {};
    }
#endif
};
} // namespace detail
} // namespace lz

#endif