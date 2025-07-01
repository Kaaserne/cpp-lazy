#pragma once

#ifndef LZ_EXCEPT_ITERATOR_HPP
#define LZ_EXCEPT_ITERATOR_HPP

#include <Lz/algorithm.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {

template<class Iterable, class Iterable2, class BinaryPredicate>
class except_iterator
    : public iterator<except_iterator<Iterable, Iterable2, BinaryPredicate>, ref_t<iter_t<Iterable>>,
                      fake_ptr_proxy<ref_t<iter_t<Iterable>>>, diff_type<iter_t<Iterable>>,
                      common_type<iter_cat_t<iter_t<Iterable>>, std::bidirectional_iterator_tag>, default_sentinel> {

    using iter = iter_t<Iterable>;
    using iter_traits = std::iterator_traits<iter>;

public:
    using value_type = typename iter_traits::value_type;
    using difference_type = typename iter_traits::difference_type;
    using reference = typename iter_traits::reference;
    using pointer = fake_ptr_proxy<reference>;

private:
    Iterable _iterable;
    Iterable2 _to_except;
    iter _iterator;
    mutable BinaryPredicate _predicate;

    LZ_CONSTEXPR_CXX_14 void find_next() {
        using detail::find_if;
        using std::find_if;

        _iterator = find_if(std::move(_iterator), std::end(_iterable), [this](reference value) {
            const auto it =
                sized_lower_bound(std::begin(_to_except), value, _predicate, static_cast<difference_type>(lz::size(_to_except)));
            return it == std::end(_to_except) || _predicate(value, *it);
        });
    }

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr except_iterator()
        requires std::default_initializable<Iterable> && std::default_initializable<Iterable2> &&
                     std::default_initializable<BinaryPredicate> && std::default_initializable<iter>
    = default;

#else

    template<
        class I = Iterable,
        class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<Iterable2>::value &&
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
        if (std::begin(_to_except) == std::end(_to_except)) {
            return;
        }
        find_next();
    }

    LZ_CONSTEXPR_CXX_14 except_iterator& operator=(default_sentinel) {
        _iterator = std::end(_iterable);
        return *this;
    }

    constexpr reference dereference() const {
        return *_iterator;
    }

    constexpr pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        ++_iterator;
        find_next();
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        do {
            --_iterator;
            const auto it = sized_lower_bound(std::begin(_to_except), *_iterator, _predicate,
                                              static_cast<difference_type>(lz::size(_to_except)));
            if (it == std::end(_to_except) || _predicate(*_iterator, *it)) {
                return;
            }
        } while (_iterator != std::begin(_iterable));
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const except_iterator& b) const {
        LZ_ASSERT(std::end(_iterable) == std::end(b._iterable) && std::begin(_iterable) == std::begin(b._iterable) &&
                      std::begin(_to_except) == std::begin(b._to_except) && std::end(_to_except) == std::end(b._to_except),
                  "Incompatible iterators");
        return _iterator == b._iterator;
    }

    constexpr bool eq(default_sentinel) const {
        return _iterator == std::end(_iterable);
    }
};
} // namespace detail
} // namespace lz

#endif