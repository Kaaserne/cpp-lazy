#pragma once

#ifndef LZ_UNIQUE_ITERATOR_HPP
#define LZ_UNIQUE_ITERATOR_HPP

#include "FunctionContainer.hpp"
#include "LzTools.hpp"

#include <algorithm>

namespace lz {
namespace internal {
#ifdef LZ_HAS_EXECUTION
template<class Execution, LZ_CONCEPT_ITERATOR Iterator, class Compare>
#else  // ^^^ lz has execution vvv ! lz has execution
template<LZ_CONCEPT_ITERATOR Iterator, class Compare>
#endif // LZ_HAS_EXECUTION
class UniqueIterator {
    using IterTraits = std::iterator_traits<Iterator>;

    Iterator _iterator{};
    Iterator _end{};
    mutable FunctionContainer<Compare> _compare{};
#ifdef LZ_HAS_EXECUTION
    Execution _execution;
#endif // LZ_HAS_EXECUTION

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = typename IterTraits::value_type;
    using difference_type = typename IterTraits::difference_type;
    using reference = typename IterTraits::reference;
    using pointer = FakePointerProxy<reference>;

#ifdef LZ_HAS_EXECUTION
    constexpr UniqueIterator(Iterator begin, Iterator end, Compare compare, Execution execution)
#else  // ^^^ lz has execution vvv ! lz has execution
    constexpr UniqueIterator(Iterator begin, Iterator end, Compare compare)
#endif // LZ_HAS_EXECUTION
        :
        _iterator(std::move(begin)),
        _end(std::move(end)),
        _compare(std::move(compare))
#ifdef LZ_HAS_EXECUTION
        ,
        _execution(execution)
#endif // LZ_HAS_EXECUTION
    {
    }

    constexpr UniqueIterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator*() const {
        return *_iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer operator->() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 UniqueIterator& operator++() {
#ifdef LZ_HAS_EXECUTION
        if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
            _iterator = std::adjacent_find(std::move(_iterator), _end, _compare);
        }
        else {
            _iterator = std::adjacent_find(_execution, std::move(_iterator), _end, _compare);
        }
#else  // ^^^ lz has execution vvv ! lz has execution
        _iterator = std::adjacent_find(std::move(_iterator), _end, _compare);
#endif // LZ_HAS_EXECUTION

        if (_iterator != _end) {
            ++_iterator;
        }
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 UniqueIterator operator++(int) {
        UniqueIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator!=(const UniqueIterator& a, const UniqueIterator& b) noexcept {
        return a._iterator != b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator==(const UniqueIterator& a, const UniqueIterator& b) noexcept {
        return !(a != b); // NOLINT
    }
};
} // namespace internal
} // namespace lz

#endif