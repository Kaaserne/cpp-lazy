#pragma once

#ifndef LZ_INTERLEAVED_ITERABLE_HPP
#define LZ_INTERLEAVED_ITERABLE_HPP

#include <Lz/detail/iterators/interleave.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {

template<class... Iterables>
class interleave_iterable {
    std::tuple<ref_or_view<Iterables>...> _iterables;

    using iter_tuple = std::tuple<iter_t<Iterables>...>;
    using sentinel_tuple = std::tuple<sentinel_t<Iterables>...>;

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 std::size_t size(index_sequence<I...>) const {
        return std::min({ static_cast<std::size_t>(std::get<I>(_iterables).size())... }) * sizeof...(Iterables);
    }

    using index_sequence_for_this = make_index_sequence<sizeof...(Iterables)>;

public:
    using iterator = interleave_iterator<iter_tuple, sentinel_tuple>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    template<class I, class... Is>
    constexpr interleave_iterable(I&& iterable, Is&&... iterables) :
        _iterables{ std::forward<I>(iterable), std::forward<Is>(iterables)... } {
    }

    template<class T = conjunction<sized<Iterables>...>>
    LZ_NODISCARD constexpr enable_if<T::value, std::size_t> size() const {
        return size(index_sequence_for_this{});
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { begin_tuple(_iterables) };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return { begin_tuple(std::move(_iterables)) };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, iterator> end() const& {
        return { smallest_end_tuple(_iterables, index_sequence_for_this{}) };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, iterator> end() && {
        return { smallest_end_tuple(std::move(_iterables), index_sequence_for_this{}) };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, sentinel_tuple> end() const& {
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
