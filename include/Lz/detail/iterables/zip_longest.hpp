#pragma once

#ifndef LZ_ZIP_LONGEST_ITERABLE_HPP
#define LZ_ZIP_LONGEST_ITERABLE_HPP

#include <Lz/detail/iterators/zip_longest.hpp>
#include <Lz/detail/ref_or_view.hpp>
#include <Lz/detail/tuple_helpers.hpp>

namespace lz {
namespace detail {
template<class... Iterables>
class zip_longest_iterable {
    std::tuple<ref_or_view<Iterables>...> _iterables;

    static constexpr bool is_bidirectional = is_bidi_tag<iter_tuple_iter_cat_t<std::tuple<iter_t<Iterables>...>>>::value;

public:
    using iterator =
        zip_longest_iterator<is_bidirectional, std::tuple<iter_t<Iterables>...>, std::tuple<sentinel_t<Iterables>...>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    using is = make_index_sequence<sizeof...(Iterables)>;

    template<std::size_t... I>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 std::size_t size(index_sequence_helper<I...>) const {
        const std::size_t sizes[] = { static_cast<std::size_t>(lz::size(std::get<I>(_iterables)))... };
        return *std::max_element(std::begin(sizes), std::end(sizes));
    }

private:
    static_assert(sizeof...(Iterables) > 0, "Cannot create zip longest object with 0 iterators");

public:
    constexpr zip_longest_iterable() = default;

    template<class... I>
    LZ_CONSTEXPR_CXX_14 zip_longest_iterable(I&&... iterables) : _iterables{ std::forward<I>(iterables)... } {
    }

    template<class T = conjunction<sized<Iterables>...>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<T::value, std::size_t> size() const {
        return size(make_index_sequence<sizeof...(Iterables)>{});
    }

    template<bool bidi = is_bidirectional>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<bidi, iterator> begin() const& {
        return { begin_tuple(_iterables), begin_tuple(_iterables), end_tuple(_iterables) };
    }

    template<bool bidi = is_bidirectional>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!bidi, iterator> begin() const& {
        return { begin_tuple(_iterables), end_tuple(_iterables) };
    }

    template<bool bidi = is_bidirectional>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!bidi, iterator> begin() && {
        return { begin_tuple(std::move(_iterables)), end_tuple(std::move(_iterables)) };
    }

    template<bool bidi = is_bidirectional>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<bidi, iterator> end() const {
        return { begin_tuple(_iterables), end_tuple(_iterables), end_tuple(_iterables) };
    }

    template<bool bidi = is_bidirectional>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!bidi, default_sentinel> end() const noexcept {
        return {};
    }

    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend zip_longest_iterable<Iterable, Iterables...>
    operator|(Iterable&& iterable, zip_longest_iterable<Iterables...>&& zipper) {
        return iterable_tuple_cat(std::forward<Iterable>(iterable), std::move(zipper._iterables));
    }

    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend zip_longest_iterable<Iterable, Iterables...>
    operator|(Iterable&& iterable, const zip_longest_iterable<Iterables...>& zipper) {
        return iterable_tuple_cat(std::forward<Iterable>(iterable), zipper._iterables);
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_ZIP_LONGEST_ITERABLE_HPP
