#pragma once

#ifndef LZ_CONCATENATE_ITERABLE_HPP
#define LZ_CONCATENATE_ITERABLE_HPP

#include <Lz/detail/iterators/concatenate.hpp>
#include <Lz/detail/ref_or_view.hpp>
#include <Lz/detail/tuple_helpers.hpp>

namespace lz {
namespace detail {

template<class... Iterables>
class concatenate_iterable : public lazy_view {
    std::tuple<ref_or_view<Iterables>...> _iterables;

    template<std::size_t... I>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 std::size_t size(index_sequence_helper<I...>) const {
        const std::size_t sizes[] = { static_cast<std::size_t>(lz::size(std::get<I>(_iterables)))... };
        return std::accumulate(std::begin(sizes), std::end(sizes), std::size_t{ 0 });
    }

public:
    using iterator = concatenate_iterator<std::tuple<iter_t<Iterables>...>, std::tuple<sentinel_t<Iterables>...>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    template<class... Is>
    LZ_CONSTEXPR_CXX_14 concatenate_iterable(Is&&... iterables) : _iterables{ std::forward<Is>(iterables)... } {
    }

    constexpr concatenate_iterable() = default;

    template<class T = conjunction<sized<Iterables>...>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<T::value, std::size_t> size() const {
        return size(make_index_sequence<sizeof...(Iterables)>{});
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { begin_tuple(_iterables), begin_tuple(_iterables), end_tuple(_iterables) };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, iterator> begin() && {
        return { begin_tuple(std::move(_iterables)), begin_tuple(std::move(_iterables)), end_tuple(std::move(_iterables)) };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, iterator> end() const {
        return { end_tuple(_iterables), begin_tuple(_iterables), end_tuple(_iterables) };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, default_sentinel> end() const {
        return {};
    }

    template<class Iterable>
    friend concatenate_iterable<remove_ref<Iterable>, Iterables...>
    operator|(Iterable&& iterable, concatenate_iterable<Iterables...>&& concatenate) {
        return iterable_tuple_cat(std::forward<Iterable>(iterable), std::move(concatenate._iterables));
    }

    template<class Iterable>
    friend concatenate_iterable<remove_ref<Iterable>, Iterables...>
    operator|(Iterable&& iterable, const concatenate_iterable<Iterables...>& concatenate) {
        return iterable_tuple_cat(std::forward<Iterable>(iterable), concatenate._iterables);
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_CONCATENATE_HPP
