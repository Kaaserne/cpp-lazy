#pragma once

#include "Lz/detail/compiler_checks.hpp"
#include "Lz/detail/procs.hpp"
#include "Lz/detail/traits.hpp"

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
        return std::min({ static_cast<std::size_t>(lz::size(std::get<I>(_iterables)))... });
    }

public:
    using iterator = zip_iterator<std::tuple<iter_t<Iterables>...>, sentinel_tuple>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

private:
    using seq = make_index_sequence<sizeof...(Iterables)>;

public:
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
        return { smallest_end_tuple(_iterables, seq{}) };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, iterator> end() && {
        return { smallest_end_tuple(std::move(_iterables), seq{}) };
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
