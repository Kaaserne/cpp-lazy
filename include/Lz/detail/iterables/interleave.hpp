#pragma once

#ifndef LZ_INTERLEAVED_ITERABLE_HPP
#define LZ_INTERLEAVED_ITERABLE_HPP

#include <Lz/detail/iterators/interleave.hpp>
#include <Lz/detail/maybe_owned.hpp>

namespace lz {
namespace detail {

template<class... Iterables>
class interleave_iterable : public lazy_view {
    maybe_homogeneous_t<maybe_owned<Iterables>...> _iterables{};

    using iterators = maybe_homogeneous_t<iter_t<Iterables>...>;
    using sentinels = maybe_homogeneous_t<sentinel_t<Iterables>...>;
    using this_sentinel = sentinel_with<sentinels>;

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 size_t size(index_sequence<I...>) const {
        using std::get;
        return std::min({ static_cast<size_t>(get<I>(_iterables).size())... }) * sizeof...(Iterables);
    }

    using is = make_index_sequence<sizeof...(Iterables)>;

    template<class Iterable2, size_t... Is>
    static interleave_iterable<remove_ref_t<Iterable2>, Iterables...>
    concat_iterables(Iterable2&& iterable2, interleave_iterable<Iterables...>&& interleaved, index_sequence<Is...>) {
        using std::get;
        return { std::forward<Iterable2>(iterable2), std::move(get<Is>(interleaved._iterables))... };
    }

    template<class Iterable2, size_t... Is>
    static interleave_iterable<remove_ref_t<Iterable2>, Iterables...>
    concat_iterables(Iterable2&& iterable2, const interleave_iterable<Iterables...>& interleaved, index_sequence<Is...>) {
        using std::get;
        return { std::forward<Iterable2>(iterable2), get<Is>(interleaved._iterables)... };
    }

public:
    using iterator = interleave_iterator<iterators, sentinels>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

private:
    static constexpr bool return_sentinel =
        !is_bidi_tag<typename iterator::iterator_category>::value || disjunction<has_sentinel<Iterables>...>::value;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr interleave_iterable()
        requires(std::default_initializable<maybe_owned<Iterables>> && ...)
    = default;

#else

    template<class I = decltype(_iterables), class = enable_if_t<std::is_default_constructible<I>::value>>
    constexpr interleave_iterable() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    template<class... Is>
    constexpr interleave_iterable(Is&&... iterables) : _iterables{ std::forward<Is>(iterables)... } {
    }

#ifdef LZ_HAS_CONCEPTS

    [[nodiscard]] constexpr size_t size() const
        requires(sized<Iterables> && ...)
    {
        return size(is{});
    }

#else

    template<class T = conjunction<is_sized<Iterables>...>>
    LZ_NODISCARD constexpr enable_if_t<T::value, size_t> size() const {
        return size(is{});
    }

#endif

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const {
        return { begin_maybe_homo(_iterables) };
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (!return_sentinel) {
            return iterator{ smallest_end_maybe_homo(_iterables, is{}) };
        }
        else {
            return this_sentinel{ end_maybe_homo(_iterables) };
        }
    }

#else

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<!R, iterator> end() const {
        return { smallest_end_maybe_homo(_iterables, is{}) };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<R, this_sentinel> end() const {
        return this_sentinel{ end_maybe_homo(_iterables) };
    }

#endif

    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend interleave_iterable<remove_ref_t<Iterable>, Iterables...>
    operator|(Iterable&& iterable,
              interleave_iterable<Iterables...> interleaved) { // TODO remove const& for std::move copy for all operator|
        return concat_iterables(std::forward<Iterable>(iterable), std::move(interleaved), is{});
    }
};

} // namespace detail
} // namespace lz
#endif // LZ_INTERLEAVED_ITERABLE_HPP
