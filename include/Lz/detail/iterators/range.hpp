#pragma once

#ifndef LZ_RANGE_ITERATOR_HPP
#define LZ_RANGE_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/procs.hpp>
#include <Lz/detail/traits.hpp>
#include <Lz/iterator_base.hpp>
#include <cmath>
#include <limits>

namespace lz {
namespace detail {

template<class Floating>
constexpr bool almost_equal(const Floating a, const Floating b, const Floating epsilon = static_cast<Floating>(1e-6)) {
    return std::fabs(a - b) < epsilon;
}

template<class Arithmetic, bool /* step wise */>
class range_iterator;

template<class Arithmetic>
class range_iterator<Arithmetic, true> : public iterator<range_iterator<Arithmetic, true>, Arithmetic, fake_ptr_proxy<Arithmetic>,
                                                         std::ptrdiff_t, std::random_access_iterator_tag> {
    Arithmetic _index{};
    Arithmetic _step{};

public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = Arithmetic;
    using difference_type = std::ptrdiff_t;
    using pointer = fake_ptr_proxy<Arithmetic>;
    using reference = Arithmetic;

    constexpr range_iterator() noexcept = default;

    constexpr range_iterator(const Arithmetic it, const Arithmetic step) noexcept : _index{ it }, _step{ step } {
    }

    // Operator= default_sentinel_t not necessary, as it never returns as default_sentinel_t
    LZ_CONSTEXPR_CXX_14 range_iterator& operator=(default_sentinel_t) = delete;

    constexpr value_type dereference() const noexcept {
        return _index;
    }

    constexpr pointer arrow() const noexcept {
        return fake_ptr_proxy<value_type>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() noexcept {
        _index += _step;
    }

    LZ_CONSTEXPR_CXX_14 void decrement() noexcept {
        _index -= _step;
    }

    difference_type difference(default_sentinel_t) const = delete; // Cannot calculate difference with default_sentinel_t

#ifdef LZ_HAS_CXX_17

    constexpr difference_type difference(const range_iterator& b) const noexcept {
        if constexpr (std::is_floating_point_v<Arithmetic>) {
            LZ_ASSERT(almost_equal(_step, b._step, std::numeric_limits<Arithmetic>::epsilon()), "Incompatible iterators");
            LZ_ASSERT(std::abs(_step) > std::numeric_limits<Arithmetic>::epsilon(), "Division by zero in range size calculation");
            const auto current_size = (_index - b._index) / _step;
            const auto int_part = static_cast<difference_type>(current_size);
            const auto arithmetic_int_part = static_cast<Arithmetic>(int_part);
            return !almost_equal(current_size, arithmetic_int_part) ? int_part + 1 : int_part;
        }
        else {
            LZ_ASSERT(_step != 0, "Division by zero in range size calculation");
            return static_cast<difference_type>(_index - b._index) / static_cast<difference_type>(_step);
        }
    }

    constexpr bool eq(const range_iterator& other) const noexcept {
        if constexpr (std::is_floating_point_v<Arithmetic>) {
            return almost_equal(_index, other._index);
        }
        else {
            return _index == other._index;
        }
    }

#else

    template<class A = Arithmetic>
    LZ_CONSTEXPR_CXX_14 enable_if<std::is_floating_point<A>::value, difference_type>
    difference(const range_iterator& b) const noexcept {
        LZ_ASSERT(almost_equal(_step, b._step), "Division by zero in range size calculation");
        LZ_ASSERT(std::abs(_step) > std::numeric_limits<Arithmetic>::epsilon(), "Division by zero in range size calculation");

        const auto current_size = (_index - b._index) / _step;
        const auto int_part = static_cast<difference_type>(current_size);
        return (current_size > static_cast<A>(int_part)) ? int_part + 1 : int_part;
    }

    template<class A = Arithmetic>
    LZ_CONSTEXPR_CXX_14 enable_if<!std::is_floating_point<A>::value, difference_type>
    difference(const range_iterator& b) const noexcept {
        LZ_ASSERT(_step == b._step, "Incompatible iterators");
        LZ_ASSERT(_step != 0, "Division by zero in range difference calculation");
        return static_cast<difference_type>(_index - b._index) / static_cast<difference_type>(_step);
    }

    template<class A = Arithmetic>
    LZ_CONSTEXPR_CXX_14 enable_if<std::is_floating_point<A>::value, bool> eq(const range_iterator& other) const noexcept {
        return almost_equal(_index, other._index);
    }

    template<class A = Arithmetic>
    constexpr enable_if<!std::is_floating_point<A>::value, bool> eq(const range_iterator& other) const noexcept {
        return _index == other._index;
    }

#endif

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type value) noexcept {
        _index += static_cast<Arithmetic>(value) * _step;
    }

    bool eq(default_sentinel_t) const = delete; // Cannot compare with default_sentinel_t
};

template<class Arithmetic>
class range_iterator<Arithmetic, false>
    : public iterator<range_iterator<Arithmetic, false>, Arithmetic, fake_ptr_proxy<Arithmetic>, std::ptrdiff_t,
                      std::random_access_iterator_tag> {
    Arithmetic _index{};

public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = Arithmetic;
    using difference_type = std::ptrdiff_t;
    using pointer = fake_ptr_proxy<Arithmetic>;
    using reference = Arithmetic;

    constexpr range_iterator() noexcept = default;

    explicit constexpr range_iterator(const Arithmetic it) noexcept : _index{ it } {
    }

    // Operator= default_sentinel_t not necessary, as it never returns as default_sentinel_t
    LZ_CONSTEXPR_CXX_14 range_iterator& operator=(default_sentinel_t) = delete;

    constexpr value_type dereference() const noexcept {
        return _index;
    }

    constexpr pointer arrow() const noexcept {
        return fake_ptr_proxy<value_type>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() noexcept {
        ++_index;
    }

    LZ_CONSTEXPR_CXX_14 void decrement() noexcept {
        --_index;
    }

    difference_type difference(default_sentinel_t) const = delete; // Cannot calculate difference with default_sentinel_t

#ifdef LZ_HAS_CXX_17

    constexpr difference_type difference(const range_iterator& b) const noexcept {
        const auto current_size = _index - b._index;
        if constexpr (std::is_floating_point_v<Arithmetic>) {
            const auto int_part = static_cast<difference_type>(current_size);
            const auto arithmetic_int_part = static_cast<Arithmetic>(int_part);
            return !almost_equal(current_size, arithmetic_int_part) ? int_part + 1 : int_part;
        }
        else {
            return static_cast<difference_type>(current_size);
        }
    }

    constexpr bool eq(const range_iterator& other) const noexcept {
        if constexpr (std::is_floating_point_v<Arithmetic>) {
            return almost_equal(_index, other._index);
        }
        else {
            return _index == other._index;
        }
    }

#else

    template<class A = Arithmetic>
    LZ_CONSTEXPR_CXX_14 enable_if<std::is_floating_point<A>::value, difference_type>
    difference(const range_iterator& b) const noexcept {
        const auto current_size = _index - b._index;
        const auto int_part = static_cast<difference_type>(current_size);
        const auto arithmetic_int_part = static_cast<Arithmetic>(int_part);
        return !almost_equal(current_size, arithmetic_int_part) ? int_part + 1 : int_part;
    }

    template<class A = Arithmetic>
    LZ_CONSTEXPR_CXX_14 enable_if<!std::is_floating_point<A>::value, difference_type>
    difference(const range_iterator& b) const noexcept {
        return static_cast<difference_type>(_index - b._index);
    }

    template<class A = Arithmetic>
    LZ_CONSTEXPR_CXX_14 enable_if<std::is_floating_point<A>::value, bool> eq(const range_iterator& other) const noexcept {
        return almost_equal(_index, other._index);
    }

    template<class A = Arithmetic>
    constexpr enable_if<!std::is_floating_point<A>::value, bool> eq(const range_iterator& other) const noexcept {
        return _index == other._index;
    }

#endif

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type value) noexcept {
        _index += static_cast<Arithmetic>(value);
    }

    bool eq(default_sentinel_t) const = delete; // Cannot compare with default_sentinel_t
};
} // namespace detail
} // namespace lz

#endif