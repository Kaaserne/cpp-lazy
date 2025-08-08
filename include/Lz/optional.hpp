#pragma once

#ifndef LZ_OPTIONAL_HPP
#define LZ_OPTIONAL_HPP

#include <Lz/detail/compiler_checks.hpp>

#ifdef LZ_HAS_CXX_17

#include <optional>

#else // LZ_HAS_CXX_17

#include <Lz/detail/procs.hpp>
#include <cstdint>
#include <stdexcept>
#include <type_traits>

#endif // LZ_HAS_CXX_17

LZ_MODULE_EXPORT namespace lz {

#ifdef LZ_HAS_CXX_17

using std::optional;
using std::nullopt_t;
using std::nullopt;

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
    bool _has_value{ false };

    template<class U>
    void construct(U&& obj) noexcept(std::is_nothrow_constructible<T, U>::value) {
        ::new (static_cast<void*>(std::addressof(_value))) T(std::forward<U>(obj));
        _has_value = true;
    }

public:
    template<class U>
    friend class optional;

    constexpr optional() noexcept : optional{ nullopt } {
    }

    constexpr optional(nullopt_t) noexcept {
    }

    constexpr optional(const T& value) noexcept(std::is_nothrow_copy_constructible<T>::value) :
        _value{ value },
        _has_value{ true } {
    }

    constexpr optional(T&& value) noexcept(std::is_move_constructible<T>::value) :
        _value{ std::move(value) },
        _has_value{ true } {
    }

    LZ_CONSTEXPR_CXX_14 optional(optional<T>&& that) noexcept(std::is_nothrow_move_constructible<T>::value) {
        if (that) {
            construct(std::move(*that));
        }
    }

    LZ_CONSTEXPR_CXX_14 optional(const optional<T>& that) noexcept(noexcept(construct(*that))) {
        if (that) {
            construct(*that);
        }
    }

    template<class U>
    LZ_CONSTEXPR_CXX_14 optional(optional<U>&& that) noexcept(std::is_nothrow_constructible<T, U>::value) {
        if (that) {
            construct(std::move(*that));
        }
    }

    template<class U>
    LZ_CONSTEXPR_CXX_14 optional(const optional<U>& that) noexcept(noexcept(construct(*that))) {
        if (that) {
            construct(*that);
        }
    }

    ~optional() {
        if (_has_value) {
            _value.~T();
        }
    }

    LZ_CONSTEXPR_CXX_14 optional&
    operator=(T&& value) noexcept(std::is_nothrow_move_assignable<T>::value && std::is_nothrow_move_constructible<T>::value) {
        if (_has_value) {
            _value = std::move(value);
        }
        else {
            construct(std::move(value));
        }
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 optional& operator=(const T& value) noexcept(std::is_nothrow_copy_assignable<T>::value &&
                                                                     std::is_nothrow_copy_constructible<T>::value) {
        if (_has_value) {
            _value = value;
        }
        else {
            construct(value);
        }
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 optional& operator=(const optional<T>& that) noexcept(std::is_nothrow_copy_assignable<T>::value &&
                                                                              std::is_nothrow_copy_constructible<T>::value) {
        if (*this && that) {
            _value = *that;
        }
        else if (!that) {
            _has_value = false;
        }
        else {
            construct(*that);
        }
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 optional& operator=(optional<T>&& that) noexcept(std::is_nothrow_move_assignable<T>::value &&
                                                                         std::is_nothrow_move_constructible<T>::value) {
        if (*this && that) {
            _value = std::move(*that);
        }
        else if (that) {
            construct(std::move(*that));
        }
        else {
            _has_value = false;
        }
        that._has_value = false;
        return *this;
    }

    LZ_NODISCARD constexpr bool has_value() const noexcept {
        return _has_value;
    }

    LZ_NODISCARD constexpr explicit operator bool() const noexcept {
        return _has_value;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 const T& value() const {
        if (_has_value) {
            return _value;
        }
        throw std::runtime_error("Cannot get uninitialized optional");
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 T& value() {
        return const_cast<T&>(static_cast<const optional<T>*>(this)->value());
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 T& operator*() noexcept {
        return const_cast<T&>(static_cast<const optional<T>*>(this)->operator*());
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 const T& operator*() const noexcept {
        LZ_ASSERT(_has_value, "Cannot get uninitialized optional");
        return _value;
    }

    template<class U>
    LZ_NODISCARD constexpr T value_or(U&& v) const& {
        return bool(*this) ? this->value() : static_cast<T>(std::forward<U>(v));
    }

    template<class U>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 T value_or(U&& v) && {
        return bool(*this) ? std::move(this->value()) : static_cast<T>(std::forward<U>(v));
    }
};

template<class T, class U>
constexpr bool operator==(const optional<T>& lhs, const optional<U>& rhs) noexcept {
    return bool(lhs) != bool(rhs) ? false : !bool(lhs) ? true : *lhs == *rhs;
}

template<class T, class U>
constexpr bool operator!=(const optional<T>& lhs, const optional<U>& rhs) noexcept {
    return !(lhs == rhs);
}

#endif // __cpp_lib_optional

} // namespace lz

#endif
