#pragma once

#ifndef LZ_FILTER_ITERATOR_HPP
#define LZ_FILTER_ITERATOR_HPP

#include <Lz/detail/algorithm.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/iterator_base.hpp>
#include <algorithm>

namespace lz {
namespace detail {
template<class Iterator, class S, class UnaryPredicate>
class filter_iterator;

template<class Iterator, class S, class UnaryPredicate>
class filter_iterator
    : public iterator<filter_iterator<Iterator, S, UnaryPredicate>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>,
                      diff_type<Iterator>, common_type<std::bidirectional_iterator_tag, iter_cat_t<Iterator>>,
                      sentinel_selector<iter_cat_t<Iterator>, filter_iterator<Iterator, S, UnaryPredicate>>> {

    using traits = std::iterator_traits<Iterator>;

public:
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

    LZ_CONSTEXPR_CXX_14 Iterator find(Iterator first, S last) {
        using detail::find_if;
        using std::find_if;
        return find_if(std::move(first), std::move(last), _predicate);
    }

private:
    Iterator _begin{};
    Iterator _iterator{};
    S _end{};
    mutable UnaryPredicate _predicate{};

public:
    LZ_CONSTEXPR_CXX_14 filter_iterator(Iterator iterator, Iterator begin, S end, UnaryPredicate up) :
        _begin{ std::move(begin) },
        _iterator{ std::move(iterator) },
        _end{ std::move(end) },
        _predicate{ std::move(up) } {
        if (_iterator == _begin) {
            _iterator = find(std::move(_iterator), _end);
        }
    }

    constexpr filter_iterator() = default;

    constexpr reference dereference() const {
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        ++_iterator;
        _iterator = find(std::move(_iterator), _end);
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        do {
            --_iterator;
        } while (!_predicate(*_iterator) && _iterator != _begin);
    }

    constexpr bool eq(const filter_iterator& b) const noexcept {
        return _iterator == b._iterator;
    }

    constexpr bool eq(default_sentinel) const noexcept {
        return _iterator == _end;
    }
};
} // namespace detail
} // namespace lz

#endif