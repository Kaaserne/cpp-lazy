#pragma once

#ifndef LZ_DROP_WHILE_ITERABLE_HPP
#define LZ_DROP_WHILE_ITERABLE_HPP

#include <Lz/algorithm/find_if_not.hpp>
#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/func_container.hpp>
#include <Lz/detail/maybe_owned.hpp>

namespace lz {
namespace detail {
template<class Iterable>
class drop_while_iterable : public lazy_view {
    iter_t<Iterable> _begin{};
    sentinel_t<Iterable> _end{};

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
        requires(std::default_initializable<maybe_owned<Iterable>> && std::default_initializable<sentinel_t<Iterable>>)
    = default;

#else

    template<class I = decltype(_begin), class = enable_if_t<std::is_default_constructible<I>::value &&
                                                             std::is_default_constructible<sentinel_t<Iterable>>::value>>
    constexpr drop_while_iterable() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                             std::is_nothrow_default_constructible<sentinel_t<Iterable>>::value) {
    }

#endif

    template<class I, class UnaryPredicate>
    constexpr drop_while_iterable(I&& iterable, UnaryPredicate unary_predicate) :
        _begin{ lz::find_if_not(iterable, std::move(unary_predicate)) },
        _end{ detail::end(iterable) } {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const {
        return _begin;
    }

#ifdef LZ_HAS_CONCEPTS

    [[nodiscard]] constexpr size_t size() const
        requires(is_ra_tag_v<iter_cat_t<iterator>>::value)
    {
        return static_cast<size_t>(_end - _begin);
    }

#else

    template<class I = iter_cat_t<iterator>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<is_ra_tag<I>::value, size_t> size() const {
        return static_cast<size_t>(_end - _begin);
    }

#endif

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (!return_sentinel) {
            return _end;
        }
        else {
            return lz::default_sentinel;
        }
    }

#else

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<!R, sentinel> end() const {
        return _end;
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD constexpr enable_if_t<R, default_sentinel_t> end() const {
        return lz::default_sentinel;
    }

#endif
};
} // namespace detail
} // namespace lz

#endif
