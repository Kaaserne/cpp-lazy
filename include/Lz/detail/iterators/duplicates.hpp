#ifndef LZ_DUPLICATES_ITERATOR_HPP
#define LZ_DUPLICATES_ITERATOR_HPP

#include "Lz/detail/compiler_checks.hpp"

#include <Lz/detail/algorithm.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/iterator_base.hpp>
#include <iterator>

namespace lz {
namespace detail {
template<class Iterator, class S, class BinaryPredicate, class = void>
class duplicates_iterator;

template<class Iterator, class S, class BinaryPredicate>
class duplicates_iterator<Iterator, S, BinaryPredicate, enable_if<is_ra<Iterator>::value>>
    : public lz::iterator<duplicates_iterator<Iterator, S, BinaryPredicate>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>,
                          diff_type<Iterator>, std::bidirectional_iterator_tag,
                          duplicates_iterator<Iterator, S, BinaryPredicate>> {
    using traits = std::iterator_traits<Iterator>;

public:
    using value_type = typename traits::value_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = typename traits::difference_type;

private:
    Iterator _begin;
    Iterator _iterator;
    S _end;
    BinaryPredicate _compare;

    LZ_CONSTEXPR_CXX_14 void next() {
        while (_iterator != _end) {
            auto next_it = std::next(_iterator);
            if (next_it != _end && !_compare(*_iterator, *next_it)) {
                break;
            }
            if (_iterator != _begin) {
                auto prev_it = std::prev(_iterator);
                if (!_compare(*prev_it, *_iterator)) {
                    break;
                }
            }
            _iterator = std::move(next_it);
        }
    }

public:
    LZ_CONSTEXPR_CXX_14 duplicates_iterator(Iterator it, Iterator begin, S end, BinaryPredicate compare) :
        _begin{ std::move(begin) },
        _iterator{ std::move(it) },
        _end{ std::move(end) },
        _compare{ std::move(compare) } {
        next();
    }

    LZ_CONSTEXPR_CXX_14 duplicates_iterator& operator=(default_sentinel) {
        _iterator = _end;
        return *this;
    }

    constexpr reference dereference() const {
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<reference>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        ++_iterator;
        next();
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        --_iterator;
        while (_iterator != _begin) {
            auto prev_it = std::prev(_iterator);
            if (!_compare(*prev_it, *_iterator)) {
                break;
            }
            auto next_it = std::next(_iterator);
            if (next_it != _end && !_compare(*_iterator, *next_it)) {
                break;
            }
            --_iterator;
        }
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool eq(const duplicates_iterator& other) const {
        LZ_ASSERT(_begin == other._begin && _end == other._end, "Incompatible iterators");
        return _iterator == other._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool eq(default_sentinel) const {
        return _iterator == _end;
    }
};

template<class Iterator, class S, class BinaryPredicate>
class duplicates_iterator<Iterator, S, BinaryPredicate, enable_if<!is_ra<Iterator>::value && is_bidi<Iterator>::value>>
    : public lz::iterator<duplicates_iterator<Iterator, S, BinaryPredicate>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>,
                          diff_type<Iterator>, std::bidirectional_iterator_tag,
                          duplicates_iterator<Iterator, S, BinaryPredicate>> {
    using traits = std::iterator_traits<Iterator>;

public:
    using value_type = typename traits::value_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = typename traits::difference_type;

private:
    Iterator _begin;
    Iterator _iterator;
    Iterator _neighbour;
    S _end;
    BinaryPredicate _compare;

    LZ_CONSTEXPR_CXX_14 void next() {
        while (_iterator != _end) {
            auto next_it = std::next(_iterator);
            if (next_it != _end && !_compare(*_iterator, *next_it)) {
                break;
            }
            if (_neighbour != _end && !_compare(*_neighbour, *_iterator)) {
                break;
            }
            _neighbour = _iterator;
            _iterator = std::move(next_it);
        }
    }

public:
    LZ_CONSTEXPR_CXX_14 duplicates_iterator(Iterator it, Iterator begin, S end, BinaryPredicate compare) :
        _begin{ std::move(begin) },
        _iterator{ std::move(it) },
        _neighbour{ _iterator },
        _end{ std::move(end) },
        _compare{ std::move(compare) } {
        _neighbour = _end;
        next();
    }

    LZ_CONSTEXPR_CXX_14 duplicates_iterator& operator=(default_sentinel) {
        _iterator = _end;
        return *this;
    }

    constexpr reference dereference() const {
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<reference>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        _neighbour = _iterator++;
        next();
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        _neighbour = _iterator--;
        while (_iterator != _begin) {
            auto prev_it = std::prev(_iterator);
            if (!_compare(*prev_it, *_iterator)) {
                break;
            }
            if (_neighbour != _end && !_compare(*_iterator, *_neighbour)) {
                break;
            }
            _neighbour = _iterator;
            --_iterator;
        }
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool eq(const duplicates_iterator& other) const {
        LZ_ASSERT(_begin == other._begin && _end == other._end, "Incompatible iterators");
        return _iterator == other._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool eq(default_sentinel) const {
        return _iterator == _end;
    }
};

template<class Iterator, class S, class BinaryPredicate>
class duplicates_iterator<Iterator, S, BinaryPredicate, enable_if<!is_bidi<Iterator>::value>>
    : public lz::iterator<duplicates_iterator<Iterator, S, BinaryPredicate>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>,
                          diff_type<Iterator>, iter_cat_t<Iterator>, default_sentinel> {
    using traits = std::iterator_traits<Iterator>;

public:
    using value_type = typename traits::value_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = typename traits::difference_type;

private:
    Iterator _iterator;
    Iterator _prev_it;
    S _end;
    BinaryPredicate _compare;

    LZ_CONSTEXPR_CXX_14 void next() {
        while (_iterator != _end) {
            auto next_it = std::next(_iterator);
            if (next_it != _end && !_compare(*_iterator, *next_it)) {
                break;
            }
            if (_prev_it != _end && !_compare(*_prev_it, *_iterator)) {
                break;
            }
            _prev_it = _iterator;
            _iterator = std::move(next_it);
        }
    }

public:
    LZ_CONSTEXPR_CXX_14 duplicates_iterator(Iterator it, S end, BinaryPredicate compare) :
        _iterator{ std::move(it) },
        _prev_it{ _iterator },
        _end{ std::move(end) },
        _compare{ std::move(compare) } {
        _prev_it = _end;
        next();
    }

    LZ_CONSTEXPR_CXX_14 duplicates_iterator& operator=(default_sentinel) {
        _iterator = _end;
        return *this;
    }

    constexpr reference dereference() const {
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<reference>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        _prev_it = _iterator++;
        next();
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool eq(const duplicates_iterator& other) const {
        LZ_ASSERT(_end == other._end, "Incompatible iterators");
        return _iterator == other._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool eq(default_sentinel) const {
        return _iterator == _end;
    }
};
} // namespace detail
} // namespace lz

#endif
