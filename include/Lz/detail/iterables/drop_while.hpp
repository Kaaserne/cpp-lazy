#pragma once

#ifndef LZ_DROP_WHILE_ITERABLE_HPP
#define LZ_DROP_WHILE_ITERABLE_HPP

#include <Lz/algorithm.hpp>
#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/func_container.hpp>
#include <Lz/detail/maybe_owned.hpp>
#include <Lz/detail/traits.hpp>

namespace lz {
namespace detail {
template<class Iterable, class UnaryPredicate>
class drop_while_iterable : public lazy_view {
    maybe_owned<Iterable> _iterable;
    func_container<UnaryPredicate> _unary_predicate;

public:
    using iterator = iter_t<Iterable>;
    using sentinel = sentinel_t<Iterable>;
    using const_iterator = iterator;
    using value_type = val_iterable_t<Iterable>;

private:
    static constexpr bool return_sentinel = !is_bidi_tag<iter_cat_t<iterator>>::value || has_sentinel<Iterable>::value;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr drop_while_iterable()
        requires(std::default_initializable<maybe_owned<Iterable>> && std::default_initializable<UnaryPredicate>)
    = default;

#else

    template<class I = decltype(_iterable),
             class = enable_if_t<std::is_default_constructible<I>::value && std::is_default_constructible<UnaryPredicate>::value>>
    constexpr drop_while_iterable() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                             std::is_nothrow_default_constructible<UnaryPredicate>::value) {
    }

#endif

    template<class I>
    constexpr drop_while_iterable(I&& iterable, UnaryPredicate unary_predicate) :
        _iterable{ std::forward<I>(iterable) },
        _unary_predicate{ std::move(unary_predicate) } {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return lz::find_if_not(_iterable, std::move(_unary_predicate));
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return lz::find_if_not(_iterable, _unary_predicate);
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (!return_sentinel) {
            return _iterable.end();
        }
        else {
            return lz::default_sentinel;
        }
    }

#else

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<!R, sentinel> end() const {
        return _iterable.end();
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD constexpr enable_if_t<R, default_sentinel_t> end() const {
        return {};
    }

#endif
};
} // namespace detail
} // namespace lz

#endif
