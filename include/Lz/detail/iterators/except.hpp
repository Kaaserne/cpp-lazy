#pragma once

#ifndef LZ_EXCEPT_ITERATOR_HPP
#define LZ_EXCEPT_ITERATOR_HPP

#include <Lz/algorithm.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {
template<class Iterator, class S, class Iterable, class BinaryPredicate, class = void /* is to except sized */>
class except_iterator;

template<class Iterator, class S, class Iterable, class BinaryPredicate>
class except_iterator<Iterator, S, Iterable, BinaryPredicate, enable_if<sized<Iterable>::value>>
    : public iterator<except_iterator<Iterator, S, Iterable, BinaryPredicate>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>,
                      diff_type<Iterator>, std::forward_iterator_tag, default_sentinel> {

    using iter_traits = std::iterator_traits<Iterator>;

public:
    using value_type = typename iter_traits::value_type;
    using difference_type = typename iter_traits::difference_type;
    using reference = typename iter_traits::reference;
    using pointer = fake_ptr_proxy<reference>;

private:
    Iterator _iterator;
    Iterable _to_except;
    S _end;
    mutable BinaryPredicate _predicate;

    LZ_CONSTEXPR_CXX_14 void find_next() {
        using detail::find_if;
        using std::find_if;

        _iterator = find_if(std::move(_iterator), _end, [this](ref_t<Iterator> value) {
            const auto it =
                sized_lower_bound(_to_except.begin(), value, _predicate, static_cast<difference_type>(lz::size(_to_except)));
            return it == _to_except.end() || _predicate(value, *it);
        });
    }

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr except_iterator()
        requires std::default_initializable<Iterator> && std::default_initializable<Iterable> &&
                     std::default_initializable<Iterable2> && std::default_initializable<BinaryPredicate>
    = default;

#else

    template<class I = Iterator,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<Iterable>::value &&
                               std::is_default_constructible<Iterable2>::value &&
                               std::is_default_constructible<BinaryPredicate>::value>>
    constexpr except_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                         std::is_nothrow_default_constructible<Iterable>::value &&
                                         std::is_nothrow_default_constructible<Iterable2>::value &&
                                         std::is_nothrow_default_constructible<BinaryPredicate>::value) {
    }

#endif

    template<class I>
    LZ_CONSTEXPR_CXX_14 except_iterator(Iterator begin, S end, I&& to_except, BinaryPredicate compare) :
        _iterator{ std::move(begin) },
        _to_except{ std::forward<I>(to_except) },
        _end{ std::move(end) },
        _predicate{ std::move(compare) } {
        if (std::begin(_to_except) == std::end(_to_except)) {
            return;
        }
        find_next();
    }

    LZ_CONSTEXPR_CXX_14 except_iterator& operator=(default_sentinel) {
        _iterator = _end;
        return *this;
    }

    constexpr reference dereference() const {
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
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
        LZ_ASSERT(_end == b._end && _to_except.begin() == b._to_except.begin() && _to_except.end() == b._to_except.end() &&
                      _size == b._size,
                  "Incompatible iterators");
        return _iterator == b._iterator;
    }

    constexpr bool eq(default_sentinel) const {
        return _iterator == _end;
    }
};
} // namespace detail
} // namespace lz

#endif