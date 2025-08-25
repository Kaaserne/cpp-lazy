#pragma once

#ifndef LZ_EXCEPT_ITERABLE_HPP
#define LZ_EXCEPT_ITERABLE_HPP

#include <Lz/cached_size.hpp>
#include <Lz/detail/func_container.hpp>
#include <Lz/detail/iterators/except.hpp>
#include <Lz/detail/maybe_owned.hpp>

namespace lz {
namespace detail {

template<class Iterable1, class Iterable2, class BinaryPredicate>
class except_iterable : public lazy_view {
    using iterable2_type = conditional_t<is_sized<Iterable2>::value, maybe_owned<Iterable2>, cached_size_iterable<Iterable2>>;

    maybe_owned<Iterable1> _iterable1;
    iterable2_type _iterable2;
    func_container<BinaryPredicate> _binary_predicate;

public:
    using iterator = except_iterator<maybe_owned<Iterable1>, iterable2_type, func_container<BinaryPredicate>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

private:
    using iter = iter_t<Iterable1>;
    static constexpr bool return_sentinel = !is_bidi<iter>::value || has_sentinel<Iterable1>::value;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr except_iterable()
        requires(std::default_initializable<maybe_owned<Iterable1>> && std::default_initializable<iterable2_type> &&
                 std::default_initializable<BinaryPredicate>)
    = default;

#else

    template<class I = decltype(_iterable1), class = enable_if_t<std::is_default_constructible<I>::value &&
                                                                 std::is_default_constructible<iterable2_type>::value &&
                                                                 std::is_default_constructible<BinaryPredicate>::value>>
    constexpr except_iterable() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                         std::is_nothrow_default_constructible<iterable2_type>::value &&
                                         std::is_nothrow_default_constructible<BinaryPredicate>::value) {
    }

#endif

    template<class I1, class I2>
    constexpr except_iterable(I1&& iterable1, I2&& iterable2, BinaryPredicate binary_predicate) :
        _iterable1{ std::forward<I1>(iterable1) },
        _iterable2{ std::forward<I2>(iterable2) },
        _binary_predicate{ std::move(binary_predicate) } {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { _iterable1, _iterable1.begin(), _iterable2, _binary_predicate };
    }

#ifdef LZ_HAS_CONCEPTS

    [[nodiscard]] constexpr iterator begin() &&
        requires(return_sentinel)
    {
        return { _iterable1, _iterable1.begin(), std::move(_iterable2), std::move(_binary_predicate) };
    }

#else

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<R, iterator> begin() && {
        return { _iterable1, _iterable1.begin(), std::move(_iterable2), std::move(_binary_predicate) };
    }

#endif

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (!return_sentinel) {
            return iterator{ _iterable1, _iterable1.end(), _iterable2, _binary_predicate };
        }
        else {
            return default_sentinel;
        }
    }

#else

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<!R, iterator> end() const {
        return { _iterable1, _iterable1.end(), _iterable2, _binary_predicate };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<R, default_sentinel_t> end() const noexcept {
        return default_sentinel;
    }

#endif
};
} // namespace detail
} // namespace lz

#endif
