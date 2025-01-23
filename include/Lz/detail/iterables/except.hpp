#pragma once

#ifndef LZ_EXCEPT_ITERABLE_HPP
#define LZ_EXCEPT_ITERABLE_HPP

#include <Lz/detail/iterators/except.hpp>

namespace lz {
namespace detail {

template<class Iterable1, class Iterable2, class BinaryPredicate>
class except_iterable {
    iterable_ref<Iterable1> _iterable1;
    iterable_ref<Iterable2> _iterable2;
    BinaryPredicate _binaryPredicate;

public:
    using iterator = detail::except_iterator<
        iter_t<Iterable1>, sentinel_t<Iterable1>, iter_t<Iterable2>, sentinel_t<Iterable2>, BinaryPredicate>;
    using sentinel = typename iterator::sentinel;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    template<class I1, class I2>
    constexpr except_iterable(I1&& iterable1, I2&& iterable2, BinaryPredicate binary_predicate) :
        _iterable1{ iterable1 },
        _iterable2{ iterable2 },
        _binaryPredicate{ std::move(binary_predicate) } {
    }

    constexpr except_iterable() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { std::begin(_iterable1.get()), std::end(_iterable1.get()), std::begin(_iterable2.get()),
                 std::end(_iterable2.get()), _binaryPredicate };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return { std::move(_iterable1.get()).begin(), std::move(_iterable1.get()).end(),
                 std::move(_iterable2.get()).begin(), std::move(_iterable2.get()).end(),
                 std::move(_binaryPredicate) };
    }

    LZ_NODISCARD constexpr sentinel end() const noexcept {
        return {};
    }
};
} // namespace detail
} // namespace lz

#endif