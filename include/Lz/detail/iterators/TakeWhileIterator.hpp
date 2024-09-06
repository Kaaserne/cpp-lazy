#pragma once

#ifndef LZ_TAKE_WHILE_ITERATOR_HPP
#define LZ_TAKE_WHILE_ITERATOR_HPP

#include "Lz/detail/CompilerChecks.hpp"
#include "Lz/detail/FakePointerProxy.hpp"
#include "Lz/detail/FunctionContainer.hpp"

namespace lz {
namespace detail {
template<class Iterator, class UnaryPredicate>
class TakeWhileIterator {
    Iterator _iterator{};
    Iterator _end{};
    FunctionContainer<UnaryPredicate> _unaryPredicate{};

    using IterTraits = std::iterator_traits<Iterator>;

    void incrementedCheck() {
        if (_iterator != _end && !_unaryPredicate(*_iterator)) {
            _iterator = _end;
        }
    }

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = typename IterTraits::value_type;
    using difference_type = typename IterTraits::difference_type;
    using reference = typename IterTraits::reference;
    using pointer = FakePointerProxy<reference>;

    constexpr TakeWhileIterator() = default;

    LZ_CONSTEXPR_CXX_14 TakeWhileIterator(Iterator iterator, Iterator end, UnaryPredicate unaryPredicate) :
        _iterator(std::move(iterator)),
        _end(std::move(end)),
        _unaryPredicate(std::move(unaryPredicate)) {
        incrementedCheck();
    }

    LZ_CONSTEXPR_CXX_20 TakeWhileIterator& operator++() {
        ++_iterator;
        incrementedCheck();
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 TakeWhileIterator operator++(int) {
        TakeWhileIterator tmp = *this;
        ++*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_20 reference operator*() const {
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_20 pointer operator->() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 friend bool operator!=(const TakeWhileIterator& a, const TakeWhileIterator& b) noexcept {
        return a._iterator != b._iterator;
    }

    LZ_CONSTEXPR_CXX_20 friend bool operator==(const TakeWhileIterator& a, const TakeWhileIterator& b) noexcept {
        return !(a != b);
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_TAKE_WHILE_ITERATOR_HPP