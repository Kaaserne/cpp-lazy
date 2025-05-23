#pragma once

#ifndef LZ_DETAIL_VARIANT_HPP
#define LZ_DETAIL_VARIANT_HPP

#include <Lz/detail/compiler_checks.hpp>

#if defined(__cpp_lib_variant) && defined(LZ_HAS_CXX_17)

#include <variant>

namespace lz {
namespace detail {

template<class T, class T2>
using variant = std::variant<T, T2>;

} // namespace detail
} // namespace lz

#else

#include <stdexcept>

namespace lz {
namespace detail {

template<class T, class T2>
class variant {
    static_assert(!std::is_same<T, T2>::value, "T and T2 must be different types");

    enum class state {
        none,
        t,
        t2,
    } _state;

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
        ::new (std::addressof(this_variant)) decay_t<U>(std::forward<V>(other_variant_type));
    }

    template<class V, class V2>
    void construct(V&& other_variant_type, V2&& other_variant_type2) {
        switch (_state) {
        case state::t:
            ::new (std::addressof(_variant._t)) T(std::forward<V>(other_variant_type));
            break;
        case state::t2:
            ::new (std::addressof(_variant._t2)) T2(std::forward<V2>(other_variant_type2));
            break;
        default:
            break;
        }
    }

public:
    constexpr variant() noexcept : _state(state::none) {
    }

    variant(const T& t) : _state(state::t) {
        ::new (std::addressof(_variant._t)) T(t);
    }

    variant(const T2& t2) : _state(state::t2) {
        ::new (std::addressof(_variant._t2)) T2(t2);
    }

    variant(T&& t) noexcept : _state(state::t) {
        ::new (std::addressof(_variant._t)) T(std::move(t));
    }

    variant(T2&& t2) noexcept : _state(state::t2) {
        ::new (std::addressof(_variant._t2)) T2(std::move(t2));
    }

    LZ_CONSTEXPR_CXX_14 variant(const variant& other) : _state{ other._state } {
        construct(other._variant._t, other._variant._t2);
    }

   LZ_CONSTEXPR_CXX_14  variant(variant&& other) noexcept : _state{ other._state } {
        other._state = state::none;
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

    LZ_CONSTEXPR_CXX_14 variant& operator=(T&& t) noexcept {
        reconstruct(state::t, _variant._t, std::move(t));
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 variant& operator=(T2&& t2) noexcept {
        reconstruct(state::t2, _variant._t2, std::move(t2));
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 variant& operator=(const variant& other) {
        this->~variant();
        _state = other._state;
        construct(other._variant._t, other._variant._t2);
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 variant& operator=(variant&& other) noexcept {
        this->~variant();
        _state = other._state;
        construct(std::move(other._variant._t), std::move(other._variant._t2));
        other._state = state::none;
        return *this;
    }
    
    template<class U>
    LZ_CONSTEXPR_CXX_14 const U* get_if() const {
        constexpr auto is_t = std::is_same<T, U>::value;
        if (_state == state::t && is_t) {
            return reinterpret_cast<const U*>(std::addressof(_variant._t));
        }
        constexpr auto is_t2 = std::is_same<T2, U>::value;
        if (_state == state::t2 && is_t2) {
            return reinterpret_cast<const U*>(std::addressof(_variant._t2));
        }
        return nullptr;
    }

    template<class U>
    LZ_CONSTEXPR_CXX_14 U* get_if() {
        return const_cast<U*>(static_cast<const variant&>(*this).get_if<U>());
    }

    template<class U>
    LZ_CONSTEXPR_CXX_14 const U& get() const {
        if (const auto* u = get_if<U>()) {
            return *u;
        }
        throw std::runtime_error("Bad variant access");
    }

    template<class U>
    LZ_CONSTEXPR_CXX_14 U& get() {
        return const_cast<U&>(static_cast<const variant&>(*this).get<U>());
    }

    ~variant() {
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
        _state = state::none;
    }
};

template<class T, class U, class V>
LZ_CONSTEXPR_CXX_14 const T& get(const variant<U, V>& t) {
    return t.template get<T>();
}

template<class T, class U, class V>
LZ_CONSTEXPR_CXX_14 T& get(variant<U, V>& t) {
    return t.template get<T>();
}

template<class T, class U, class V>
LZ_CONSTEXPR_CXX_14 const T* get_if(const variant<U, V>* t) {
    return t->template get_if<T>();
}

template<class T, class U, class V>
LZ_CONSTEXPR_CXX_14 T* get_if(variant<U, V>* t) {
    return t->template get_if<T>();
}

} // namespace detail
} // namespace lz

#endif // !defined(__cpp_lib_variant) && !defined(LZ_HAS_CXX_17)

#endif // LZ_DETAIL_VARIANT_HPP