#pragma once

#ifndef LZ_RANDOM_ITERATOR_HPP
#define LZ_RANDOM_ITERATOR_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {
template<class Arithmetic, class Distribution, class Generator, bool UseSentinel>
class random_iterator
    : public iterator<
          random_iterator<Arithmetic, Distribution, Generator, UseSentinel>, Arithmetic, fake_ptr_proxy<Arithmetic>,
          std::ptrdiff_t, conditional<UseSentinel, std::forward_iterator_tag, std::random_access_iterator_tag>,
          conditional<UseSentinel, default_sentinel, random_iterator<Arithmetic, Distribution, Generator, UseSentinel>>> {
public:
    using value_type = Arithmetic;
    using difference_type = std::ptrdiff_t;
    using pointer = fake_ptr_proxy<Arithmetic>;
    using reference = value_type;
    using result_type = value_type;

private:
    mutable Distribution _distribution;
    Generator* _generator{ nullptr };
    std::ptrdiff_t _current{};

public:
    constexpr random_iterator(const Distribution& distribution, Generator& generator, const std::ptrdiff_t current) :
        _distribution{ distribution },
        _generator{ &generator },
        _current{ current } {
    }

    constexpr random_iterator() = default;

    LZ_CONSTEXPR_CXX_14 value_type dereference() const {
        return _distribution(*_generator);
    }

    LZ_CONSTEXPR_CXX_14 value_type operator()() const {
        return dereference();
    }

    LZ_CONSTEXPR_CXX_14 void increment() noexcept {
        --_current;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 result_type(min)() const noexcept {
        return (_distribution->min)();
    }

    LZ_CONSTEXPR_CXX_14 result_type(max)() const noexcept {
        return (_distribution->max)();
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type n) noexcept {
        _current -= n;
    }

    constexpr difference_type difference(const random_iterator& b) const noexcept {
        return b._current - _current;
    }

    constexpr bool eq(const random_iterator& b) const noexcept {
        return _current == b._current;
    }

    constexpr bool eq(default_sentinel) const noexcept {
        return _current == 0;
    }
};
} // namespace detail
} // namespace lz

#endif