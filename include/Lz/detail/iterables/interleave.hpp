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

    template<class Iterable2, std::size_t... Is>
    static interleave_iterable<remove_ref<Iterable2>, Iterables...>
    concat_iterables(Iterable2&& iterable2, interleave_iterable<Iterables...>&& interleaved, index_sequence<Is...>) {
        return { std::forward<Iterable2>(iterable2), std::move(std::get<Is>(interleaved._iterables))... };
    }

    template<class Iterable2, std::size_t... Is>
    static interleave_iterable<remove_ref<Iterable2>, Iterables...>
    concat_iterables(Iterable2&& iterable2, const interleave_iterable<Iterables...>& interleaved, index_sequence<Is...>) {
        return { std::forward<Iterable2>(iterable2), std::get<Is>(interleaved._iterables)... };
    }

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

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const& {
        if constexpr (is_bidi_tag<typename iterator::iterator_category>::value) {
            return iterator{ smallest_end_tuple(_iterables, index_sequence_for_this{}) };
        }
        else {
            return end_tuple(_iterables);
        }
    }

#else

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, iterator> end() const& {
        return { smallest_end_tuple(_iterables, index_sequence_for_this{}) };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, sentinel_tuple> end() const& {
        return end_tuple(_iterables);
    }

#endif

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, iterator> end() && {
        return { smallest_end_tuple(std::move(_iterables), index_sequence_for_this{}) };
    }

    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend interleave_iterable<remove_ref<Iterable>, Iterables...>
    operator|(Iterable&& iterable, interleave_iterable<Iterables...>&& interleaved) {
        return concat_iterables(std::forward<Iterable>(iterable), std::move(interleaved), index_sequence_for_this{});
    }

    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend interleave_iterable<remove_ref<Iterable>, Iterables...>
    operator|(Iterable&& iterable, const interleave_iterable<Iterables...>& interleaved) {
        return concat_iterables(std::forward<Iterable>(iterable), interleaved, index_sequence_for_this{});
    }
};

} // namespace detail
} // namespace lz
#endif // LZ_INTERLEAVED_ITERABLE_HPP
