#pragma once

#ifndef LZ_FLATTEN_ITERABLE_HPP
#define LZ_FLATTEN_ITERABLE_HPP

#include <Lz/detail/iterators/flatten.hpp>
#include <Lz/detail/maybe_owned.hpp>
#include <numeric>

namespace lz {
namespace detail {

#ifdef LZ_HAS_CXX_17

template<class T>
[[nodiscard]] constexpr bool all_sized_fn() {
    if constexpr (is_iterable_v<T>) {
        return is_sized_v<T> && all_sized_fn<val_iterable_t<T>>();
    }
    else {
        return true;
    }
}

template<class Iterable>
using is_all_sized = std::bool_constant<all_sized_fn<Iterable>()>;

#else

template<bool SizedIterable>
struct all_sized_impl {
    template<class>
    using type = std::integral_constant<bool, SizedIterable>;
};

template<>
struct all_sized_impl<true> {
    template<class Iterable>
    using inner = val_iterable_t<Iterable>;

    template<class Iterable>
    using is_inner_sized = is_sized<inner<Iterable>>;

    template<class Iterable>
    using type = conditional_t<
        // If the inner is another iterable
        is_iterable<inner<Iterable>>::value,
        // Check whether the inner is also sized
        typename all_sized_impl<is_inner_sized<Iterable>::value>::template type<inner<Iterable>>,
        // Otherwise return true
        std::true_type>;
};

template<class Iterable>
using is_all_sized = typename all_sized_impl<is_sized<Iterable>::value && is_iterable<Iterable>::value>::template type<Iterable>;

#endif

#ifdef LZ_HAS_CXX_17

template<size_t I, class Iterable>
[[nodiscard]] constexpr size_t size_all(Iterable&& iterable) {
    if constexpr (I == 1) {
        return static_cast<size_t>(lz::size(iterable));
    }
    else {
        using lz::detail::accumulate;
        using std::accumulate;
        return accumulate(iterable.begin(), iterable.end(), size_t{ 0 },
                          [](size_t sum, ref_iterable_t<Iterable> val) { return sum + size_all<I - 1>(val); });
    }
}

#else

template<size_t I, class Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<I == 1, size_t> size_all(Iterable&& iterable) {
    return static_cast<size_t>(lz::size(iterable));
}

template<size_t I, class Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<(I > 1), size_t> size_all(Iterable&& iterable) {
    using lz::detail::accumulate;
    using std::accumulate;
    return accumulate(iterable.begin(), iterable.end(), size_t{ 0 },
                      [](size_t sum, ref_iterable_t<Iterable> val) { return sum + size_all<I - 1>(val); });
}

#endif

template<class Iterable, size_t Dims>
class flatten_iterable : public lazy_view {
    using inner_iter = iter_t<Iterable>;
    using inner_sentinel = sentinel_t<Iterable>;

    maybe_owned<Iterable> _iterable;

public:
    using iterator = flatten_iterator<Iterable, Dims>;
    using const_iterator = iterator;
    using value_type = typename flatten_iterator<Iterable, 0>::value_type;

private:
    static constexpr bool return_sentinel =
        !is_bidi_tag<typename iterator::iterator_category>::value || is_sentinel<inner_iter, inner_sentinel>::value;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr flatten_iterable()
        requires(std::default_initializable<maybe_owned<Iterable>>)
    = default;

#else

    template<class I = decltype(_iterable), class = enable_if_t<std::is_default_constructible<I>::value>>
    constexpr flatten_iterable() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    template<class I>
    explicit LZ_CONSTEXPR_CXX_14 flatten_iterable(I&& iterable) : _iterable{ std::forward<I>(iterable) } {
    }

#ifdef LZ_HAS_CONCEPTS

    [[nodiscard]] constexpr size_t size() const
        requires(is_all_sized<Iterable>::value)
    {
        return size_all<Dims + 1>(_iterable);
    }

#else

    template<class T = is_all_sized<Iterable>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<T::value, size_t> size() const {
        return size_all<Dims + 1>(_iterable);
    }

#endif

    LZ_CONSTEXPR_CXX_14 iterator begin() const {
        return { _iterable, _iterable.begin() };
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (!return_sentinel) {
            return iterator{ _iterable, _iterable.end() };
        }
        else {
            return lz::default_sentinel;
        }
    }

#else

    template<bool R = return_sentinel>
    LZ_NODISCARD constexpr enable_if_t<!R, iterator> end() const {
        return { _iterable, _iterable.end() };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD constexpr enable_if_t<R, default_sentinel_t> end() const noexcept {
        return {};
    }

#endif
};
} // namespace detail
} // namespace lz

#endif // LZ_FLATTEN_ITERABLE_HPP
