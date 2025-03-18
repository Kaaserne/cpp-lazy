#pragma once

#ifndef LZ_CARTESIAN_PRODUCT_ITERABLE_HPP
#define LZ_CARTESIAN_PRODUCT_ITERABLE_HPP

#include <Lz/detail/iterators/cartesian_product.hpp>
#include <Lz/detail/ref_or_view.hpp>
#include <Lz/detail/tuple_helpers.hpp>

namespace lz {
namespace detail {
template<class... Iterables>
class cartesian_product_iterable : public lazy_view {
    std::tuple<ref_or_view<Iterables>...> _iterables;

    template<std::size_t... Is>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 std::size_t size(index_sequence_helper<Is...>) const {
        const std::size_t sizes[] = { static_cast<std::size_t>(lz::size(std::get<Is>(_iterables)))... };
        return std::accumulate(std::begin(sizes), std::end(sizes), std::size_t{ 1 }, std::multiplies<std::size_t>{});
    }

public:
    using iterator = cartesian_product_iterator<std::tuple<iter_t<Iterables>...>, std::tuple<sentinel_t<Iterables>...>>;
    using sentinel = typename iterator::sentinel;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr cartesian_product_iterable() = default;

    template<class... Is>
    LZ_CONSTEXPR_CXX_14 cartesian_product_iterable(Is&&... iterables) : _iterables{ std::forward<Is>(iterables)... } {
    }

    template<class T = conjunction<sized<Iterables>...>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<T::value, std::size_t> size() const {
        return size(make_index_sequence<sizeof...(Iterables)>{});
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const {
        return { begin_tuple(_iterables), begin_tuple(_iterables), end_tuple(_iterables) };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, iterator> end() const {
        return { end_tuple(_iterables), begin_tuple(_iterables), end_tuple(_iterables) };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, sentinel> end() const noexcept {
        return {};
    }

    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend cartesian_product_iterable<Iterable, Iterables...>
    operator|(Iterable&& iterable, cartesian_product_iterable<Iterables...>&& cartesian) {
        return { std::tuple_cat(std::make_tuple(ref_or_view<Iterable>(std::forward<Iterable>(iterable))),
                                std::move(cartesian._iterables)) };
    }

    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend cartesian_product_iterable<Iterable, Iterables...>
    operator|(Iterable&& iterable, const cartesian_product_iterable<Iterables...>& cartesian) {
        return { std::tuple_cat(std::make_tuple(ref_or_view<Iterable>(std::forward<Iterable>(iterable))), cartesian._iterables) };
    }
};

} // namespace detail
} // namespace lz
#endif
