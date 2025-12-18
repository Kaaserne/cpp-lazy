#pragma once

#ifndef LZ_ZIP_LONGEST_ITERABLE_HPP
#define LZ_ZIP_LONGEST_ITERABLE_HPP

#include <Lz/detail/iterators/zip_longest.hpp>
#include <Lz/detail/maybe_owned.hpp>
#include <Lz/detail/traits/is_sentinel.hpp>
#include <Lz/detail/tuple_helpers.hpp>
#include <Lz/traits/lazy_view.hpp>

namespace lz {
namespace detail {
template<class... Iterables>
class zip_longest_iterable : public lazy_view {
    std::tuple<maybe_owned<Iterables>...> _iterables{};

    using iterators = std::tuple<iter_t<Iterables>...>;
    using sentinels = std::tuple<sentinel_t<Iterables>...>;

    static constexpr bool bidi = is_bidi_tag<iter_tuple_iter_cat_t<iterators>>::value;

    static constexpr bool return_sentinel = !bidi || disjunction<has_sentinel<Iterables>...>::value;

    using iter_category = iter_tuple_iter_cat_t<iterators>;

public:
    using iterator = zip_longest_iterator<iter_category, maybe_owned<Iterables>...>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    using is = make_index_sequence<sizeof...(Iterables)>;

#ifdef LZ_HAS_CONCEPTS

    constexpr zip_longest_iterable()
        requires(std::default_initializable<maybe_owned<Iterables>> && ...)
    = default;

#else

    template<class I = decltype(_iterables), class = enable_if_t<std::is_default_constructible<I>::value>>
    constexpr zip_longest_iterable() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    template<size_t... I>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 size_t size(index_sequence<I...>) const {
        return max_variadic(static_cast<size_t>(lz::size(std::get<I>(_iterables)))...);
    }

    template<class Iterable2, size_t... Is>
    static zip_longest_iterable<remove_ref_t<Iterable2>, Iterables...>
    concat_iterables(Iterable2&& iterable2, zip_longest_iterable<Iterables...> zipper, index_sequence<Is...>) {
        return { std::forward<Iterable2>(iterable2), std::move(std::get<Is>(zipper._iterables))... };
    }

public:
    template<class... I>
    LZ_CONSTEXPR_CXX_14 zip_longest_iterable(I&&... iterables) : _iterables{ std::forward<I>(iterables)... } {
    }

#ifdef LZ_HAS_CONCEPTS

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 size_t size() const
        requires(sized<Iterables> && ...)
    {
        return size(is{});
    }

#else

    template<bool S = conjunction<is_sized<Iterables>...>::value>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<S, size_t> size() const {
        return size(is{});
    }

#endif

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr iterator begin() const {
        if constexpr (!is_bidi_tag_v<iter_category>) {
            return { begin_tuple(_iterables), end_tuple(_iterables) };
        }
        else {
            using diff = typename iterator::difference_type;
            return iterator{ _iterables, begin_tuple(_iterables), tuple_of<diff>(is{}) };
        }
    }

    [[nodiscard]] constexpr auto end() const {
        if constexpr (return_sentinel) {
            return lz::default_sentinel;
        }
        else {
            using diff = typename iterator::difference_type;
            return iterator{ _iterables, end_tuple(_iterables), iterables_size_tuple<diff>(_iterables, is{}) };
        }
    }

#else

    template<class C = iter_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<!is_bidi_tag<C>::value, iterator> begin() const {
        return { begin_tuple(_iterables), end_tuple(_iterables) };
    }

    template<class C = iter_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<is_bidi_tag<C>::value, iterator> begin() const {
        using diff = typename iterator::difference_type;
        return { _iterables, begin_tuple(_iterables), tuple_of<diff>(is{}) };
    }

    template<class C = iter_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<!return_sentinel && is_bidi_tag<C>::value, iterator> end() const {
        using diff = typename iterator::difference_type;
        return iterator{ _iterables, end_tuple(_iterables), iterables_size_tuple<diff>(_iterables, is{}) };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<R, default_sentinel_t> end() const noexcept {
        return {};
    }

#endif

    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend zip_longest_iterable<remove_ref_t<Iterable>, Iterables...>
    operator|(Iterable&& iterable, zip_longest_iterable<Iterables...> zipper) {
        return concat_iterables(std::forward<Iterable>(iterable), std::move(zipper), is{});
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_ZIP_LONGEST_ITERABLE_HPP
