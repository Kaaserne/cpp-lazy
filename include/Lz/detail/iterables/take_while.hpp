#pragma once

#ifndef LZ_TAKE_WHILE_ITERABLE_HPP
#define LZ_TAKE_WHILE_ITERABLE_HPP

#include <Lz/detail/func_container.hpp>
#include <Lz/detail/iterators/take_while.hpp>
#include <Lz/detail/maybe_owned.hpp>

namespace lz {
namespace detail {
template<class Iterable, class UnaryPredicate>
class take_while_iterable : public lazy_view {
    maybe_owned<Iterable> _iterable{};
    func_container<UnaryPredicate> _unary_predicate{};

    using it = iter_t<Iterable>;
    using sent = sentinel_t<Iterable>;

public:
    using iterator = take_while_iterator<maybe_owned<Iterable>, func_container<UnaryPredicate>>;
    using sentinel = typename iterator::sentinel;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

private:
    static constexpr bool return_sentinel =
        !is_bidi_tag<typename iterator::iterator_category>::value || is_sentinel<it, sent>::value;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr take_while_iterable()
        requires(std::default_initializable<maybe_owned<Iterable>> && std::default_initializable<UnaryPredicate>)
    = default;

#else

    template<class I = decltype(_iterable),
             class = enable_if_t<std::is_default_constructible<I>::value && std::is_default_constructible<UnaryPredicate>::value>>
    constexpr take_while_iterable() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                             std::is_nothrow_default_constructible<UnaryPredicate>::value) {
    }

#endif

    template<class I>
    constexpr take_while_iterable(I&& iterable, UnaryPredicate unary_predicate) :
        _iterable{ std::forward<I>(iterable) },
        _unary_predicate{ std::move(unary_predicate) } {
    }

    LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { _iterable, _iterable.begin(), _unary_predicate };
    }

#ifdef LZ_HAS_CONCEPTS

    [[nodiscard]] constexpr iterator begin() &&
        requires(return_sentinel)
    {
        return { _iterable, _iterable.begin(), std::move(_unary_predicate) };
    }

#else

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<R, iterator> begin() && {
        return { _iterable, _iterable.begin(), std::move(_unary_predicate) };
    }

#endif

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (!return_sentinel) {
            return iterator{ _iterable, _iterable.end(), _unary_predicate };
        }
        else {
            return lz::default_sentinel;
        }
    }

#else

    template<bool R = return_sentinel>
    LZ_CONSTEXPR_CXX_14 enable_if_t<!R, iterator> end() const {
        return { _iterable, _iterable.end(), _unary_predicate };
    }

    template<bool R = return_sentinel>
    constexpr enable_if_t<R, default_sentinel_t> end() const noexcept {
        return default_sentinel;
    }
#endif
};
} // namespace detail
} // namespace lz

#endif
