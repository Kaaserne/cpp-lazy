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
    using iter_tuple = maybe_homogeneous<iter_t<Iterables>...>;
    using sentinel_tuple = maybe_homogeneous<sentinel_t<Iterables>...>;

    maybe_homogeneous<ref_or_view<Iterables>...> _iterables;

    template<std::size_t... I>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 std::size_t size(index_sequence<I...>) const {
        return std::min({ static_cast<std::size_t>(lz::size(std::get<I>(_iterables)))... });
    }

    template<class Iterable2, std::size_t... Is>
    static zip_iterable<remove_ref<Iterable2>, Iterables...>
    concat_iterables(Iterable2&& iterable2, zip_iterable<Iterables...>&& zipper, index_sequence<Is...>) {
        return { std::forward<Iterable2>(iterable2), std::move(std::get<Is>(zipper._iterables))... };
    }

    template<class Iterable2, std::size_t... Is>
    static zip_iterable<remove_ref<Iterable2>, Iterables...>
    concat_iterables(Iterable2&& iterable2, const zip_iterable<Iterables...>& zipper, index_sequence<Is...>) {
        return { std::forward<Iterable2>(iterable2), std::get<Is>(zipper._iterables)... };
    }

public:
    using iterator = zip_iterator<iter_tuple, sentinel_tuple>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

private:
    using seq = make_index_sequence<sizeof...(Iterables)>;

    static constexpr bool return_sentinel = !is_bidi_tag<typename iterator::iterator_category>::value ||
                                            disjunction<is_sentinel<iter_t<Iterables>, sentinel_t<Iterables>>...>::value;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr zip_iterable()
        requires(std::default_initializable<Iterables> && ...)
    = default;

#else

    template<class I = decltype(_iterables), class = enable_if<std::is_default_constructible<I>::value>>
    constexpr zip_iterable() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    template<class... Is>
    LZ_CONSTEXPR_CXX_14 zip_iterable(Is&&... iterables) : _iterables{ std::forward<Is>(iterables)... } {
    }

    template<class T = conjunction<sized<Iterables>...>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<T::value, std::size_t> size() const {
        return size(seq{});
    }

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
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!R, iterator> end() const& {
        return { smallest_end_maybe_homo(_iterables, seq{}) };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!R, iterator> end() && {
        return { smallest_end_maybe_homo(std::move(_iterables), seq{}) };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<R, typename iterator::sentinel> end() const& {
        return { end_maybe_homo(_iterables) };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<R, typename iterator::sentinel> end() && {
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
