#pragma once

#ifndef LZ_INTERSECTION_ITERATOR_HPP
#define LZ_INTERSECTION_ITERATOR_HPP

#include <Lz/algorithm.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/traits.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {

template<class Iterable1, class Iterable2, class BinaryPredicate>
class intersection_iterator
    : public iterator<intersection_iterator<Iterable1, Iterable2, BinaryPredicate>, ref_t<iter_t<Iterable1>>,
                      fake_ptr_proxy<ref_t<iter_t<Iterable1>>>, diff_type<iter_t<Iterable1>>,
                      common_type<iter_cat_t<iter_t<Iterable1>>, iter_cat_t<iter_t<Iterable2>>, std::bidirectional_iterator_tag>,
                      default_sentinel_t> {

    using it1 = iter_t<Iterable1>;
    using it2 = iter_t<Iterable2>;

    it1 _iterator1;
    it2 _iterator2;
    Iterable1 _iterable1;
    Iterable2 _iterable2;
    mutable BinaryPredicate _compare;

    using iter_traits = std::iterator_traits<it1>;

    void find_next() {
        using detail::find_if;
        using std::find_if;

        using ref_type = typename iter_traits::reference;

        _iterator1 = find_if(std::move(_iterator1), std::end(_iterable1), [this](ref_type value) {
            while (_iterator2 != std::end(_iterable2)) {
                if (_compare(value, *_iterator2)) {
                    return false;
                }
                else if (!_compare(*_iterator2, value)) {
                    return true;
                }
                ++_iterator2;
            }
            return false;
        });
    }

public:
    using value_type = typename iter_traits::value_type;
    using difference_type = typename iter_traits::difference_type;
    using reference = typename iter_traits::reference;
    using pointer = fake_ptr_proxy<reference>;

#ifdef LZ_HAS_CONCEPTS

    constexpr intersection_iterator()
        requires std::default_initializable<it1> && std::default_initializable<it2> && std::default_initializable<Iterable1> &&
                     std::default_initializable<Iterable2> && std::default_initializable<BinaryPredicate>
    = default;

#else

    template<
        class I = it1,
        class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<it2>::value &&
                          std::is_default_constructible<Iterable1>::value && std::is_default_constructible<Iterable2>::value &&
                          std::is_default_constructible<BinaryPredicate>::value>>
    constexpr intersection_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                               std::is_nothrow_default_constructible<it2>::value &&
                                               std::is_nothrow_default_constructible<Iterable1>::value &&
                                               std::is_nothrow_default_constructible<Iterable2>::value &&
                                               std::is_nothrow_default_constructible<BinaryPredicate>::value) {
    }

#endif

    template<class I1, class I2>
    LZ_CONSTEXPR_CXX_14 intersection_iterator(I1&& i1, I2&& i2, it1 it_1, it2 it_2, BinaryPredicate compare) :
        _iterator1{ std::move(it_1) },
        _iterator2{ std::move(it_2) },
        _iterable1{ std::forward<I1>(i1) },
        _iterable2{ std::forward<I2>(i2) },
        _compare{ std::move(compare) } {
        find_next();
    }

    LZ_CONSTEXPR_CXX_14 intersection_iterator& operator=(default_sentinel_t) {
        _iterator1 = std::end(_iterable1);
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        LZ_ASSERT(!eq(lz::default_sentinel), "Cannot dereference end iterator");
        return *_iterator1;
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT(!eq(lz::default_sentinel), "Cannot increment end iterator");
        ++_iterator2, ++_iterator1;
        find_next();
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        LZ_ASSERT(_iterator1 != std::begin(_iterable1) && _iterator2 != std::begin(_iterable2),
                  "Cannot decrement begin iterator");
        --_iterator2, --_iterator1;

        while (_iterator2 != std::begin(_iterable2) && _iterator1 != std::begin(_iterable1)) {
            if (_compare(*_iterator1, *_iterator2)) {
                --_iterator2;
                continue;
            }
            if (!_compare(*_iterator2, *_iterator1)) {
                return;
            }
            --_iterator1;
        }
        if (_iterator2 == std::begin(_iterable2)) {
            _iterator1 = std::begin(_iterable1);
        }
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const intersection_iterator& other) const {
        LZ_ASSERT(std::begin(_iterable1) == std::begin(other._iterable1) && std::end(_iterable1) == std::end(other._iterable1) &&
                      std::begin(_iterable2) == std::begin(other._iterable2) &&
                      std::end(_iterable2) == std::end(other._iterable2),
                  "Incompatible iterators");
        return _iterator1 == other._iterator1;
    }

    constexpr bool eq(default_sentinel_t) const {
        return _iterator1 == std::end(_iterable1);
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_INTERSECTION_ITERATOR_HPP
