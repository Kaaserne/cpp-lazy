#pragma once

#ifndef LZ_RANDOM_ITERATOR_HPP
#define LZ_RANDOM_ITERATOR_HPP

#include <Lz/detail/compiler_config.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/iterator.hpp>
#include <Lz/detail/procs/assert.hpp>
#include <Lz/detail/traits/conditional.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <Lz/util/default_sentinel.hpp>

namespace lz {
namespace detail {
template<class Arithmetic, class Distribution, class Generator, bool UseSentinel>
class random_iterator
    : public iterator<
          random_iterator<Arithmetic, Distribution, Generator, UseSentinel>, Arithmetic, fake_ptr_proxy<Arithmetic>, ptrdiff_t,
          std::random_access_iterator_tag,
          conditional_t<UseSentinel, default_sentinel_t, random_iterator<Arithmetic, Distribution, Generator, UseSentinel>>> {
public:
    using value_type = Arithmetic;
    using difference_type = ptrdiff_t;
    using pointer = fake_ptr_proxy<Arithmetic>;
    using reference = value_type;
    using result_type = value_type;

private:
    mutable Distribution _distribution{};
    Generator* _generator{ nullptr };
    ptrdiff_t _current{};

public:
    constexpr random_iterator(const random_iterator&) =
        default;
    LZ_CONSTEXPR_CXX_14 random_iterator& operator=(const random_iterator&) = default;

#ifdef LZ_HAS_CONCEPTS

    constexpr random_iterator()
        requires(std::default_initializable<Distribution>)
    = default;

#else

    template<class D = Distribution, class = enable_if_t<std::is_default_constructible<D>::value>>
    constexpr random_iterator() noexcept(std::is_nothrow_default_constructible<D>::value) {
    }

#endif

    constexpr random_iterator(const Distribution& distribution, Generator& generator, const ptrdiff_t current) :
        _distribution{ distribution },
        _generator{ detail::addressof(generator) },
        _current{ current } {
    }

    LZ_CONSTEXPR_CXX_14 random_iterator& operator=(default_sentinel_t) noexcept {
        _current = 0;
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 value_type dereference() const {
        LZ_ASSERT_DEREFERENCABLE(_current > 0);
        LZ_ASSERT(_generator != nullptr, "Generator is not initialized");
        return _distribution(*_generator);
    }

    LZ_CONSTEXPR_CXX_14 void increment() noexcept {
        LZ_ASSERT_INCREMENTABLE(_current >= 0);
        --_current;
    }

    LZ_CONSTEXPR_CXX_14 void decrement() noexcept {
        ++_current;
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
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
        LZ_ASSERT_ADDABLE(_current >= 0);
    }

    constexpr difference_type difference(const random_iterator& other) const noexcept {
        return other._current - _current;
    }

    constexpr difference_type difference(default_sentinel_t) const noexcept {
        return -_current;
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const random_iterator& other) const noexcept {
        LZ_ASSERT_COMPATIBLE(_generator == other._generator);
        return _current == other._current;
    }

    constexpr bool eq(default_sentinel_t) const noexcept {
        return _current == 0;
    }
};
} // namespace detail
} // namespace lz

#endif
