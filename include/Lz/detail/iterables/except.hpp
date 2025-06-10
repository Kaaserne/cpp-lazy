#pragma once

#ifndef LZ_EXCEPT_ITERABLE_HPP
#define LZ_EXCEPT_ITERABLE_HPP

#include <Lz/detail/func_container.hpp>
#include <Lz/detail/iterators/except.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {

template<class Iterable1, class Iterable2, class BinaryPredicate>
class except_iterable : public lazy_view {
    ref_or_view<Iterable1> _iterable1;
    ref_or_view<Iterable2> _iterable2;
    func_container<BinaryPredicate> _binary_predicate;

public:
    using iterator =
        except_iterator<iter_t<Iterable1>, sentinel_t<Iterable1>, ref_or_view<Iterable2>, func_container<BinaryPredicate>>;
    using sentinel = typename iterator::sentinel;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

#ifdef LZ_HAS_CONCEPTS

    constexpr except_iterable()
        requires std::default_initializable<Iterable1> && std::default_initializable<Iterable2> &&
                     std::default_initializable<BinaryPredicate>
    = default;

#else

    template<class I = decltype(_iterable1),
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<Iterable2>::value &&
                               std::is_default_constructible<BinaryPredicate>::value>>
    constexpr except_iterable() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                         std::is_nothrow_default_constructible<Iterable2>::value &&
                                         std::is_nothrow_default_constructible<BinaryPredicate>::value) {
    }

#endif

    template<class I1, class I2>
    constexpr except_iterable(I1&& iterable1, I2&& iterable2, BinaryPredicate binary_predicate) :
        _iterable1{ iterable1 },
        _iterable2{ iterable2 },
        _binary_predicate{ std::move(binary_predicate) } {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { std::begin(_iterable1), std::end(_iterable1), _iterable2, _binary_predicate };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return { detail::begin(std::move(_iterable1)), detail::end(std::move(_iterable1)), std::move(_iterable2),
                 std::move(_binary_predicate) };
    }

    LZ_NODISCARD constexpr default_sentinel end() const noexcept {
        return {};
    }
};
} // namespace detail
} // namespace lz

#endif