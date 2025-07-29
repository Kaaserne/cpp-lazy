#pragma once

#ifndef LZ_INTERSECTION_ITERABLE_HPP
#define LZ_INTERSECTION_ITERABLE_HPP

#include <Lz/detail/func_container.hpp>
#include <Lz/detail/iterators/intersection.hpp>
#include <Lz/detail/maybe_owned.hpp>

namespace lz {
namespace detail {
template<class Iterable, class Iterable2, class BinaryPredicate>
class intersection_iterable : public lazy_view {
    maybe_owned<Iterable> _iterable;
    maybe_owned<Iterable2> _iterable2;
    func_container<BinaryPredicate> _compare;

public:
    using iterator = intersection_iterator<maybe_owned<Iterable>, maybe_owned<Iterable2>, func_container<BinaryPredicate>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

private:
    static constexpr bool return_sentinel = !is_bidi_tag<typename iterator::iterator_category>::value ||
                                            is_sentinel<iter_t<Iterable>, sentinel_t<Iterable>>::value ||
                                            is_sentinel<iter_t<Iterable2>, sentinel_t<Iterable2>>::value;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr intersection_iterable()
        requires std::default_initializable<maybe_owned<Iterable>> && std::default_initializable<maybe_owned<Iterable2>> &&
                     std::default_initializable<BinaryPredicate>
    = default;

#else

    template<class I = decltype(_iterable), class = enable_if<std::is_default_constructible<I>::value &&
                                                              std::is_default_constructible<maybe_owned<Iterable2>>::value &&
                                                              std::is_default_constructible<BinaryPredicate>::value>>
    constexpr intersection_iterable() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                               std::is_nothrow_default_constructible<maybe_owned<Iterable2>>::value &&
                                               std::is_nothrow_default_constructible<BinaryPredicate>::value) {
    }

#endif

    template<class I, class I2>
    constexpr intersection_iterable(I&& iterable, I2&& iterable2, BinaryPredicate compare) :
        _iterable{ std::forward<I>(iterable) },
        _iterable2{ std::forward<I2>(iterable2) },
        _compare{ std::move(compare) } {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { _iterable, _iterable2, std::begin(_iterable), std::begin(_iterable2), _compare };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<R, iterator> begin() && {
        return { _iterable, _iterable2, std::begin(_iterable), std::begin(_iterable2), std::move(_compare) };
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (!return_sentinel) {
            return iterator{ _iterable, _iterable2, std::end(_iterable), std::end(_iterable2), _compare };
        }
        else {
            return lz::default_sentinel;
        }
    }

#else

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!R, iterator> end() const {
        return { _iterable, _iterable2, std::end(_iterable), std::end(_iterable2), _compare };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD constexpr enable_if<R, default_sentinel_t> end() const {
        return {};
    }

#endif
};
}
} // namespace lz

#endif // LZ_INTERSECTION_ITERABLE_HPP