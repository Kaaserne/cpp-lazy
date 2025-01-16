#pragma once

#ifndef LZ_OPTIONAL_HPP
#define LZ_OPTIONAL_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <type_traits>

#ifdef __cpp_lib_optional
#include <optional>
#endif // __cpp_lib_optional

namespace lz {
#ifdef __cpp_lib_optional
template<class T>
using optional = std::optional<T>;

using nullopt_t = std::nullopt_t;

constexpr inline nullopt_t nullopt = std::nullopt;
#else

struct nullopt_t {
    struct init {};
    constexpr nullopt_t(init) noexcept {
    }
};

constexpr nullopt_t nullopt{ nullopt_t::init{} };

template<class T>
class optional {
    static_assert(!std::is_array<T>::value && std::is_object<T>::value, "T may not be an array and must be an object");

    union {
        typename std::remove_const<T>::type _value;
        std::uint8_t _dummy;
    };
    bool _has_value;

    template<class U>
    void construct(U&& obj) noexcept(noexcept(::new(static_cast<void*>(std::addressof(_value))) T(std::forward<U>(obj)))) {
        ::new (static_cast<void*>(std::addressof(_value))) T(std::forward<U>(obj));
        _has_value = true;
    }

public:
    constexpr optional() noexcept : optional(nullopt) {
    }

    constexpr optional(nullopt_t) noexcept : _has_value(false) {
    }

    template<class U = T>
    constexpr optional(U&& value) noexcept : _value(std::forward<U>(value)), _has_value(true) {
    }

    LZ_CONSTEXPR_CXX_14 optional(optional<T>&& right) noexcept {
        if (right) {
            construct(std::move(right.value()));
        }
        else {
            _has_value = false;
            right._has_value = false;
        }
    }

    LZ_CONSTEXPR_CXX_14 optional(const optional<T>& right) noexcept {
        if (right) {
            construct(right.value());
        }
    }

    ~optional() {
        if LZ_CONSTEXPR_IF (std::is_trivially_destructible<T>::value) {
            _has_value = false;
            return;
        }
        if (_has_value) {
            _has_value = false;
            _value.~T();
        }
    }

    template<class U = T>
    LZ_CONSTEXPR_CXX_14 optional& operator=(U&& value) noexcept {
        if (_has_value) {
            _value = std::forward<U>(value);
        }
        else {
            construct(std::forward<U>(value));
        }
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 optional& operator=(const optional<T>& value) noexcept {
        if (_has_value && value) {
            _value = *value;
        }
        else if (!value) {
            _has_value = false;
        }
        else {
            construct(*value);
        }
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 optional& operator=(optional<T>&& value) noexcept {
        if (_has_value && value) {
            _value = std::move(*value);
        }
        else if (value) {
            construct(std::move(*value));
        }
        else {
            _has_value = false;
        }
        value._has_value = false;
        return *this;
    }

    constexpr explicit operator bool() const noexcept {
        return _has_value;
    }

    LZ_CONSTEXPR_CXX_14 const T& value() const {
        if (_has_value) {
            return _value;
        }
        throw std::runtime_error("Cannot get uninitialized optional");
    }

    LZ_CONSTEXPR_CXX_14 T& value() {
        return const_cast<T&>(static_cast<const optional<T>*>(this)->value());
    }

    constexpr T& operator*() noexcept {
        return const_cast<T&>(static_cast<const optional<T>*>(this)->operator*());
    }

    constexpr const T& operator*() const noexcept {
        return _value;
    }

    template<class U>
    constexpr T value_or(U&& v) const& {
        return bool(*this) ? this->value() : static_cast<T>(std::forward<U>(v));
    }

    template<class U>
    constexpr T value_or(U&& v) && {
        return bool(*this) ? std::move(this->value()) : static_cast<T>(std::forward<U>(v));
    }
};

template<class T>
constexpr bool operator==(const optional<T>& lhs, const optional<T>& rhs) {
    return bool(lhs) != bool(rhs) ? false : !bool(lhs) ? true : *lhs == *rhs;
}

template<class T>
constexpr bool operator!=(const optional<T>& lhs, const optional<T>& rhs) {
    return !(lhs == rhs);
}

template<class T>
constexpr bool operator<(const optional<T>& lhs, const optional<T>& rhs) {
    return !rhs ? false : !lhs ? true : *lhs < *rhs;
}

template<class T>
constexpr bool operator>(const optional<T>& lhs, const optional<T>& rhs) {
    return rhs < lhs;
}

template<class T>
constexpr bool operator<=(const optional<T>& lhs, const optional<T>& rhs) {
    return !(rhs < lhs);
}

template<class T>
constexpr bool operator>=(const optional<T>& lhs, const optional<T>& rhs) {
    return !(lhs < rhs);
}
#endif // __cpp_lib_optional
} // namespace lz

#endif