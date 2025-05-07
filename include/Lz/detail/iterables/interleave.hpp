#pragma once

#ifndef LZ_INTERLEAVED_ITERABLE_HPP
#define LZ_INTERLEAVED_ITERABLE_HPP

#include <Lz/detail/iterators/interleave.hpp>
#include <Lz/detail/ref_or_view.hpp>
#include <tuple>

namespace lz {
namespace detail {

template<class... Iterables>
class interleave_iterable {
    std::tuple<ref_or_view<Iterables>...> _iterables;

    using sentinel_tuple = std::tuple<sentinel_t<Iterables>...>;

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 std::size_t size(index_sequence<I...>) const {
        return std::min({ static_cast<std::size_t>(std::get<I>(_iterables).size())... }) * sizeof...(Iterables);
    }

    template<std::size_t... I>
    constexpr bool all_not_at_end(index_sequence<I...>) const {
        return std::min({ std::begin(std::get<I>(_iterables)) != std::end(std::get<I>(_iterables))... });
    }

    using index_sequence_for_this = make_index_sequence<sizeof...(Iterables)>;

public:
    using iterator = interleave_iterator<std::tuple<iter_t<Iterables>...>, sentinel_tuple>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    template<class I, class... Is>
    constexpr interleave_iterable(I&& iterable, Is&&... iterables) :
        _iterables{ std::forward<I>(iterable), std::forward<Is>(iterables)... } {
        static_assert(conjunction<std::is_same<ref_iterable_t<I>, ref_iterable_t<Is>>...>::value,
                      "all iterables must contain the same reference type");
    }

    template<class T = conjunction<sized<Iterables>...>>
    LZ_NODISCARD constexpr enable_if<T::value, std::size_t> size() const {
        return size(index_sequence_for_this{});
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, iterator> begin() const {
        using diff_type = typename iterator::difference_type;

        tuple_of_bools<sizeof...(Iterables)> iterator_turns{};
        std::get<0>(iterator_turns) = all_not_at_end(index_sequence_for_this{});
        const tuple_of_distances<sizeof...(Iterables), diff_type> sizes{};

        return { begin_tuple(_iterables), iterator_turns, sizes };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, iterator> begin() const {
        tuple_of_bools<sizeof...(Iterables)> iterator_turns{};
        std::get<0>(iterator_turns) = all_not_at_end(index_sequence_for_this{});
        return { begin_tuple(_iterables), iterator_turns };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD constexpr enable_if<is_bidi_tag<I>::value, iterator> end() const {
        using diff_type = typename iterator::difference_type;
        return { end_tuple(_iterables), tuple_of_bools<sizeof...(Iterables)>{},
                 iterable_tuple_eager_size_as<diff_type>(_iterables, index_sequence_for_this{}) };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, sentinel_tuple> end() const {
        return end_tuple(_iterables);
    }

    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend interleave_iterable<remove_ref<Iterable>, Iterables...>
    operator|(Iterable&& iterable, interleave_iterable<Iterables...>&& interleaved) {
        return iterable_tuple_cat(std::forward<Iterable>(iterable), std::move(interleaved._iterables));
    }

    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend interleave_iterable<remove_ref<Iterable>, Iterables...>
    operator|(Iterable&& iterable, const interleave_iterable<Iterables...>& interleaved) {
        return iterable_tuple_cat(std::forward<Iterable>(iterable), interleaved._iterables);
    }
};

} // namespace detail
} // namespace lz
#endif // LZ_INTERLEAVED_ITERABLE_HPP
