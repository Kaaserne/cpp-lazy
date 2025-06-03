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
    maybe_homogeneous<ref_or_view<Iterables>...> _iterables;

    template<std::size_t... I>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 std::size_t size(index_sequence<I...>) const {
        const std::size_t sizes[] = { static_cast<std::size_t>(lz::size(std::get<I>(_iterables)))... };
        return std::accumulate(std::begin(sizes), std::end(sizes), std::size_t{ 0 });
    }

    template<class Iterable2, std::size_t... Is>
    static concatenate_iterable<remove_ref<Iterable2>, Iterables...>
    concat_iterables(Iterable2&& iterable2, concatenate_iterable<Iterables...>&& cat, index_sequence<Is...>) {
        return { std::forward<Iterable2>(iterable2), std::move(std::get<Is>(cat._iterables))... };
    }

    template<class Iterable2, std::size_t... Is>
    static concatenate_iterable<remove_ref<Iterable2>, Iterables...>
    concat_iterables(Iterable2&& iterable2, const concatenate_iterable<Iterables...>& cat, index_sequence<Is...>) {
        return { std::forward<Iterable2>(iterable2), std::get<Is>(cat._iterables)... };
    }

    using is = make_index_sequence<sizeof...(Iterables)>;

public:
    using iterator = concatenate_iterator<maybe_homogeneous<iter_t<Iterables>...>, maybe_homogeneous<sentinel_t<Iterables>...>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    template<class... Is>
    LZ_CONSTEXPR_CXX_14 concatenate_iterable(Is&&... iterables) : _iterables{ std::forward<Is>(iterables)... } {
    }

    template<class T = conjunction<sized<Iterables>...>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<T::value, std::size_t> size() const {
        return size(make_index_sequence<sizeof...(Iterables)>{});
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { begin_maybe_homo(_iterables), begin_maybe_homo(_iterables), end_maybe_homo(_iterables) };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, iterator> begin() && {
        return { begin_maybe_homo(std::move(_iterables)), begin_maybe_homo(std::move(_iterables)),
                 end_maybe_homo(std::move(_iterables)) };
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (is_bidi_tag<typename iterator::iterator_category>::value) {
            return iterator{ end_maybe_homo(_iterables), begin_maybe_homo(_iterables), end_maybe_homo(_iterables) };
        }
        else {
            return default_sentinel{};
        }
    }

#else

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, iterator> end() const {
        return { end_maybe_homo(_iterables), begin_maybe_homo(_iterables), end_maybe_homo(_iterables) };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, default_sentinel> end() const {
        return {};
    }

#endif // LZ_HAS_CXX_17

    template<class Iterable>
    friend concatenate_iterable<remove_ref<Iterable>, Iterables...>
    operator|(Iterable&& iterable, concatenate_iterable<Iterables...>&& concatenate) {
        return concat_iterables(std::forward<Iterable>(iterable), std::move(concatenate), is{});
    }

    template<class Iterable>
    friend concatenate_iterable<remove_ref<Iterable>, Iterables...>
    operator|(Iterable&& iterable, const concatenate_iterable<Iterables...>& concatenate) {
        return concat_iterables(std::forward<Iterable>(iterable), concatenate, is{});
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_CONCATENATE_HPP
