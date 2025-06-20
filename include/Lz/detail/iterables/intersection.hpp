#pragma once

#ifndef LZ_INTERSECTION_ITERABLE_HPP
#define LZ_INTERSECTION_ITERABLE_HPP

#include <Lz/detail/iterators/intersection.hpp>
#include <Lz/detail/ref_or_view.hpp>
#include <Lz/detail/func_container.hpp>

namespace lz {
namespace detail {
template<class Iterable, class Iterable2, class BinaryPredicate>
class intersection_iterable : public lazy_view {
    ref_or_view<Iterable> _iterable;
    ref_or_view<Iterable2> _iterable2;
    func_container<BinaryPredicate> _compare;

public:
    using iterator = intersection_iterator<iter_t<Iterable>, sentinel_t<Iterable>, iter_t<Iterable2>, sentinel_t<Iterable2>,
                                           func_container<BinaryPredicate>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

#ifdef LZ_HAS_CONCEPTS

    constexpr intersection_iterable()
        requires std::default_initializable<Iterable> && std::default_initializable<Iterable2> &&
                     std::default_initializable<BinaryPredicate>
    = default;

#else

    template<class I = decltype(_iterable),
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<Iterable2>::value &&
                               std::is_default_constructible<BinaryPredicate>::value>>
    constexpr intersection_iterable() {
    }

#endif

    template<class I, class I2>
    constexpr intersection_iterable(I&& iterable, I2&& iterable2, BinaryPredicate compare) :
        _iterable{ std::forward<I>(iterable) },
        _iterable2{ std::forward<I2>(iterable2) },
        _compare{ std::move(compare) } {
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr iterator begin() const& {
        if constexpr (is_bidi_tag<typename iterator::iterator_category>::value) {
            // clang-format off
            return { std::begin(_iterable), std::begin(_iterable), std::end(_iterable), std::begin(_iterable2),
                     std::begin(_iterable2), std::end(_iterable2), _compare };
            // clang-format on
        }
        else {
            return { std::begin(_iterable), std::end(_iterable), std::begin(_iterable2), std::end(_iterable2), _compare };
        }
    }

#else

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, iterator> begin() const& {
        return { std::begin(_iterable), std::end(_iterable), std::begin(_iterable2), std::end(_iterable2), _compare };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, iterator> begin() const& {
        // clang-format off
        return { std::begin(_iterable), std::begin(_iterable), std::end(_iterable), std::begin(_iterable2),
                 std::begin(_iterable2), std::end(_iterable2), _compare };
        // clang-format on
    }

#endif

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, iterator> begin() && {
        return { detail::begin(std::move(_iterable)), detail::end(std::move(_iterable)), detail::begin(std::move(_iterable2)),
                 detail::end(std::move(_iterable2)), std::move(_compare) };
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (is_bidi_tag<typename iterator::iterator_category>::value) {
            // clang-format off
            return iterator{ std::end(_iterable), std::begin(_iterable), std::end(_iterable), std::end(_iterable2),
                             std::begin(_iterable2), std::end(_iterable2), _compare };
            // clang-format on
        }
        else {
            return default_sentinel{};
        }
    }

#else

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, iterator> end() const {
        // clang-format off
        return { std::end(_iterable), std::begin(_iterable), std::end(_iterable), std::end(_iterable2),
                 std::begin(_iterable2), std::end(_iterable2), _compare };
        // clang-format on
    }
    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD constexpr enable_if<!is_bidi_tag<I>::value, default_sentinel> end() const {
        return {};
    }

#endif
};
}
} // namespace lz

#endif // LZ_INTERSECTION_ITERABLE_HPP