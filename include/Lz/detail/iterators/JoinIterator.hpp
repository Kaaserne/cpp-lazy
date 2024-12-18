#pragma once

#ifndef LZ_JOIN_ITERATOR_HPP
#define LZ_JOIN_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/CompilerChecks.hpp"
#include "Lz/detail/FakePointerProxy.hpp"
#include "Lz/detail/Traits.hpp"

#if defined(LZ_STANDALONE)
#ifdef LZ_HAS_FORMAT
#include <format>
#else
#include <sstream>

#endif // LZ_HAS_FORMAT
#endif // LZ_STANDALONE

namespace lz {
namespace detail {
#if defined(LZ_STANDALONE) && (!defined(LZ_HAS_FORMAT))
template<class T>
EnableIf<!std::is_arithmetic<T>::value, std::string> toString(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

template<class T>
EnableIf<std::is_arithmetic<T>::value, std::string> toString(const T value) {
    char buff[SafeBufferSize<T>::value]{};
    toStringFromBuff(value, buff);
    return buff;
}

inline std::string toString(const bool value) {
    char buff[SafeBufferSize<bool>::value]{};
    toStringFromBuff(value, buff);
    return buff;
}
#endif // defined(LZ_STANDALONE) && (!defined(LZ_HAS_FORMAT))

template<class Iterator>
class JoinIterator
    : public IterBase<
          JoinIterator<Iterator>,
          Conditional<std::is_same<std::string, ValueType<Iterator>>::value, RefType<Iterator>, std::string>,
          FakePointerProxy<Conditional<std::is_same<std::string, ValueType<Iterator>>::value, RefType<Iterator>, std::string>>,
          DiffType<Iterator>, IterCat<Iterator>> {

    using IterTraits = std::iterator_traits<Iterator>;
    using ContainerType = typename IterTraits::value_type;

public:
    using value_type = std::string;
    using iterator_category = typename IterTraits::iterator_category;
    using difference_type = typename IterTraits::difference_type;
    using reference = Conditional<std::is_same<std::string, ContainerType>::value, typename IterTraits::reference, std::string>;
    using pointer = FakePointerProxy<reference>;

private:
    Iterator _iterator{};
    mutable std::string _delimiter{};
#if defined(LZ_HAS_FORMAT) || !defined(LZ_STANDALONE)
    std::string _fmt{};
#endif
    mutable bool _isIteratorTurn{ true };

public:
    const std::string& _getDelimiter() const {
        return _delimiter;
    }

    Iterator _getIterator() const {
        return _iterator;
    }

private:
    template<class T = ContainerType>
    EnableIf<!std::is_same<T, std::string>::value, reference> deref() const {
        if (_isIteratorTurn) {
#ifdef LZ_STANDALONE
#ifdef LZ_HAS_FORMAT
            return std::vformat(_fmt.c_str(), std::make_format_args(*_iterator));
#else
            return toString(*_iterator);
#endif // LZ_HAS_FORMAT
#else
#if defined(LZ_HAS_CXX_20) && FMT_VERSION >= 90000
            return fmt::format(fmt::runtime(_fmt.c_str()), *_iterator);
#else
            return fmt::format(_fmt.c_str(), *_iterator);
#endif // LZ_HAS_CXX_20 && FMT_VERSION >= 90000
#endif // LZ_STANDALONE
        }
        return _delimiter;
    }

    template<class T = ContainerType>
    LZ_CONSTEXPR_CXX_20 EnableIf<std::is_same<T, std::string>::value, reference> deref() const {
        if (_isIteratorTurn) {
            return *_iterator;
        }
        return _delimiter;
    }

    template<class T = ContainerType>
    LZ_CONSTEXPR_CXX_20 EnableIf<std::is_same<T, std::string>::value, reference>
    indexOperator(const difference_type offset) const {
        // If we use *(*this + offset) when a delimiter must be returned, then we get a segfault because the operator+ returns a
        // copy of the delimiter
        if (_isIteratorTurn && isEven(offset)) {
            return *(*this + offset);
        }
        return _delimiter;
    }

    template<class T = ContainerType>
    LZ_CONSTEXPR_CXX_20 EnableIf<!std::is_same<T, std::string>::value, reference>
    indexOperator(const difference_type offset) const {
        return *(*this + offset);
    }

public:
#if defined(LZ_HAS_FORMAT) || !defined(LZ_STANDALONE)
    LZ_CONSTEXPR_CXX_20
    JoinIterator(Iterator iterator, std::string delimiter, std::string fmt, const bool isIteratorTurn) :
        _iterator(std::move(iterator)),
        _delimiter(std::move(delimiter)),
        _fmt(std::move(fmt)),
        _isIteratorTurn(isIteratorTurn) {
    }
#else
    LZ_CONSTEXPR_CXX_20
    JoinIterator(Iterator iterator, std::string delimiter, const bool isIteratorTurn) :
        _iterator(std::move(iterator)),
        _delimiter(std::move(delimiter)),
        _isIteratorTurn(isIteratorTurn) {
    }
#endif // has format

    JoinIterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return deref();
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        if (_isIteratorTurn) {
            ++_iterator;
        }
        _isIteratorTurn = !_isIteratorTurn;
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        _isIteratorTurn = !_isIteratorTurn;
        if (_isIteratorTurn) {
            --_iterator;
        }
    }

    LZ_CONSTEXPR_CXX_20 void plusIs(const difference_type offset) {
        _iterator += offset < 0 ? roundEven<difference_type>(offset * -1, static_cast<difference_type>(2)) * -1
                                : roundEven<difference_type>(offset, static_cast<difference_type>(2));
        if (!isEven(offset)) {
            _isIteratorTurn = !_isIteratorTurn;
        }
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 difference_type difference(const JoinIterator& b) const {
        LZ_ASSERT(_delimiter == b._delimiter, "incompatible iterator types: found different delimiters");
        // distance * 2 for delimiter, - 1 for removing last delimiter
        return (_iterator - b._iterator) * 2 - 1;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const JoinIterator& b) const noexcept {
        LZ_ASSERT(_delimiter == b._delimiter, "incompatible iterator types: found different delimiters");
        return _iterator == b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator[](const difference_type offset) const {
        return indexOperator(offset);
    }
};
} // namespace detail
} // namespace lz

#endif