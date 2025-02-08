#pragma once

#ifndef LZ_CONCATENATE_ITERABLE_HPP
#define LZ_CONCATENATE_ITERABLE_HPP

#include <Lz/detail/iterators/concatenate.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {

template<class... Iterables>
class concatenate_iterable : public lazy_view {
    std::tuple<ref_or_view<Iterables>...> _iterables;

    using is = make_index_sequence<sizeof...(Iterables)>;
    using concat = concatenate_iterable<Iterables...>;

    template<std::size_t... I>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_all_same<sized<decltype(std::get<I>(_iterables))>...>::value, std::size_t>
    size_impl(index_sequence_helper<I...>) const {
        const std::size_t sizes[] = { lz::size(std::get<I>(_iterables))... };
        return std::accumulate(std::begin(sizes), std::end(sizes), std::size_t{ 0 });
    }

    // clang-format off

    template<std::size_t... I>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14
    enable_if<!is_all_same<sized<decltype(std::get<I>(_iterables))>...>::value, void>
    size_impl(index_sequence_helper<I...>) const;

    // clang-format on

public:
    using iterator = concatenate_iterator<std::tuple<iter_t<Iterables>...>, std::tuple<sentinel_t<Iterables>...>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    template<class... Is>
    LZ_CONSTEXPR_CXX_14 concatenate_iterable(Is&&... iterables) : _iterables{ std::forward<Is>(iterables)... } {
    }

    constexpr concatenate_iterable() = default;

    template<class I = decltype(std::declval<concat>().size_impl(is{}))>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<std::is_same<I, std::size_t>::value, std::size_t> size() const {
        return size_impl(is{});
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { begin_tuple(_iterables, is{}), begin_tuple(_iterables, is{}), end_tuple(_iterables, is{}) };
    }

    template<class I = iterator>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi<I>::value, iterator> begin() && {
        return { begin_tuple(std::move(_iterables), is{}), begin_tuple(std::move(_iterables), is{}),
                 end_tuple(std::move(_iterables), is{}) };
    }

    template<class I = iterator>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi<I>::value, iterator> end() const {
        return { end_tuple(_iterables, is{}), begin_tuple(_iterables, is{}), end_tuple(_iterables, is{}) };
    }

    template<class I = iterator>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi<I>::value, default_sentinel> end() const {
        return {};
    }

    // TODO make generic function for this
    template<class Iterable>
    friend concatenate_iterable<Iterable, Iterables...>
    operator|(Iterable&& iterable, concatenate_iterable<Iterables...>&& concatenate) {
        return { std::tuple_cat(std::make_tuple(ref_or_view<Iterable>(std::forward<Iterable>(iterable))),
                                std::move(concatenate._iterables)) };
    }

    template<class Iterable>
    friend concatenate_iterable<Iterable, Iterables...>
    operator|(Iterable&& iterable, concatenate_iterable<Iterables...>& concatenate) {
        return { std::tuple_cat(std::make_tuple(ref_or_view<Iterable>(std::forward<Iterable>(iterable))),
                                concatenate._iterables) };
    }

    template<class Iterable>
    friend concatenate_iterable<Iterable, Iterables...>
    operator|(Iterable&& iterable, const concatenate_iterable<Iterables...>& concatenate) {
        return { std::tuple_cat(std::make_tuple(ref_or_view<Iterable>(std::forward<Iterable>(iterable))),
                                concatenate._iterables) };
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_CONCATENATE_HPP
