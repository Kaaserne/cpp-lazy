#pragma once

#ifndef LZ_UNIQUE_PTR_HPP
#define LZ_UNIQUE_PTR_HPP

#ifdef LZ_HAS_CXX_14

#include <memory>

#endif

#ifdef LZ_HAS_CXX_11

#include <utility>

#endif

namespace lz {
namespace detail {

#ifdef LZ_HAS_CXX_14

using std::make_unique;
using std::unique_ptr;

#else

template<class T>
class unique_ptr {
    T* _ptr{ nullptr };

    template<class>
    friend class unique_ptr;

    void check_pointer_compat() const noexcept {
        LZ_ASSERT(_ptr != nullptr, "Cannot dereference nullptr");
    }

public:
    constexpr unique_ptr() noexcept = default;

    constexpr unique_ptr(T* ptr) noexcept : _ptr{ ptr } {
    }

    constexpr unique_ptr(const unique_ptr&) = delete;
    constexpr unique_ptr& operator=(const unique_ptr&) const = delete;

    template<class U>
    unique_ptr(unique_ptr<U>&& other) noexcept {
        _ptr = other.release();
        other._ptr = nullptr;
    }

    template<class U>
    unique_ptr& operator=(unique_ptr<U>&& other) noexcept {
        if (this != &other) {
            reset(other.release());
        }
        return *this;
    }

    ~unique_ptr() {
        if (_ptr) {
            delete _ptr;
            _ptr = nullptr;
        }
    }

    constexpr const T* get() const noexcept {
        return _ptr;
    }

    T* release() noexcept {
        T* ptr = _ptr;
        _ptr = nullptr;
        return ptr;
    }

    void reset(T* ptr = nullptr) noexcept {
        if (_ptr == ptr) {
            return;
        }
        delete _ptr;
        _ptr = ptr;
    }

    T& operator*() noexcept {
        check_pointer_compat();
        return *_ptr;
    }

    const T& operator*() const noexcept {
        check_pointer_compat();
        return *_ptr;
    }

    T* operator->() noexcept {
        check_pointer_compat();
        return _ptr;
    }

    const T* operator->() const noexcept {
        check_pointer_compat();
        return _ptr;
    }

    constexpr explicit operator bool() const noexcept {
        return _ptr != nullptr;
    }
};

template<class T, class... Args>
unique_ptr<T> make_unique(Args&&... args) {
    return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

#endif

} // namespace detail
} // namespace lz

#endif
