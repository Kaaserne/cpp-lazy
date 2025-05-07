#pragma once

#include "Lz/detail/procs.hpp"

#include <algorithm>
#ifndef LZ_ZIP_ITERABLE_HPP
#define LZ_ZIP_ITERABLE_HPP

#include <Lz/detail/iterators/zip.hpp>
#include <Lz/detail/ref_or_view.hpp>
#include <Lz/detail/tuple_helpers.hpp>

namespace lz {
namespace detail {
template<class... Iterables>
class zip_iterable : public lazy_view {
    using iter_tuple = std::tuple<iter_t<Iterables>...>;
    using sentinel_tuple = std::tuple<sentinel_t<Iterables>...>;

    std::tuple<ref_or_view<Iterables>...> _iterables;

    template<std::size_t... I>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 std::size_t size(index_sequence<I...>) const {
        return std::min({ static_cast<std::size_t>(lz::eager_size(std::get<I>(_iterables)))... });
    }

public:
    using iterator = zip_iterator<std::tuple<iter_t<Iterables>...>, sentinel_tuple>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

private:
    template<class Iterable>
    LZ_CONSTEXPR_CXX_14 iter_t<Iterable>
    increment_or_decrement_iterator(Iterable&& iterable, std::size_t this_size, std::size_t min) const {
        using diff_type = typename iterator::difference_type;
        // Check whether we can better use decrement or increment. For random access iterators, we can
        // use either, but for bidirectional iterators, we need to be "careful" about the direction.
        // We want to use the iterator that requires the least amount of increments/decrements.
        const auto to_decrement = static_cast<diff_type>(this_size - min);
        const auto to_increment = static_cast<diff_type>(min);
        if (to_increment > to_decrement) {
            return std::prev(detail::end(std::forward<Iterable>(iterable)), to_decrement);
        }
        return std::next(detail::begin(std::forward<Iterable>(iterable)), to_increment);
    }

    template<std::size_t... Is, class Iterable>
    LZ_CONSTEXPR_CXX_14 iter_tuple make_end_tuple(Iterable&& iterable_tuple, index_sequence<Is...>) const {
        const std::size_t sizes[] = { static_cast<std::size_t>(lz::eager_size(std::get<Is>(iterable_tuple)))... };
        const auto min = std::min({ sizes[Is]... });

        return std::make_tuple(
            increment_or_decrement_iterator(std::get<Is>(std::forward<Iterable>(iterable_tuple)), sizes[Is], min)...);
    }

    using seq = make_index_sequence<sizeof...(Iterables)>;

public:
    constexpr zip_iterable() = default;

    template<class... Is>
    LZ_CONSTEXPR_CXX_14 zip_iterable(Is&&... iterables) : _iterables{ std::forward<Is>(iterables)... } {
    }

    template<class T = conjunction<sized<Iterables>...>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<T::value, std::size_t> size() const {
        return size(make_index_sequence<sizeof...(Iterables)>{});
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { begin_tuple(_iterables) };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return { begin_tuple(std::move(_iterables)) };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, iterator> end() const& {
        return { make_end_tuple(_iterables, seq{}) };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, iterator> end() && {
        return { make_end_tuple(std::move(_iterables), seq{}) };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, typename iterator::sentinel> end() const& {
        return { end_tuple(_iterables) };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, typename iterator::sentinel> end() && {
        return { end_tuple(std::move(_iterables)) };
    }

    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend zip_iterable<remove_ref<Iterable>, Iterables...>
    operator|(Iterable&& iterable, zip_iterable<Iterables...>&& zipper) {
        return iterable_tuple_cat(std::forward<Iterable>(iterable), std::move(zipper._iterables));
    }

    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend zip_iterable<remove_ref<Iterable>, Iterables...>
    operator|(Iterable&& iterable, const zip_iterable<Iterables...>& zipper) {
        return iterable_tuple_cat(std::forward<Iterable>(iterable), zipper._iterables);
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_ZIP_ITERABLE_HPP
