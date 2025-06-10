#pragma once

#ifndef LZ_FLATTEN_ITERABLE_HPP
#define LZ_FLATTEN_ITERABLE_HPP

#include <Lz/detail/iterators/flatten.hpp>
#include <Lz/detail/ref_or_view.hpp>
#include <numeric>

namespace lz {
namespace detail {
template<bool SizedIterable>
struct all_sized_helper {
    template<class>
    using type = std::integral_constant<bool, SizedIterable>;
};

template<>
struct all_sized_helper<true> {
    template<class Iterable>
    using inner = val_iterable_t<Iterable>;

    template<class Iterable>
    using is_inner_sized = sized<inner<Iterable>>;

    template<class Iterable>
    using type = conditional<
        // If the inner is another iterable
        is_iterable<inner<Iterable>>::value,
        // Check whether the inner is also sized
        typename all_sized_helper<is_inner_sized<Iterable>::value>::template type<inner<Iterable>>,
        // Otherwise return true
        std::true_type>;
};

template<class Iterable>
using all_sized = typename all_sized_helper<sized<Iterable>::value && is_iterable<Iterable>::value>::template type<Iterable>;

#ifdef LZ_HAS_CXX_17

template<std::size_t I, class Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 std::size_t size_all(Iterable&& iterable) {
    if constexpr (I == 1) {
        return static_cast<std::size_t>(lz::size(iterable));
    }
    else {
        using lz::detail::accumulate;
        using std::accumulate;
        return accumulate(std::begin(iterable), std::end(iterable), std::size_t{ 0 },
                          [](std::size_t sum, ref_iterable_t<Iterable> val) { return sum + size_all<I - 1>(val); });
    }
}

#else

template<std::size_t I, class Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<I == 1, std::size_t> size_all(Iterable&& iterable) {
    return static_cast<std::size_t>(lz::size(iterable));
}

template<std::size_t I, class Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<(I > 1), std::size_t> size_all(Iterable&& iterable) {
    using lz::detail::accumulate;
    using std::accumulate;
    return accumulate(std::begin(iterable), std::end(iterable), std::size_t{ 0 },
                      [](std::size_t sum, ref_iterable_t<Iterable> val) { return sum + size_all<I - 1>(val); });
}

#endif

template<class Iterable, std::size_t Dims>
class flatten_iterable : public lazy_view {
    using inner_iter = iter_t<Iterable>;
    using inner_sentinel = sentinel_t<Iterable>;

    ref_or_view<Iterable> _iterable;

public:
    using iterator = flatten_iterator<inner_iter, inner_sentinel, Dims>;
    using const_iterator = iterator;
    using value_type = typename flatten_iterator<inner_iter, inner_sentinel, 0>::value_type;

#ifdef LZ_HAS_CONCEPTS

    constexpr flatten_iterable()
        requires std::default_initializable<Iterable>
    = default;

#else

    template<class I = decltype(_iterable), class = enable_if<std::is_default_constructible<I>::value>>
    constexpr flatten_iterable() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    template<class I>
    LZ_CONSTEXPR_CXX_14 flatten_iterable(I&& iterable) : _iterable{ std::forward<I>(iterable) } {
    }

    template<class T = all_sized<Iterable>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<T::value, std::size_t> size() const {
        return size_all<Dims + 1>(_iterable);
    }

    LZ_CONSTEXPR_CXX_14 iterator begin() const {
        return { std::begin(_iterable), std::begin(_iterable), std::end(_iterable) };
    }

    template<class I = typename iterator::iterator_category>
    LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, iterator> begin() && {
        return { std::begin(_iterable), std::begin(_iterable), detail::end(std::move(_iterable)) };
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (is_bidi_tag<typename iterator::iterator_category>::value) {
            return iterator{ std::end(_iterable), std::begin(_iterable), std::end(_iterable) };
        }
        else {
            return default_sentinel{};
        }
    }

#else

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD constexpr enable_if<is_bidi_tag<I>::value, iterator> end() const {
        return { std::end(_iterable), std::begin(_iterable), std::end(_iterable) };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD constexpr enable_if<!is_bidi_tag<I>::value, default_sentinel> end() const noexcept {
        return {};
    }

#endif
};
} // namespace detail
} // namespace lz

#endif // LZ_FLATTEN_ITERABLE_HPP
