#pragma once

#ifndef LZ_INTERSECTION_ITERATOR_HPP
#define LZ_INTERSECTION_ITERATOR_HPP

namespace lz {
namespace detail {
template<class Iterator, class S, class Iterator2, class S2, class Compare>
class intersection_iterator {
    Iterator _iterator;
    Iterator2 _iterator2;
    S _end;
    S2 _end2;
    Compare _compare;
    // TODO
public:
    constexpr intersection_iterator() = default;

    constexpr intersection_iterator(Iterator iterator, S end, Iterator2 iterator2, S2 end2, Compare compare) :
        _iterator{ iterator },
        _iterator2{ iterator2 },
        _end{ end },
        _end2{ end2 },
        _compare{ compare } {
        if (_iterator != _end && _iterator2 != _end2) {
            if (_compare(*_iterator, *_iterator2)) {
                ++(*this);
            }
            else if (_compare(*_iterator2, *_iterator)) {
                ++_iterator2;
            }
        }
    }
};
}
} // namespace lz

#endif // LZ_INTERSECTION_ITERATOR_HPP
