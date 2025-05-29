#pragma once

#include "Lz/detail/compiler_checks.hpp"

#include <type_traits>
#ifndef LZ_RANGE_ITERATOR_HPP
#define LZ_RANGE_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/procs.hpp>
#include <Lz/detail/traits.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {

template<class Arithmetic>
class range_iterator : public iterator<range_iterator<Arithmetic>, Arithmetic, fake_ptr_proxy<Arithmetic>, std::ptrdiff_t,
                                       std::random_access_iterator_tag> {
    Arithmetic _index{};
    Arithmetic _step{};

public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = Arithmetic;
    using difference_type = std::ptrdiff_t;
    using pointer = fake_ptr_proxy<Arithmetic>;
    using reference = Arithmetic;

    constexpr range_iterator(const Arithmetic it, const Arithmetic step) noexcept : _index{ it }, _step{ step } {
    }

    // Operator= default_sentinel not necessary, as it never returns as default_sentinel

    constexpr value_type dereference() const noexcept {
        return _index;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const noexcept {
        return fake_ptr_proxy<value_type>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() noexcept {
        _index += _step;
    }

    LZ_CONSTEXPR_CXX_14 void decrement() noexcept {
        _index -= _step;
    }

    template<class A = Arithmetic>
    LZ_CONSTEXPR_CXX_14 enable_if<std::is_floating_point<A>::value, difference_type>
    difference(const range_iterator& b) const noexcept {
        LZ_ASSERT(_step == b._step, "Incompatible iterators");
        LZ_ASSERT(_step != 0, "Division by zero in range difference calculation");

        const auto x = (_index - b._index) / _step;
        const auto int_part = static_cast<difference_type>(x);
        return (x > static_cast<A>(int_part)) ? int_part + 1 : int_part;
    }

    template<class A = Arithmetic>
    LZ_CONSTEXPR_CXX_14 enable_if<!std::is_floating_point<A>::value, difference_type>
    difference(const range_iterator& b) const noexcept {
        LZ_ASSERT(_step == b._step, "Incompatible iterators");
        LZ_ASSERT(_step != 0, "Division by zero in range difference calculation");

        return static_cast<difference_type>(_index - b._index) / static_cast<difference_type>(_step);
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type value) noexcept {
        _index += static_cast<Arithmetic>(value) * _step;
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const range_iterator& other) const noexcept {
        LZ_ASSERT(_step == other._step, "Incompatible iterators");
        return _index == other._index;
    }
};
} // namespace detail
} // namespace lz

#endif