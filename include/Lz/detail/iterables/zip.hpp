#pragma once

#ifndef LZ_ZIP_ITERABLE_HPP
#define LZ_ZIP_ITERABLE_HPP

#include <Lz/detail/iterators/zip.hpp>
#include <Lz/detail/ref_or_view.hpp>
#include <Lz/detail/tuple_helpers.hpp>

namespace lz {
namespace detail {
template<class... Iterables>
class zip_iterable : public lazy_view {
    using sentinel_tuple = std::tuple<sentinel_t<Iterables>...>;
    std::tuple<ref_or_view<Iterables>...> _iterables;

    template<std::size_t... I>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 std::size_t size(index_sequence_helper<I...>) const {
        return std::min({ static_cast<std::size_t>(lz::eager_size(std::get<I>(_iterables)))... });
    }

    template<class I, std::size_t... Is>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 sentinel_tuple fake_end_tuple(I&& iterables, index_sequence_helper<Is...> is) const {
        return { std::next(std::begin(std::get<Is>(std::forward<I>(iterables))), static_cast<std::ptrdiff_t>(size(is)))... };
    }

public:
    using iterator = zip_iterator<std::tuple<iter_t<Iterables>...>, sentinel_tuple>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr zip_iterable() = default;

    template<class... Is>
    LZ_CONSTEXPR_CXX_14 zip_iterable(Is&&... iterables) : _iterables{ std::forward<Is>(iterables)... } {
    }

    template<class T = conjunction<sized<Iterables>...>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 enable_if<T::value, std::size_t> size() const {
        return size(make_index_sequence<sizeof...(Iterables)>{});
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { begin_tuple(_iterables) };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return { begin_tuple(std::move(_iterables)) };
    }

    // TODO make everything like this: iterator::iterator_category
    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, iterator> end() const& {
        return { fake_end_tuple(_iterables, make_index_sequence<sizeof...(Iterables)>{}) };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, iterator> end() && {
        return { fake_end_tuple(std::move(_iterables), make_index_sequence<sizeof...(Iterables)>{}) };
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
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend zip_iterable<Iterable, Iterables...>
    operator|(Iterable&& iterable, zip_iterable<Iterables...>&& zipper) {
        return iterable_tuple_cat(std::forward<Iterable>(iterable), std::move(zipper._iterables));
    }

    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend zip_iterable<Iterable, Iterables...>
    operator|(Iterable&& iterable, const zip_iterable<Iterables...>& zipper) {
        return iterable_tuple_cat(std::forward<Iterable>(iterable), zipper._iterables);
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_ZIP_ITERABLE_HPP
