#pragma once

#ifndef LZ_FAKE_POINTER_PROXY_HPP
#define LZ_FAKE_POINTER_PROXY_HPP

#include <type_traits>
#include <Lz/detail/procs/addressof.hpp>

namespace lz {
namespace detail {

template<class T>
class fake_ptr_proxy {
    T _t;

    using ptr = decltype(detail::addressof(_t));

public:
    constexpr explicit fake_ptr_proxy(const T& t) noexcept(std::is_nothrow_copy_constructible<T>::value) : _t{ t } {
    }

    LZ_NODISCARD constexpr ptr operator->() const noexcept {
        return detail::addressof(_t);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 ptr operator->() noexcept {
        return detail::addressof(_t);
    }
};
} // namespace detail
} // namespace lz
#endif // LZ_FAKE_POINTER_PROXY_HPP
