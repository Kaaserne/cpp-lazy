#pragma once

#ifndef LZ_UNIQUE_ITERATOR_HPP
#define LZ_UNIQUE_ITERATOR_HPP

#include <Lz/detail/algorithm.hpp>
#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/iterator_base.hpp>
#include <algorithm>

namespace lz {
namespace detail {
template<class Iterator, class S, class BinaryPredicate, class = void>
class unique_iterator;

template<class Iterator, class S, class BinaryPredicate>
class unique_iterator<Iterator, S, BinaryPredicate, enable_if<!is_bidi<Iterator>::value>>
    : public iterator<unique_iterator<Iterator, S, BinaryPredicate>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>,
                      diff_type<Iterator>, iter_cat_t<Iterator>, default_sentinel> {

    using traits = std::iterator_traits<Iterator>;

    Iterator _iterator;
    S _end;
    mutable BinaryPredicate _predicate;

public:
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

#ifdef LZ_HAS_CONCEPTS

    constexpr unique_iterator()
        requires std::default_initializable<Iterator> && std::default_initializable<S> &&
                     std::default_initializable<BinaryPredicate>
    = default;

#else

    template<class I = Iterator,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<S>::value &&
                               std::is_default_constructible<BinaryPredicate>::value>>
    constexpr unique_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                         std::is_nothrow_default_constructible<S>::value &&
                                         std::is_nothrow_default_constructible<BinaryPredicate>::value) {
    }

#endif

    constexpr unique_iterator(Iterator begin, S end, BinaryPredicate binary_predicate) :
        _iterator{ std::move(begin) },
        _end{ std::move(end) },
        _predicate{ std::move(binary_predicate) } {
    }

    LZ_CONSTEXPR_CXX_14 unique_iterator& operator=(default_sentinel) {
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
        using std::adjacent_find;
        _iterator = adjacent_find(std::move(_iterator), _end, _predicate);

        if (_iterator != _end) {
            ++_iterator;
        }
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const unique_iterator& b) const {
        LZ_ASSERT(_end == b._end, "unique_iterators are not compatible");
        return _iterator == b._iterator;
    }

    constexpr bool eq(default_sentinel) const {
        return _iterator == _end;
    }
};

template<class Iterator, class S, class BinaryPredicate>
class unique_iterator<Iterator, S, BinaryPredicate, enable_if<is_bidi<Iterator>::value>>
    : public iterator<unique_iterator<Iterator, Iterator, BinaryPredicate>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>,
                      diff_type<Iterator>, std::bidirectional_iterator_tag> {

    using traits = std::iterator_traits<Iterator>;

    Iterator _begin{};
    Iterator _iterator{};
    S _end{};
    BinaryPredicate _predicate{};

public:
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

#ifdef LZ_HAS_CONCEPTS

    constexpr unique_iterator()
        requires std::default_initializable<Iterator> && std::default_initializable<S> &&
                     std::default_initializable<BinaryPredicate>
    = default;

#else

    template<class I = Iterator,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<S>::value &&
                               std::is_default_constructible<BinaryPredicate>::value>>
    constexpr unique_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                         std::is_nothrow_default_constructible<S>::value &&
                                         std::is_nothrow_default_constructible<BinaryPredicate>::value) {
    }

#endif

    constexpr unique_iterator(Iterator it, Iterator begin, S end, BinaryPredicate compare) :
        _begin{ std::move(begin) },
        _iterator{ std::move(it) },
        _end{ std::move(end) },
        _predicate{ std::move(compare) } {
    }

    LZ_CONSTEXPR_CXX_14 unique_iterator& operator=(default_sentinel) {
        _iterator = _end;
        return *this;
    }

    constexpr reference dereference() const {
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        using std::adjacent_find;
        _iterator = adjacent_find(std::move(_iterator), _end, _predicate);

        if (_iterator != _end) {
            ++_iterator;
        }
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        --_iterator;
        if (_iterator == _begin) {
            return;
        }

        auto next = _iterator;
        for (--next; next != _begin; --next, --_iterator) {
            if (!_predicate(*next, *_iterator)) {
                return;
            }
        }
        _iterator = std::move(next);
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const unique_iterator& b) const {
        LZ_ASSERT(_end == b._end && _begin == b._begin, "Incompatible iterators");
        return _iterator == b._iterator;
    }

    constexpr bool eq(default_sentinel) const {
        return _iterator == _end;
    }
};
} // namespace detail
} // namespace lz

#endif