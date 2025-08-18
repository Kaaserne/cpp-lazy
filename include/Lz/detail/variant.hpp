#pragma once

#ifndef LZ_DETAIL_VARIANT_HPP
#define LZ_DETAIL_VARIANT_HPP

#include <Lz/detail/compiler_checks.hpp>

#ifdef LZ_HAS_CXX_17

#include <variant>

namespace lz {
namespace detail {

template<class T, class T2>
using variant = std::variant<T, T2>;

} // namespace detail
} // namespace lz

#else

#include <Lz/detail/procs.hpp>
#include <cstdint>

namespace lz {
namespace detail {

template<class T, class T2>
class variant {
    static_assert(!std::is_same<T, T2>::value, "T and T2 must be different types");

    enum class state : std::uint_least8_t {
        none = static_cast<std::uint_least8_t>(-1),
        t = 0,
        t2 = 1,
    } _state{ state::none };

    union types {
        T _t;
        T2 _t2;

        types() {
        }

        ~types() {
        }
    } _variant;

    template<class U, class V>
    void reconstruct(state s, U&& this_variant, V&& other_variant_type) {
        this->~variant();
        _state = s;
        ::new (detail::addressof(this_variant)) decay_t<U>(std::forward<V>(other_variant_type));
    }

    template<class V, class V2>
    void construct(V&& other_variant_type, V2&& other_variant_type2) {
        switch (_state) {
        case state::t:
            ::new (detail::addressof(_variant._t)) T(std::forward<V>(other_variant_type));
            break;
        case state::t2:
            ::new (detail::addressof(_variant._t2)) T2(std::forward<V2>(other_variant_type2));
            break;
        default:
            break;
        }
    }

public:
    constexpr variant() noexcept = default;

    variant(const T& t) : _state{ state::t } {
        ::new (detail::addressof(_variant._t)) T(t);
    }

    variant(const T2& t2) : _state{ state::t } {
        ::new (detail::addressof(_variant._t2)) T2(t2);
    }

    variant(T&& t) noexcept(std::is_nothrow_move_constructible<T>::value) : _state{ state::t } {
        ::new (detail::addressof(_variant._t)) T(std::move(t));
    }

    variant(T2&& t2) noexcept(std::is_nothrow_move_constructible<T2>::value) : _state{ state::t2 } {
        ::new (detail::addressof(_variant._t2)) T2(std::move(t2));
    }

    LZ_CONSTEXPR_CXX_14 variant(const variant& other) : _state{ other._state } {
        construct(other._variant._t, other._variant._t2);
    }

    LZ_CONSTEXPR_CXX_14 variant(variant&& other) noexcept(std::is_nothrow_move_constructible<T>::value &&
                                                          std::is_nothrow_move_constructible<T2>::value) :
        _state{ other._state } {
        construct(std::move(other._variant._t), std::move(other._variant._t2));
    }

    LZ_CONSTEXPR_CXX_14 variant& operator=(const T& t) {
        reconstruct(state::t, _variant._t, t);
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 variant& operator=(const T2& t2) {
        reconstruct(state::t2, _variant._t2, t2);
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 variant& operator=(T&& t) noexcept(std::is_nothrow_move_constructible<T>::value) {
        reconstruct(state::t, _variant._t, std::move(t));
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 variant& operator=(T2&& t2) noexcept(std::is_nothrow_move_constructible<T2>::value) {
        reconstruct(state::t2, _variant._t2, std::move(t2));
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 variant& operator=(const variant& other) {
        this->~variant();
        _state = other._state;
        construct(other._variant._t, other._variant._t2);
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 variant& operator=(variant&& other) noexcept(std::is_nothrow_move_constructible<T>::value &&
                                                                     std::is_nothrow_move_constructible<T2>::value) {
        this->~variant();
        _state = other._state;
        construct(std::move(other._variant._t), std::move(other._variant._t2));
        return *this;
    }

    template<size_t I, class... Args>
    LZ_CONSTEXPR_CXX_14 enable_if_t<I == 0> emplace(Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value) {
        this->~variant();
        ::new (detail::addressof(_variant._t)) T(std::forward<Args>(args)...);
        _state = state::t;
    }

    template<size_t I, class... Args>
    LZ_CONSTEXPR_CXX_14 enable_if_t<I == 1> emplace(Args&&... args) noexcept(std::is_nothrow_constructible<T2, Args...>::value) {
        this->~variant();
        ::new (detail::addressof(_variant._t2)) T2(std::forward<Args>(args)...);
        _state = state::t2;
    }

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if_t<I == 0, const T&> get() const noexcept {
        LZ_ASSERT(_state == state::t, "Invalid variant access");
        return _variant._t;
    }

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 const enable_if_t<I == 1, const T2&> get() const noexcept {
        LZ_ASSERT(_state == state::t2, "Invalid variant access");
        return _variant._t2;
    }

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if_t<I == 0, T&> get() noexcept {
        LZ_ASSERT(_state == state::t, "Invalid variant access");
        return _variant._t;
    }

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if_t<I == 1, T2&> get() noexcept {
        LZ_ASSERT(_state == state::t2, "Invalid variant access");
        return _variant._t2;
    }

    LZ_CONSTEXPR_CXX_14 std::int_least8_t index() const noexcept {
        return static_cast<std::int_least8_t>(_state);
    }

    ~variant() noexcept(noexcept(std::is_nothrow_destructible<T>::value) && noexcept(std::is_nothrow_destructible<T2>::value)) {
        switch (_state) {
        case state::t:
            _variant._t.~T();
            break;
        case state::t2:
            _variant._t2.~T2();
            break;
        default:
            break;
        }
    }
};

template<size_t I, class T, class T2>
LZ_CONSTEXPR_CXX_14 auto get(const variant<T, T2>& v) noexcept -> decltype(v.template get<I>()) {
    return v.template get<I>();
}

template<size_t I, class T, class T2>
LZ_CONSTEXPR_CXX_14 auto get(variant<T, T2>& v) noexcept -> decltype(v.template get<I>()) {
    return v.template get<I>();
}

} // namespace detail
} // namespace lz

#endif // !defined(__cpp_lib_variant) && !defined(LZ_HAS_CXX_17)

#endif // LZ_DETAIL_VARIANT_HPP
