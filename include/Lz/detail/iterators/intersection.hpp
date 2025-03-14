#pragma once

#ifndef LZ_INTERSECTION_ITERATOR_HPP
#define LZ_INTERSECTION_ITERATOR_HPP

#include <Lz/algorithm.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/traits.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {
template<class I>
using iter_cat_intersection = common_type<std::bidirectional_iterator_tag, iter_cat_t<I>>;

template<class Iterator, class S, class Iterator2, class S2, class BinaryPredicate, class = void>
class intersection_iterator;

template<class Iterator, class S, class Iterator2, class S2, class BinaryPredicate>
class intersection_iterator<Iterator, S, Iterator2, S2, BinaryPredicate,
                            enable_if<!is_bidi<Iterator>::value || !is_bidi<Iterator2>::value>>
    : public iterator<intersection_iterator<Iterator, S, Iterator2, S2, BinaryPredicate>, ref_t<Iterator>,
                      fake_ptr_proxy<ref_t<Iterator>>, diff_type<Iterator>,
                      common_type<iter_cat_t<Iterator>, iter_cat_t<Iterator2>>, default_sentinel> {
    Iterator _iterator;
    Iterator2 _iterator2;
    S _end;
    S2 _end2;
    BinaryPredicate _compare;

    using iter_traits = std::iterator_traits<Iterator>;

    void find_next() {
        using detail::find_if;
        using std::find_if;

        _iterator = find_if(std::move(_iterator), _end, [this](const auto& value) {
            while (_iterator2 != _end2) {
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

    constexpr intersection_iterator() = default;

    constexpr intersection_iterator(Iterator iterator, S end, Iterator2 iterator2, S2 end2, BinaryPredicate compare) :
        _iterator{ std::move(iterator) },
        _iterator2{ std::move(iterator2) },
        _end{ std::move(end) },
        _end2{ std::move(end2) },
        _compare{ std::move(compare) } {
        find_next();
    }

    constexpr reference dereference() const {
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        ++_iterator2;
        ++_iterator;
        find_next();
    }

    constexpr bool eq(const intersection_iterator& other) const {
        LZ_ASSERT(_end == other._end && _end2 == other._end2, "Incompatible iterators");
        return _iterator == other._iterator;
    }

    constexpr bool eq(default_sentinel) const {
        return _iterator == _end;
    }
};

template<class Iterator, class S, class Iterator2, class S2, class BinaryPredicate>
class intersection_iterator<Iterator, S, Iterator2, S2, BinaryPredicate,
                            enable_if<is_bidi<Iterator>::value && is_bidi<Iterator2>::value>>
    : public iterator<intersection_iterator<Iterator, S, Iterator2, S2, BinaryPredicate>, ref_t<Iterator>,
                      fake_ptr_proxy<ref_t<Iterator>>, diff_type<Iterator>, std::bidirectional_iterator_tag,
                      intersection_iterator<Iterator, S, Iterator2, S2, BinaryPredicate>> {
    Iterator _iterator;
    Iterator2 _iterator2;
    Iterator _begin;
    Iterator2 _begin2;
    S _end;
    S2 _end2;
    BinaryPredicate _compare;

    using iter_traits = std::iterator_traits<Iterator>;

    void find_next() {
        using detail::find_if;
        using std::find_if;

        _iterator = find_if(std::move(_iterator), _end, [this](const auto& value) {
            while (_iterator2 != _end2) {
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

    constexpr intersection_iterator() = default;

    constexpr intersection_iterator(Iterator iterator, Iterator begin, S end, Iterator2 iterator2, Iterator2 begin2, S2 end2,
                                    BinaryPredicate compare) :
        _iterator{ std::move(iterator) },
        _iterator2{ std::move(iterator2) },
        _begin{ std::move(begin) },
        _begin2{ std::move(begin2) },
        _end{ std::move(end) },
        _end2{ std::move(end2) },
        _compare{ std::move(compare) } {
        find_next();
    }

    constexpr reference dereference() const {
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        ++_iterator2;
        ++_iterator;
        find_next();
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        --_iterator2;
        --_iterator;

        while (_iterator2 != _begin2 && _iterator != _begin) {
            if (_compare(*_iterator, *_iterator2)) {
                --_iterator2;
                continue;
            }
            if (!_compare(*_iterator2, *_iterator)) {
                return;
            }
            --_iterator;
        }
        if (_iterator2 == _begin2) {
            _iterator = _begin;
        }
    }

    constexpr bool eq(const intersection_iterator& other) const {
        LZ_ASSERT(_end == other._end && _end2 == other._end2 && _begin == other._begin && _begin2 == other._begin2,
                  "Incompatible iterators");
        return _iterator == other._iterator;
    }

    constexpr bool eq(default_sentinel) const noexcept {
        return _iterator == _end;
    }
};
}
} // namespace lz

#endif // LZ_INTERSECTION_ITERATOR_HPP
