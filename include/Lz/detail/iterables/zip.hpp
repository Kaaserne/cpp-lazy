#pragma once

#ifndef LZ_ZIP_ITERABLE_HPP
#define LZ_ZIP_ITERABLE_HPP

#include <Lz/detail/iterators/zip.hpp>
#include <Lz/detail/maybe_owned.hpp>
#include <Lz/detail/tuple_helpers.hpp>

namespace lz {
namespace detail {
template<class... Iterables>
class zip_iterable : public lazy_view {
    using iter_tuple = maybe_homogeneous_t<iter_t<Iterables>...>;
    using sentinel_tuple = maybe_homogeneous_t<sentinel_t<Iterables>...>;

    maybe_homogeneous_t<maybe_owned<Iterables>...> _iterables;

    template<size_t... I>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 size_t size(index_sequence<I...>) const {
        using std::get;
        return std::min({ static_cast<size_t>(lz::size(get<I>(_iterables)))... });
    }

    template<class Iterable2, size_t... Is>
    static zip_iterable<remove_ref<Iterable2>, Iterables...>
    concat_iterables(Iterable2&& iterable2, zip_iterable<Iterables...>&& zipper, index_sequence<Is...>) {
        using std::get;
        return { std::forward<Iterable2>(iterable2), std::move(get<Is>(zipper._iterables))... };
    }

    template<class Iterable2, size_t... Is>
    static zip_iterable<remove_ref<Iterable2>, Iterables...>
    concat_iterables(Iterable2&& iterable2, const zip_iterable<Iterables...>& zipper, index_sequence<Is...>) {
        using std::get;
        return { std::forward<Iterable2>(iterable2), get<Is>(zipper._iterables)... };
    }

public:
    using iterator = zip_iterator<iter_tuple, sentinel_tuple>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

private:
    using seq = make_index_sequence<sizeof...(Iterables)>;

    static constexpr bool return_sentinel =
        !is_bidi_tag<typename iterator::iterator_category>::value || disjunction<has_sentinel<Iterables>...>::value;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr zip_iterable()
        requires(std::default_initializable<maybe_owned<Iterables>> && ...)
    = default;

#else

    template<class I = decltype(_iterables), class = enable_if_t<std::is_default_constructible<I>::value>>
    constexpr zip_iterable() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    template<class... Is>
    LZ_CONSTEXPR_CXX_14 zip_iterable(Is&&... iterables) : _iterables{ std::forward<Is>(iterables)... } {
    }

#ifdef LZ_HAS_CONCEPTS

    [[nodiscard]] constexpr size_t size() const
        requires(sized<Iterables> && ...)
    {
        return size(seq{});
    }

#else

    template<class T = conjunction<is_sized<Iterables>...>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<T::value, size_t> size() const {
        return size(seq{});
    }

#endif

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { begin_maybe_homo(_iterables) };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return { begin_maybe_homo(std::move(_iterables)) };
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const& {
        if constexpr (!return_sentinel) {
            return iterator{ smallest_end_maybe_homo(_iterables, seq{}) };
        }
        else {
            return typename iterator::sentinel{ end_maybe_homo(_iterables) };
        }
    }

    [[nodiscard]] constexpr auto end() && {
        if constexpr (!return_sentinel) {
            return iterator{ smallest_end_maybe_homo(std::move(_iterables), seq{}) };
        }
        else {
            return typename iterator::sentinel{ end_maybe_homo(std::move(_iterables)) };
        }
    }

#else

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<!R, iterator> end() const& {
        return { smallest_end_maybe_homo(_iterables, seq{}) };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<!R, iterator> end() && {
        return { smallest_end_maybe_homo(std::move(_iterables), seq{}) };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<R, typename iterator::sentinel> end() const& {
        return { end_maybe_homo(_iterables) };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<R, typename iterator::sentinel> end() && {
        return { end_maybe_homo(std::move(_iterables)) };
    }

#endif

    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend zip_iterable<remove_ref<Iterable>, Iterables...>
    operator|(Iterable&& iterable, zip_iterable<Iterables...>&& zipper) {
        return concat_iterables(std::forward<Iterable>(iterable), std::move(zipper), seq{});
    }

    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend zip_iterable<remove_ref<Iterable>, Iterables...>
    operator|(Iterable&& iterable, const zip_iterable<Iterables...>& zipper) {
        return concat_iterables(std::forward<Iterable>(iterable), zipper, seq{});
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_ZIP_ITERABLE_HPP
