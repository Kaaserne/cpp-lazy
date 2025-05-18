#pragma once

#include <utility>
#ifndef LZ_CARTESIAN_PRODUCT_ITERABLE_HPP
#define LZ_CARTESIAN_PRODUCT_ITERABLE_HPP

#include <Lz/detail/iterators/cartesian_product.hpp>
#include <Lz/detail/ref_or_view.hpp>
#include <Lz/detail/tuple_helpers.hpp>
#include <numeric>

namespace lz {
namespace detail {
template<class Iterable, class... Iterables>
class cartesian_product_iterable : public lazy_view {
    ref_or_view<Iterable> _first_iterable;
    std::tuple<ref_or_view<Iterables>...> _rest_iterables;

    template<std::size_t... Is>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 std::size_t size(index_sequence<Is...>) const {
        const std::size_t sizes[] = { static_cast<std::size_t>(lz::size(_first_iterable)),
                                      static_cast<std::size_t>(lz::size(std::get<Is>(_rest_iterables)))... };
        return std::accumulate(std::begin(sizes), std::end(sizes), std::size_t{ 1 }, std::multiplies<std::size_t>{});
    }

    template<std::ptrdiff_t I, class Iterator, class... Iterators>
    LZ_CONSTEXPR_CXX_14 enable_if<(I >= 0)>
    init_iterators(Iterator& first_it, const std::tuple<Iterators...>& rest_it, bool& first_at_end) const {
        const bool this_at_end = std::get<I>(rest_it) == std::end(std::get<I>(_rest_iterables));
        if (this_at_end && !first_at_end) {
            first_it = std::end(_first_iterable);
            first_at_end = true;
        }
        init_iterators<I - 1>(first_it, rest_it, first_at_end);
    }

    template<std::ptrdiff_t I, class Iterator, class... Iterators>
    LZ_CONSTEXPR_CXX_14 enable_if<(I < 0)> init_iterators(Iterator&, const std::tuple<Iterators...>&, bool&) const noexcept {
    }

public:
    using iterator = cartesian_product_iterator<std::tuple<iter_t<Iterable>, iter_t<Iterables>...>,
                                                std::tuple<sentinel_t<Iterable>, sentinel_t<Iterables>...>>;
    using sentinel = typename iterator::sentinel;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    template<class I, class... Is>
    LZ_CONSTEXPR_CXX_14 cartesian_product_iterable(I&& first, Is&&... rest) :
        _first_iterable{ std::forward<I>(first) },
        _rest_iterables{ std::forward<Is>(rest)... } {
    }

    template<class T = conjunction<sized<Iterables>...>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<T::value, std::size_t> size() const {
        return size(make_index_sequence<sizeof...(Iterables)>{});
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        auto first_it = std::begin(_first_iterable);
        auto first_end = std::end(_first_iterable);

        auto rest_it = begin_tuple(_rest_iterables);
        auto end = end_tuple(_rest_iterables);
        bool first_at_end = first_it == first_end;

        init_iterators<static_cast<std::ptrdiff_t>(sizeof...(Iterables) - 1)>(first_it, rest_it, first_at_end);

        return { std::tuple_cat(std::make_tuple(first_it), rest_it), std::tuple_cat(std::make_tuple(first_it), rest_it),
                 std::tuple_cat(std::make_tuple(first_end), end) };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, iterator> begin() && {
        auto first_it = std::begin(_first_iterable);
        auto first_end = std::end(_first_iterable);

        auto rest_it = begin_tuple(_rest_iterables);
        auto end = end_tuple(_rest_iterables);
        bool first_at_end = first_it == first_end;

        init_iterators<static_cast<std::ptrdiff_t>(sizeof...(Iterables) - 1)>(first_it, rest_it, first_at_end);

        return { std::tuple_cat(std::make_tuple(first_it), rest_it), std::tuple_cat(std::make_tuple(first_it), rest_it),
                 std::tuple_cat(std::make_tuple(first_end), end) };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, iterator> end() const {
        auto first_it = std::end(_first_iterable);
        auto first_end = std::end(_first_iterable);

        auto rest_it = begin_tuple(_rest_iterables);
        auto end = end_tuple(_rest_iterables);

        return { std::tuple_cat(std::make_tuple(first_it), rest_it), std::tuple_cat(std::make_tuple(first_it), rest_it),
                 std::tuple_cat(std::make_tuple(first_end), end) };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, sentinel> end() const noexcept {
        return {};
    }

    template<class Iterable2>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend cartesian_product_iterable<remove_ref<Iterable2>, Iterable, Iterables...>
    operator|(Iterable2&& iterable2, cartesian_product_iterable<Iterable, Iterables...>&& cartesian) {
        return { std::forward<Iterable2>(iterable2),
                 std::tuple_cat(std::make_tuple(std::move(cartesian._first_iterable)), std::move(cartesian._rest_iterables)) };
    }

    template<class Iterable2>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend cartesian_product_iterable<remove_ref<Iterable2>, Iterable, Iterables...>
    operator|(Iterable2&& iterable2, const cartesian_product_iterable<Iterable, Iterables...>& cartesian) {
        return { std::forward<Iterable2>(iterable2),
                 std::tuple_cat(std::make_tuple(cartesian._first_iterable), cartesian._rest_iterables) };
    }
};

} // namespace detail
} // namespace lz
#endif
