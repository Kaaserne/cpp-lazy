#pragma once

#ifndef LZ_CARTESIAN_PRODUCT_ITERABLE_HPP
#define LZ_CARTESIAN_PRODUCT_ITERABLE_HPP

#include <Lz/detail/iterators/cartesian_product.hpp>
#include <Lz/detail/maybe_owned.hpp>
#include <Lz/detail/tuple_helpers.hpp>
#include <numeric>

namespace lz {
namespace detail {
template<class... Iterables>
class cartesian_product_iterable : public lazy_view {
    maybe_homogeneous_t<maybe_owned<Iterables>...> _iterables;

    template<size_t... Is>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 size_t size(index_sequence<Is...>) const {
        using std::get;
        const size_t sizes[] = { static_cast<size_t>(lz::size(get<Is>(_iterables)))... };
        return std::accumulate(std::begin(sizes), std::end(sizes), size_t{ 1 }, std::multiplies<size_t>{});
    }

#ifdef LZ_HAS_CXX_17

    template<std::ptrdiff_t I, class Iterators>
    constexpr void init_iterators(Iterators& it, bool& first_at_end) const {
        using std::get;
        if constexpr (I >= 0) {
            const bool this_at_end = get<I>(it) == get<I>(_iterables).end();
            if (this_at_end && !first_at_end) {
                get<0>(it) = get<0>(_iterables).end();
                first_at_end = true;
                return;
            }
            init_iterators<I - 1>(it, first_at_end);
        }
    }

#else

    template<std::ptrdiff_t I, class Iterators>
    LZ_CONSTEXPR_CXX_14 enable_if_t<(I >= 0)> init_iterators(Iterators& it, bool& first_at_end) const {
        using std::get;
        const bool this_at_end = get<I>(it) == get<I>(_iterables).end();
        if (this_at_end && !first_at_end) {
            get<0>(it) = get<0>(_iterables).end();
            first_at_end = true;
            return;
        }
        init_iterators<I - 1>(it, first_at_end);
    }

    template<std::ptrdiff_t I, class Iterators>
    LZ_CONSTEXPR_CXX_14 enable_if_t<(I < 0)> init_iterators(Iterators&, bool&) const noexcept {
    }

#endif

    template<class Iterable2, size_t... Is>
    static cartesian_product_iterable<remove_ref<Iterable2>, Iterables...>
    concat_iterables(Iterable2&& iterable2, cartesian_product_iterable<Iterables...>&& cartesian, index_sequence<Is...>) {
        using std::get;
        return { std::forward<Iterable2>(iterable2), std::move(get<Is>(cartesian._iterables))... };
    }

    template<class Iterable2, size_t... Is>
    static cartesian_product_iterable<remove_ref<Iterable2>, Iterables...>
    concat_iterables(Iterable2&& iterable2, const cartesian_product_iterable<Iterables...>& cartesian, index_sequence<Is...>) {
        using std::get;
        return { std::forward<Iterable2>(iterable2), get<Is>(cartesian._iterables)... };
    }

    static constexpr size_t tuple_size = sizeof...(Iterables);
    using is = make_index_sequence<tuple_size>;

public:
    using iterator = cartesian_product_iterator<maybe_homogeneous_t<maybe_owned<Iterables>...>>;
    using sentinel = typename iterator::sentinel;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

private:
    static constexpr bool return_sentinel =
        !is_bidi_tag<typename iterator::iterator_category>::value || disjunction<has_sentinel<Iterables>...>::value;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr cartesian_product_iterable()
        requires(std::default_initializable<maybe_owned<Iterables>> && ...)
    = default;

#else

    template<class I = decltype(_iterables), class = enable_if_t<std::is_default_constructible<I>::value>>
    constexpr cartesian_product_iterable() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    template<class... Is>
    LZ_CONSTEXPR_CXX_14 cartesian_product_iterable(Is&&... iterables) : _iterables{ std::forward<Is>(iterables)... } {
    }

#ifdef LZ_HAS_CONCEPTS

    [[nodiscard]] constexpr size_t size() const
        requires(sized<Iterables> && ...)
    {
        return size(is{});
    }

#else

    template<bool S = conjunction<is_sized<Iterables>...>::value>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<S, size_t> size() const {
        return size(is{});
    }

#endif

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        using std::get;
        auto it = begin_maybe_homo(_iterables);
        auto end = end_maybe_homo(_iterables);
        auto first_at_end = get<0>(it) == get<0>(end);
        init_iterators<static_cast<std::ptrdiff_t>(tuple_size) - 1>(it, first_at_end);
        return { _iterables, it };
    }

#ifdef LZ_HAS_CONCEPTS

    [[nodiscard]] constexpr iterator begin() &&
        requires(return_sentinel)
    {
        using std::get;
        auto it = begin_maybe_homo(_iterables);
        auto end = end_maybe_homo(_iterables);
        auto first_at_end = get<0>(it) == get<0>(end);
        init_iterators<static_cast<std::ptrdiff_t>(tuple_size) - 1>(it, first_at_end);
        return { std::move(_iterables), it };
    }

#else

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<R, iterator> begin() && {
        using std::get;
        auto it = begin_maybe_homo(_iterables);
        auto end = end_maybe_homo(_iterables);
        auto first_at_end = get<0>(it) == get<0>(end);
        init_iterators<static_cast<std::ptrdiff_t>(tuple_size) - 1>(it, first_at_end);
        return { std::move(_iterables), it };
    }

#endif

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        using std::get;
        if constexpr (!return_sentinel) {
            auto rest_it = begin_maybe_homo(_iterables);
            auto end = end_maybe_homo(_iterables);
            get<0>(rest_it) = get<0>(end);
            return iterator{ _iterables, rest_it };
        }
        else {
            return sentinel{};
        }
    }

#else

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<!R, iterator> end() const {
        using std::get;
        auto rest_it = begin_maybe_homo(_iterables);
        auto end = end_maybe_homo(_iterables);
        get<0>(rest_it) = get<0>(end);
        return iterator{ _iterables, rest_it };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<R, sentinel> end() const noexcept {
        return {};
    }

#endif

    template<class Iterable2>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend cartesian_product_iterable<remove_ref<Iterable2>, Iterables...>
    operator|(Iterable2&& iterable2, cartesian_product_iterable<Iterables...>&& cartesian) {
        return concat_iterables(std::forward<Iterable2>(iterable2), std::move(cartesian), is{});
    }

    template<class Iterable2>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend cartesian_product_iterable<remove_ref<Iterable2>, Iterables...>
    operator|(Iterable2&& iterable2, const cartesian_product_iterable<Iterables...>& cartesian) {
        return concat_iterables(std::forward<Iterable2>(iterable2), cartesian, is{});
    }
};

} // namespace detail
} // namespace lz
#endif
