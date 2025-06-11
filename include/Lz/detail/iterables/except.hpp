#pragma once

#ifndef LZ_EXCEPT_ITERABLE_HPP
#define LZ_EXCEPT_ITERABLE_HPP

#include <Lz/cached_size.hpp>
#include <Lz/detail/func_container.hpp>
#include <Lz/detail/iterators/except.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {

template<class Iterable1, class Iterable2, class BinaryPredicate>
class except_iterable : public lazy_view {
    using iterable2_type = conditional<sized<Iterable2>::value, ref_or_view<Iterable2>, cached_size_iterable<Iterable2>>;

    ref_or_view<Iterable1> _iterable1;
    iterable2_type _iterable2;
    func_container<BinaryPredicate> _binary_predicate;

public:
    using iterator = except_iterator<ref_or_view<Iterable1>, iterable2_type, func_container<BinaryPredicate>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

#ifdef LZ_HAS_CONCEPTS

    constexpr except_iterable()
        requires std::default_initializable<Iterable1> && std::default_initializable<iterable2_type> &&
                     std::default_initializable<BinaryPredicate>
    = default;

#else

    template<class I = decltype(_iterable1),
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<iterable2_type>::value &&
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
        return { _iterable1, std::begin(_iterable1), _iterable2, _binary_predicate };
    }

    template<class I = iter_t<Iterable1>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi<I>::value || is_sentinel<I, sentinel_t<Iterable1>>::value, iterator>
    begin() && {
        return { _iterable1, std::begin(_iterable1), std::move(_iterable2), std::move(_binary_predicate) };
    }

    template<class I = iter_t<Iterable1>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi<I>::value && !is_sentinel<I, sentinel_t<Iterable1>>::value, iterator>
    end() const& {
        return { _iterable1, std::end(_iterable1), _iterable2, _binary_predicate };
    }

    // clang-format off

    template<class I = iter_t<Iterable1>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14
    enable_if<!is_bidi<I>::value || is_sentinel<I, sentinel_t<Iterable1>>::value, default_sentinel>
    end() const noexcept {
        return {};
    }

    // clang-format on
};
} // namespace detail
} // namespace lz

#endif