#pragma once

#ifndef LZ_FILTER_ITERATOR_HPP
#define LZ_FILTER_ITERATOR_HPP

#include <Lz/detail/algorithm.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/iterator_base.hpp>
#include <algorithm>

namespace lz {
namespace detail {

template<class Iterable, class UnaryPredicate>
class filter_iterator
    : public iterator<filter_iterator<Iterable, UnaryPredicate>, ref_t<iter_t<Iterable>>, fake_ptr_proxy<ref_t<iter_t<Iterable>>>,
                      diff_type<iter_t<Iterable>>, common_type<iter_cat_t<iter_t<Iterable>>, std::bidirectional_iterator_tag>,
                      default_sentinel> {

    using it = iter_t<Iterable>;
    using traits = std::iterator_traits<it>;

public:
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

    LZ_CONSTEXPR_CXX_14 void find() {
        using detail::find_if;
        using std::find_if;
        _iterator = find_if(std::move(_iterator), std::end(_iterable), _predicate);
    }

private:
    it _iterator;
    Iterable _iterable;
    UnaryPredicate _predicate;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr filter_iterator()
        requires std::default_initializable<Iterable> && std::default_initializable<it> &&
                     std::default_initializable<UnaryPredicate>
    = default;

#else

    template<class I = it,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<Iterable>::value &&
                               std::is_default_constructible<UnaryPredicate>::value>>
    constexpr filter_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                         std::is_nothrow_default_constructible<Iterable>::value &&
                                         std::is_nothrow_default_constructible<UnaryPredicate>::value) {
    }

#endif

    template<class I>
    LZ_CONSTEXPR_CXX_14 filter_iterator(I&& iterable, it iter, UnaryPredicate up) :
        _iterator{ std::move(iter) },
        _iterable{ std::forward<I>(iterable) },
        _predicate{ std::move(up) } {
        find();
    }

    LZ_CONSTEXPR_CXX_14 filter_iterator& operator=(default_sentinel) {
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
        find();
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        do {
            --_iterator;
        } while (!_predicate(*_iterator) && _iterator != std::begin(_iterable));
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const filter_iterator& b) const {
        LZ_ASSERT(std::end(_iterable) == std::end(b._iterable) && std::begin(_iterable) == std::begin(b._iterable),
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