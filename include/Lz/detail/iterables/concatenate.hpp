#pragma once

#ifndef LZ_CONCATENATE_ITERABLE_HPP
#define LZ_CONCATENATE_ITERABLE_HPP

#include <Lz/detail/iterators/concatenate.hpp>
#include <Lz/detail/maybe_owned.hpp>
#include <Lz/detail/traits/is_sentinel.hpp>
#include <Lz/detail/tuple_helpers.hpp>

namespace lz {
namespace detail {

template<class... Iterables>
class concatenate_iterable : public lazy_view {
    using iterables = maybe_homogeneous_t<maybe_owned<Iterables>...>;
    iterables _iterables{};

    template<size_t... I>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 size_t size(index_sequence<I...>) const {
        using std::get;
        const size_t sizes[] = { static_cast<size_t>(lz::size(get<I>(_iterables)))... };
        return std::accumulate(detail::begin(sizes), detail::end(sizes), size_t{ 0 });
    }

    template<class Iterable2, size_t... Is>
    static concatenate_iterable<remove_ref_t<Iterable2>, Iterables...>
    concat_iterables(Iterable2&& iterable2, concatenate_iterable<Iterables...>&& cat, index_sequence<Is...>) {
        using std::get;
        return { std::forward<Iterable2>(iterable2), std::move(get<Is>(cat._iterables))... };
    }

    template<class Iterable2, size_t... Is>
    static concatenate_iterable<remove_ref_t<Iterable2>, Iterables...>
    concat_iterables(Iterable2&& iterable2, const concatenate_iterable<Iterables...>& cat, index_sequence<Is...>) {
        using std::get;
        return { std::forward<Iterable2>(iterable2), get<Is>(cat._iterables)... };
    }

    using is = make_index_sequence<sizeof...(Iterables)>;

public:
    using iterator = concatenate_iterator<iterables, maybe_homogeneous_t<iter_t<Iterables>...>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

private:
    static constexpr bool return_sentinel =
        !is_bidi_tag<typename iterator::iterator_category>::value || disjunction<has_sentinel<Iterables>...>::value;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr concatenate_iterable()
        requires(std::default_initializable<maybe_owned<Iterables>> && ...)
    = default;

#else

    template<class I = decltype(_iterables), class = enable_if_t<std::is_default_constructible<I>::value>>
    constexpr concatenate_iterable() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    template<class... Is>
    LZ_CONSTEXPR_CXX_14 concatenate_iterable(Is&&... its) : _iterables{ std::forward<Is>(its)... } {
    }

#ifdef LZ_HAS_CONCEPTS

    [[nodiscard]] constexpr size_t size() const
        requires(sized<Iterables> && ...)
    {
        return size(is{});
    }

#else

    template<class T = conjunction<is_sized<Iterables>...>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<T::value, size_t> size() const {
        return size(is{});
    }

#endif

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const {
        return { _iterables, begin_maybe_homo(_iterables) };
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (!return_sentinel) {
            return iterator{ _iterables, end_maybe_homo(_iterables) };
        }
        else {
            return lz::default_sentinel;
        }
    }

#else

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<!R, iterator> end() const {
        return { _iterables, end_maybe_homo(_iterables) };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<R, default_sentinel_t> end() const {
        return {};
    }

    // clang-format on
#endif // LZ_HAS_CXX_17

    template<class Iterable>
    friend concatenate_iterable<remove_ref_t<Iterable>, Iterables...>
    operator|(Iterable&& iterable, concatenate_iterable<Iterables...>&& concatenate) {
        return concat_iterables(std::forward<Iterable>(iterable), std::move(concatenate), is{});
    }

    template<class Iterable>
    friend concatenate_iterable<remove_ref_t<Iterable>, Iterables...>
    operator|(Iterable&& iterable, const concatenate_iterable<Iterables...>& concatenate) {
        return concat_iterables(std::forward<Iterable>(iterable), concatenate, is{});
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_CONCATENATE_HPP
