#pragma once

#ifndef LZ_EXCEPT_ITERATOR_HPP
#define LZ_EXCEPT_ITERATOR_HPP

#include <Lz/detail/procs/assert.hpp>
#include <Lz/algorithm/find_if.hpp>
#include <Lz/detail/algorithm/lower_bound.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/iterator.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>
#include <Lz/procs/size.hpp>
#include <Lz/util/default_sentinel.hpp>

namespace lz {
namespace detail {

template<class Iterable, class Iterable2, class BinaryPredicate>
class except_iterator : public iterator<except_iterator<Iterable, Iterable2, BinaryPredicate>, ref_t<iter_t<Iterable>>,
                                        fake_ptr_proxy<ref_t<iter_t<Iterable>>>, diff_type<iter_t<Iterable>>,
                                        bidi_strongest_cat<iter_cat_t<iter_t<Iterable>>>, default_sentinel_t> {

    using iter = iter_t<Iterable>;
    using iter_traits = std::iterator_traits<iter>;

public:
    using value_type = typename iter_traits::value_type;
    using difference_type = typename iter_traits::difference_type;
    using reference = typename iter_traits::reference;
    using pointer = fake_ptr_proxy<reference>;

    constexpr except_iterator(const except_iterator&) = default;
    LZ_CONSTEXPR_CXX_14 except_iterator& operator=(const except_iterator&) = default;

private:
    Iterable _iterable{};
    Iterable2 _to_except{};
    iter _iterator{};
    mutable BinaryPredicate _predicate{};

    LZ_CONSTEXPR_CXX_14 void find_next() {
        _iterator = detail::find_if(std::move(_iterator), _iterable.end(), [this](reference value) {
            const auto it = detail::sized_lower_bound(_to_except.begin(), value, _predicate,
                                                      static_cast<difference_type>(lz::size(_to_except)));
            return it == _to_except.end() || _predicate(value, *it);
        });
    }

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr except_iterator()
        requires(std::default_initializable<Iterable> && std::default_initializable<Iterable2> &&
                 std::default_initializable<BinaryPredicate> && std::default_initializable<iter>)
    = default;

#else

    template<
        class I = Iterable,
        class = enable_if_t<std::is_default_constructible<I>::value && std::is_default_constructible<Iterable2>::value &&
                            std::is_default_constructible<BinaryPredicate>::value && std::is_default_constructible<iter>::value>>
    constexpr except_iterator() noexcept(std::is_nothrow_default_constructible<Iterable2>::value &&
                                         std::is_nothrow_default_constructible<I>::value &&
                                         std::is_nothrow_default_constructible<BinaryPredicate>::value &&
                                         std::is_nothrow_default_constructible<iter>::value) {
    }

#endif

    template<class I, class I2>
    LZ_CONSTEXPR_CXX_14 except_iterator(I&& iterable, iter it, I2&& to_except, BinaryPredicate compare) :
        _iterable{ std::forward<I>(iterable) },
        _to_except{ std::forward<I2>(to_except) },
        _iterator{ std::move(it) },
        _predicate{ std::move(compare) } {
        if (_to_except.begin() == _to_except.end()) {
            return;
        }
        find_next();
    }

    LZ_CONSTEXPR_CXX_14 except_iterator& operator=(default_sentinel_t) {
        _iterator = _iterable.end();
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        LZ_ASSERT_INCREMENTABLE(!eq(lz::default_sentinel));
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT_INCREMENTABLE(!eq(lz::default_sentinel));
        ++_iterator;
        find_next();
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        LZ_ASSERT_DECREMENTABLE(_iterator != _iterable.begin());
        do {
            --_iterator;
            const auto it = detail::sized_lower_bound(_to_except.begin(), *_iterator, _predicate,
                                                      static_cast<difference_type>(lz::size(_to_except)));
            if (it == _to_except.end() || _predicate(*_iterator, *it)) {
                return;
            }
        } while (_iterator != _iterable.begin());
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const except_iterator& other) const {
        LZ_ASSERT_COMPATIBLE(_iterable.end() == other._iterable.end() && _iterable.begin() == other._iterable.begin() &&
                             _to_except.begin() == other._to_except.begin() && _to_except.end() == other._to_except.end());
        return _iterator == other._iterator;
    }

    constexpr bool eq(default_sentinel_t) const {
        return _iterator == _iterable.end();
    }
};
} // namespace detail
} // namespace lz

#endif
