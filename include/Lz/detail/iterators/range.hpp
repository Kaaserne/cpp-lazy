#pragma once

#ifndef LZ_RANGE_ITERATOR_HPP
#define LZ_RANGE_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/procs.hpp>
#include <Lz/detail/traits.hpp>
#include <Lz/iterator_base.hpp>
#include <iterator>
#include <limits>

namespace lz {
namespace detail {
template<class Arithmetic>
class range_iterator : public iterator<range_iterator<Arithmetic>, Arithmetic, fake_ptr_proxy<Arithmetic>, std::ptrdiff_t,
                                       std::random_access_iterator_tag> {
    Arithmetic _iterator{};
    Arithmetic _step{};

public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = Arithmetic;
    using difference_type = std::ptrdiff_t;
    using pointer = fake_ptr_proxy<Arithmetic>;
    using reference = Arithmetic;

    constexpr range_iterator(const Arithmetic it, const Arithmetic step) noexcept :
        _iterator{ it },
        _step{ step } {
    }

    LZ_CONSTEXPR_CXX_14 range_iterator& operator=(default_sentinel) noexcept {
        _iterator = _step < 0 ? std::numeric_limits<Arithmetic>::min() : std::numeric_limits<Arithmetic>::max();
        return *this;
    }

    constexpr value_type dereference() const noexcept {
        return _iterator;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const noexcept {
        return fake_ptr_proxy<value_type>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() noexcept {
        _iterator += _step;
    }

    LZ_CONSTEXPR_CXX_14 void decrement() noexcept {
        _iterator -= _step;
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const range_iterator& b) const {
        LZ_ASSERT(_step == b._step, "Incompatible iterators");
        return static_cast<difference_type>((_iterator - b._iterator) / _step);
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type value) noexcept {
        _iterator += static_cast<Arithmetic>(value) * _step;
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const range_iterator& b) const noexcept {
        LZ_ASSERT(_step == b._step, "Incompatible iterators");
        return _step < 0 ? _iterator <= b._iterator : _iterator >= b._iterator;
    }
};
} // namespace detail
} // namespace lz

#endif