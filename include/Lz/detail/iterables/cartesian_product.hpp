#pragma once

#ifndef LZ_CARTESIAN_PRODUCT_ITERABLE_HPP
#define LZ_CARTESIAN_PRODUCT_ITERABLE_HPP

#include <Lz/detail/iterators/cartesian_product.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {
template<class... Iterables>
class cartesian_product_iterable : public lazy_view {
    std::tuple<ref_or_view<Iterables>...> _iterables;

    using is = make_index_sequence<sizeof...(Iterables)>;

public:
    using iterator = cartesian_product_iterator<std::tuple<iter_t<Iterables>...>, std::tuple<sentinel_t<Iterables>...>>;
    using sentinel = typename iterator::sentinel;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr cartesian_product_iterable() = default;

    template<class... Is>
    LZ_CONSTEXPR_CXX_14 cartesian_product_iterable(Is&&... iterables) : _iterables{ std::forward<Is>(iterables)... } {
    }

    // TODO add size, also add test for that

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const {
        return { begin_tuple(_iterables, is{}), begin_tuple(_iterables, is{}), end_tuple(_iterables, is{}) };
    }

    template<class I = iterator>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi<I>::value, iterator> end() const {
        return { end_tuple(_iterables, is{}), begin_tuple(_iterables, is{}), end_tuple(_iterables, is{}) };
    }

    template<class I = iterator>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi<I>::value, sentinel> end() const noexcept {
        return {};
    }

    template<class Iterable>
    friend cartesian_product_iterable<Iterable, Iterables...>
    operator|(Iterable&& iterable, cartesian_product_iterable<Iterables...>&& cartesian) {
        return { std::tuple_cat(std::make_tuple(ref_or_view<Iterable>(std::forward<Iterable>(iterable))),
                                std::move(cartesian._iterables)) };
    }

    template<class Iterable>
    friend cartesian_product_iterable<Iterable, Iterables...>
    operator|(Iterable&& iterable, cartesian_product_iterable<Iterables...>& cartesian) {
        return { std::tuple_cat(std::make_tuple(ref_or_view<Iterable>(std::forward<Iterable>(iterable))), cartesian._iterables) };
    }

    template<class Iterable>
    friend cartesian_product_iterable<Iterable, Iterables...>
    operator|(Iterable&& iterable, const cartesian_product_iterable<Iterables...>& cartesian) {
        return { std::tuple_cat(std::make_tuple(ref_or_view<Iterable>(std::forward<Iterable>(iterable))), cartesian._iterables) };
    }
};

} // namespace detail
} // namespace lz
#endif
