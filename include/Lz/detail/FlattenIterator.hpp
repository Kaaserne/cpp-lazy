#pragma once

#ifndef LZ_FLATTEN_ITERATOR_HPP
#define LZ_FLATTEN_ITERATOR_HPP

#include "LzTools.hpp"

namespace lz {
namespace internal {
template<class T, class U = void>
struct AliasWrapper {
    using Type = U;
};

template<class T, class U = void>
using AliasWrapperT = typename AliasWrapper<T, U>::Type;

template<class T, class Enable = void>
struct HasValueType : std::false_type {};

template<class T>
struct HasValueType<T, AliasWrapperT<typename T::value_type>> : std::true_type {};

template<class T, class Enable = void>
struct HasDifferenceType : std::false_type {};

template<class T>
struct HasDifferenceType<T, AliasWrapperT<typename T::difference_type>> : std::true_type {};

template<class T, class Enable = void>
struct HasPointer : std::false_type {};

template<class T>
struct HasPointer<T, AliasWrapperT<typename T::pointer>> : std::true_type {};

template<class T, class Enable = void>
struct HasIterCat : std::false_type {};

template<class T>
struct HasIterCat<T, AliasWrapperT<typename T::iterator_category>> : std::true_type {};

template<class T, class Enable = void>
struct HasReference : std::false_type {};

template<class T>
struct HasReference<T, AliasWrapperT<typename T::reference>> : std::true_type {};

template<class T>
struct IsIterator {
    static constexpr bool value = HasValueType<T>::value && HasDifferenceType<T>::value && HasPointer<T>::value &&
                                  HasReference<T>::value && HasIterCat<T>::value;
};

template<class T, class = void>
struct IterTraitsOrUnderlyingType {
    using Type = T;
};

template<class T>
struct IterTraitsOrUnderlyingType<T, AliasWrapperT<typename T::iterator>> {
    using Type = std::iterator_traits<typename T::iterator>;
};

template<class T, class U = void>
using IterTraitsOrUnderlyingTypeT = typename IterTraitsOrUnderlyingType<T, U>::Type;

template<bool B>
struct CountDimsHelper;

template<>
struct CountDimsHelper<true> {
    template<class T>
    using Inner = IterTraitsOrUnderlyingTypeT<typename T::value_type>;

    template<class T>
    using type = std::integral_constant<int, 1 + CountDimsHelper<IsIterator<Inner<T>>::value>::template type<Inner<T>>::value>;
};

template<>
struct CountDimsHelper<false> {
    template<class>
    using type = std::integral_constant<int, 0>;
};

template<class T>
using CountDims = typename CountDimsHelper<IsIterator<T>::value>::template type<T>;

// Improvement of https://stackoverflow.com/a/21076724/8729023
template<class Iterator>
class FlattenWrapper {
    Iterator _begin{};
    Iterator _current{};
    Iterator _end{};

    using IterTraits = std::iterator_traits<Iterator>;

public:
    using reference = typename IterTraits::reference;
    using pointer = FakePointerProxy<reference>;
    using value_type = typename IterTraits::value_type;
    using iterator_category = typename IterTraits::iterator_category;
    using difference_type = typename IterTraits::difference_type;

    constexpr FlattenWrapper() = default;

    LZ_CONSTEXPR_CXX_20 FlattenWrapper(Iterator current, Iterator begin, Iterator end) :
        _begin(std::move(begin)),
        _current(std::move(current)),
        _end(std::move(end)) {
    }

    LZ_CONSTEXPR_CXX_20 bool hasSome() const noexcept {
        return _current != _end;
    }

    LZ_CONSTEXPR_CXX_20 bool hasPrev() const noexcept {
        return _current != _begin;
    }

    LZ_CONSTEXPR_CXX_20 friend bool operator!=(const FlattenWrapper& a, const FlattenWrapper& b) noexcept {
        return a._current != b._current;
    }

    LZ_CONSTEXPR_CXX_20 friend bool operator==(const FlattenWrapper& a, const FlattenWrapper& b) noexcept {
        return !(a != b); // NOLINT
    }

    LZ_CONSTEXPR_CXX_20 reference operator*() const {
        return *_current;
    }

    LZ_CONSTEXPR_CXX_20 pointer operator->() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 FlattenWrapper& operator++() {
        ++_current;
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 FlattenWrapper operator++(int) {
        FlattenWrapper tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_20 FlattenWrapper& operator--() {
        --_current;
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 FlattenWrapper operator--(int) {
        FlattenWrapper tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_20 difference_type distance() const {
        using lz::distance;
        using std::distance;
        return distance(_current, _end);
    }

    LZ_CONSTEXPR_CXX_20 friend difference_type operator-(const FlattenWrapper&, const FlattenWrapper& b) {
        return b.distance();
    }

    LZ_CONSTEXPR_CXX_20 FlattenWrapper operator+(const difference_type offset) const {
        using lz::next;
        using std::next;
        FlattenWrapper tmp(*this);
        tmp._current = next(std::move(tmp._current), offset);
        return tmp;
    }
};

template<class Iterator, int N>
class FlattenIterator {
    using Inner = FlattenIterator<decltype(std::begin(*std::declval<Iterator>())), N - 1>;

public:
    using reference = typename Inner::reference;
    using pointer = typename Inner::pointer;
    using value_type = typename Inner::value_type;
    using iterator_category = typename Inner::iterator_category;
    using difference_type = typename Inner::difference_type;

private:
    LZ_CONSTEXPR_CXX_20 void advance() {
        if (_innerIter.hasSome()) {
            return;
        }
        for (++_outerIter; _outerIter.hasSome(); ++_outerIter) {
            const auto begin = std::begin(*_outerIter);
            _innerIter = { begin, begin, std::end(*_outerIter) };
            if (_innerIter.hasSome()) {
                return;
            }
        }
        _innerIter = {};
    }

    FlattenWrapper<Iterator> _outerIter{};
    Inner _innerIter{};

public:
    constexpr FlattenIterator() = default;

    LZ_CONSTEXPR_CXX_20 FlattenIterator(Iterator it, Iterator begin, Iterator end) :
        _outerIter(std::move(it), std::move(begin), std::move(end)) {
        if (_outerIter.hasSome()) {
            const auto beg = std::begin(*_outerIter);
            _innerIter = { beg, beg, std::end(*_outerIter) };
            this->advance();
        }
    }

    LZ_CONSTEXPR_CXX_20 bool hasSome() const {
        return _outerIter.hasSome();
    }

    LZ_CONSTEXPR_CXX_20 bool hasPrev() const {
        return _innerIter.hasPrev() || _outerIter.hasPrev();
    }

    LZ_CONSTEXPR_CXX_20 friend bool operator!=(const FlattenIterator& a, const FlattenIterator& b) noexcept {
        return a._outerIter != b._outerIter || a._innerIter != b._innerIter;
    }

    LZ_CONSTEXPR_CXX_20 friend bool operator==(const FlattenIterator& a, const FlattenIterator& b) noexcept {
        return !(a != b); // NOLINT
    }

    LZ_CONSTEXPR_CXX_20 reference operator*() const {
        return *_innerIter;
    }

    LZ_CONSTEXPR_CXX_20 pointer operator->() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 FlattenIterator& operator++() {
        ++_innerIter;
        this->advance();
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 FlattenIterator operator++(int) {
        FlattenIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_20 FlattenIterator& operator--() {
        if (_innerIter.hasPrev()) {
            --_innerIter;
            return *this;
        }
        while (_outerIter.hasPrev()) {
            --_outerIter;
            const auto end = std::end(*_outerIter);
            _innerIter = { end, std::begin(*_outerIter), end };
            if (_innerIter.hasPrev()) {
                --_innerIter;
                return *this;
            }
        }
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 FlattenIterator operator--(int) {
        FlattenIterator tmp(*this);
        --*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_20 difference_type distance() const {
        FlattenIterator tmp(*this);
        if (!tmp._outerIter.hasSome()) {
            return 0;
        }
        difference_type total = 0;
        for (;;) {
            total += tmp._innerIter.distance();
            ++tmp._outerIter;
            if (!tmp._outerIter.hasSome()) {
                break;
            }
            const auto begin = std::begin(*tmp._outerIter);
            tmp._innerIter = { begin, begin, std::end(*tmp._outerIter) };
        }
        return total;
    }

    LZ_CONSTEXPR_CXX_20 friend difference_type operator-(const FlattenIterator&, const FlattenIterator& b) {
        return b.distance();
    }

    LZ_CONSTEXPR_CXX_20 FlattenIterator operator+(difference_type offset) const {
        FlattenIterator tmp(*this);
        if (!tmp._outerIter.hasSome()) {
            return tmp;
        }
        auto dist = tmp._innerIter.distance();
        while (offset >= dist) {
            ++tmp._outerIter;
            offset -= dist;
            if (!tmp._outerIter.hasSome()) {
                tmp._innerIter = {};
                break;
            }
            const auto begin = std::begin(*tmp._outerIter);
            tmp._innerIter = { begin, begin, std::end(*tmp._outerIter) };
            dist = tmp._innerIter.distance();
        }
        if (!tmp._outerIter.hasSome() && !tmp._innerIter.hasSome()) {
            LZ_ASSERT(offset == 0, "cannot access elements after end");
            tmp._innerIter = {};
            return tmp;
        }
        tmp._innerIter = tmp._innerIter + offset;
        return tmp;
    }
};

template<class Iterator>
class FlattenIterator<Iterator, 0> {
    FlattenWrapper<Iterator> _range;
    using Traits = std::iterator_traits<Iterator>;

public:
    using pointer = typename Traits::pointer;
    using reference = typename Traits::reference;
    using value_type = typename Traits::value_type;
    using iterator_category = typename Traits::iterator_category;
    using difference_type = typename Traits::difference_type;

    constexpr FlattenIterator() = default;

    constexpr FlattenIterator(Iterator it, Iterator begin, Iterator end) :
        _range(std::move(it), std::move(begin), std::move(end)) {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator*() const {
        return *_range;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer operator->() const {
        return &*_range;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator!=(const FlattenIterator& a, const FlattenIterator& b) noexcept {
        return a._range != b._range;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator==(const FlattenIterator& a, const FlattenIterator& b) noexcept {
        return !(a != b); // NOLINT
    }

    LZ_CONSTEXPR_CXX_20 FlattenIterator& operator++() {
        ++_range;
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 FlattenIterator operator++(int) {
        FlattenIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_20 FlattenIterator& operator--() {
        --_range;
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 FlattenIterator operator--(int) {
        FlattenIterator tmp(*this);
        --*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_20 difference_type distance() const {
        return _range.distance();
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend difference_type operator-(const FlattenIterator&, const FlattenIterator& b) {
        return b._range.distance();
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 FlattenIterator operator+(const difference_type offset) const {
        FlattenIterator tmp(*this);
        tmp._range = tmp._range + offset;
        return tmp;
    }
};
} // namespace internal

/**
 * Gets the distance of the iterator. Please note that the first argument *must* be created from `flatten(...).begin()`, not
 * `flatten(...).end()`. The second argument is not used. Distance is O(N), where N is the amount of inner containers.
 * (this is only the case for random access iterators, or iterators that define a custom `distance`).
 * @param begin The flatten iterator created from `lz::flatten(...).begin()`
 * @return The distance (size/length) of the iterator.
 */
template<LZ_CONCEPT_ITERATOR Iterator, int N>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 typename internal::FlattenIterator<Iterator, N>::difference_type
distance(const internal::FlattenIterator<Iterator, N>& begin, const internal::FlattenIterator<Iterator, N>&) {
    return begin.distance();
}

/**
 * Gets the nth value of the iterator. If value >= 0, this function is O(N), where N is the amount of inner containers.
 * (this is only the case for random access iterators, or iterators that define a custom `distance`). Otherwise this function is
 * O(N * M) where M is the amount of elements inside that dimension and N is the amount of inner containers.
 * @param iter The iterator to increment.
 * @param value The amount to increment
 * @return A flatten iterator with iter + value.
 */
template<LZ_CONCEPT_ITERATOR Iterator, int N>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::FlattenIterator<Iterator, N>
next(const internal::FlattenIterator<Iterator, N>& iter, const internal::DiffType<internal::FlattenIterator<Iterator, N>> value) {
    if (value < 0) {
        return std::next(iter, value);
    }
    return iter + value;
}
} // namespace lz

#endif // LZ_FLATTEN_ITERATOR_HPP
