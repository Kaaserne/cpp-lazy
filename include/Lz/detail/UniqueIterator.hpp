#pragma once

#ifndef LZ_UNIQUE_ITERATOR_HPP
#define LZ_UNIQUE_ITERATOR_HPP

#include <algorithm>


#include "LzTools.hpp"


namespace lz { namespace internal {
#ifdef LZ_HAS_EXECUTION
    template<class Execution, LZ_CONCEPT_ITERATOR Iterator>
#else // ^^^ lz has execution vvv ! lz has execution
    template<LZ_CONCEPT_ITERATOR Iterator>
#endif // end lz has execution
    class UniqueIterator {
        using IterTraits = std::iterator_traits<Iterator>;

        Iterator _iterator{};
        Iterator _end{};
#ifdef LZ_HAS_EXECUTION
        Execution _execution;
#endif // end lz has execution

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = typename IterTraits::value_type;
        using difference_type = typename IterTraits::difference_type;
        using pointer = typename IterTraits::pointer;
        using reference = typename IterTraits::reference;

#ifdef LZ_HAS_EXECUTION
        UniqueIterator(Iterator begin, Iterator end, const Execution execution)
#else // ^^^ lz has execution vvv ! lz has execution
        UniqueIterator(Iterator begin, Iterator end)
#endif // end lz has execution
        :
            _iterator(std::move(begin)),
            _end(std::move(end))
#ifdef LZ_HAS_EXECUTION
            , _execution(execution)
#endif // end lz has execution
        {
            if (_iterator == _end) {
                return;
            }

#ifdef LZ_HAS_EXECUTION
            if (std::is_sorted(_execution, _iterator, _end)) {
                return;
            }
            std::sort(_execution, _iterator, _end);
#else // ^^^ lz has execution vvv ! lz has execution
            if (std::is_sorted(_iterator, _end)) {
                return;
            }
            std::sort(_iterator, _end);
#endif
        }

        UniqueIterator() = default;

        reference operator*() const {
            return *_iterator;
        }

        pointer operator->() const {
            return &*_iterator;
        }

        UniqueIterator& operator++() {
#ifdef LZ_HAS_EXECUTION
            if constexpr (IsSequencedPolicyV<Execution>) {
                _iterator = std::adjacent_find(_iterator, _end, std::less<value_type>());
            }
            else {
                _iterator = std::adjacent_find(_execution, _iterator, _end, std::less<>());
            }
#else // ^^^ lz has execution vvv ! lz has execution
            _iterator = std::adjacent_find(_iterator, _end, std::less<value_type>());
#endif // end lz has execution

            if (_iterator != _end) {
                ++_iterator;
            }
            return *this;
        }

        UniqueIterator operator++(int) {
            UniqueIterator tmp(*this);
            ++*this;
            return tmp;
        }

        friend bool operator!=(const UniqueIterator& a, const UniqueIterator& b) {
            return a._iterator != b._iterator;
        }

        friend bool operator==(const UniqueIterator& a, const UniqueIterator& b) {
            return !(a != b); // NOLINT
        }
    };
}}

#endif