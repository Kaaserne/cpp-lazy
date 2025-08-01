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
template<class Iterable, class UnaryPredicate, class = void>
class drop_while_iterable;

template<class Iterable, class UnaryPredicate>
class drop_while_iterable<Iterable, UnaryPredicate, enable_if<!is_ra<iter_t<Iterable>>::value>> : public lazy_view {
    maybe_owned<Iterable> _iterable;
    func_container<UnaryPredicate> _unary_predicate;

public:
    using iterator = iter_t<Iterable>;
    using sentinel = sentinel_t<Iterable>;
    using const_iterator = iterator;
    using value_type = val_iterable_t<Iterable>;

private:
    static constexpr bool return_sentinel = !is_bidi_tag<iter_cat_t<iterator>>::value || is_sentinel<iterator, sentinel>::value;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr drop_while_iterable()
        requires std::default_initializable<maybe_owned<Iterable>> && std::default_initializable<UnaryPredicate>
    = default;

#else

    template<class I = decltype(_iterable),
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<UnaryPredicate>::value>>
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
            return std::end(_iterable);
        }
        else {
            return lz::default_sentinel;
        }
    }

#else

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!R, sentinel> end() const {
        return std::end(_iterable);
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD constexpr enable_if<R, default_sentinel_t> end() const {
        return {};
    }

#endif
};

template<class Iterable, class UnaryPredicate>
class drop_while_iterable<Iterable, UnaryPredicate, enable_if<is_ra<iter_t<Iterable>>::value>> : public lazy_view {
    iter_t<Iterable> _begin;
    sentinel_t<Iterable> _end;

public:
    using iterator = iter_t<Iterable>;
    using sentinel = sentinel_t<Iterable>;
    using const_iterator = iterator;
    using value_type = val_iterable_t<Iterable>;

    template<class I>
    LZ_CONSTEXPR_CXX_14 drop_while_iterable(I&& iterable, UnaryPredicate unary_predicate) :
        _begin{ lz::find_if_not(iterable, std::move(unary_predicate)) },
        _end{ std::forward<I>(iterable).end() } {
    }

    template<class I = iter_t<Iterable>>
    LZ_NODISCARD constexpr enable_if<is_ra<I>::value, std::size_t> size() const {
        return static_cast<std::size_t>(_end - _begin);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return std::move(_begin);
    }

    LZ_NODISCARD constexpr iterator begin() const& {
        return _begin;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 sentinel end() && {
        return std::move(_end);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 sentinel end() const& {
        return _end;
    }
};
} // namespace detail
} // namespace lz

#endif