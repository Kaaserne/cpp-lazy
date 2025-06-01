#pragma once

#ifndef LZ_ZIP_LONGEST_ITERABLE_HPP
#define LZ_ZIP_LONGEST_ITERABLE_HPP

#include <Lz/detail/iterators/zip_longest.hpp>
#include <Lz/detail/ref_or_view.hpp>
#include <Lz/detail/tuple_helpers.hpp>

namespace lz {
namespace detail {
template<class... Iterables>
class zip_longest_iterable : public lazy_view {
    std::tuple<ref_or_view<Iterables>...> _iterables;

    static constexpr bool bidi = is_bidi_tag<iter_tuple_iter_cat_t<std::tuple<iter_t<Iterables>...>>>::value;

public:
    using iterator = zip_longest_iterator<bidi, std::tuple<iter_t<Iterables>...>, std::tuple<sentinel_t<Iterables>...>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    using is = make_index_sequence<sizeof...(Iterables)>;

    template<std::size_t... I>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 std::size_t size(index_sequence<I...>) const {
        return std::max({ static_cast<std::size_t>(lz::size(std::get<I>(_iterables)))... });
    }

    template<class Iterable2, std::size_t... Is>
    static zip_longest_iterable<remove_ref<Iterable2>, Iterables...>
    concat_iterables(Iterable2&& iterable2, zip_longest_iterable<Iterables...>&& zipper, index_sequence<Is...>) {
        return { std::forward<Iterable2>(iterable2), std::get<Is>(std::move(zipper._iterables))... };
    }

    template<class Iterable2, std::size_t... Is>
    static zip_longest_iterable<remove_ref<Iterable2>, Iterables...>
    concat_iterables(Iterable2&& iterable2, const zip_longest_iterable<Iterables...>& zipper, index_sequence<Is...>) {
        return { std::forward<Iterable2>(iterable2), std::get<Is>(zipper._iterables)... };
    }

public:
    template<class... I>
    LZ_CONSTEXPR_CXX_14 zip_longest_iterable(I&&... iterables) : _iterables{ std::forward<I>(iterables)... } {
    }

    template<bool AllSized = conjunction<sized<Iterables>...>::value>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<AllSized, std::size_t> size() const {
        return size(is{});
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr iterator begin() const& {
        if constexpr (bidi) {
            using diff = typename iterator::difference_type;
            return { begin_tuple(_iterables), end_tuple(_iterables), tuple_of_n<diff>(is{}) };
        }
        else {
            return { begin_tuple(_iterables), end_tuple(_iterables) };
        }
    }

#else

    template<bool IsBidi = bidi>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<IsBidi, iterator> begin() const& {
        using diff = typename iterator::difference_type;
        return { begin_tuple(_iterables), end_tuple(_iterables), tuple_of_n<diff>(is{}) };
    }

    template<bool IsBidi = bidi>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!IsBidi, iterator> begin() const& {
        return { begin_tuple(_iterables), end_tuple(_iterables) };
    }

#endif

    template<bool IsBidi = bidi>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!IsBidi, iterator> begin() && {
        return { begin_tuple(std::move(_iterables)), end_tuple(std::move(_iterables)) };
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (bidi) {
            using diff = typename iterator::difference_type;
            return iterator{ end_tuple(_iterables), end_tuple(_iterables), iterable_tuple_eager_size_as<diff>(_iterables, is{}) };
        }
        else {
            return default_sentinel{};
        }
    }

#else

    template<bool IsBidi = bidi>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<IsBidi, iterator> end() const {
        using diff = typename iterator::difference_type;
        return { end_tuple(_iterables), end_tuple(_iterables), iterable_tuple_eager_size_as<diff>(_iterables, is{}) };
    }

    template<bool IsBidi = bidi>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!IsBidi, default_sentinel> end() const noexcept {
        return {};
    }

#endif

    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend zip_longest_iterable<remove_ref<Iterable>, Iterables...>
    operator|(Iterable&& iterable, zip_longest_iterable<Iterables...>&& zipper) {
        return concat_iterables(std::forward<Iterable>(iterable), std::move(zipper._iterables), is{});
    }

    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend zip_longest_iterable<remove_ref<Iterable>, Iterables...>
    operator|(Iterable&& iterable, const zip_longest_iterable<Iterables...>& zipper) {
        return concat_iterables(std::forward<Iterable>(iterable), zipper._iterables, is{});
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_ZIP_LONGEST_ITERABLE_HPP
