#pragma once

#ifndef LZ_C_STRING_ADAPTOR_HPP
#define LZ_C_STRING_ADAPTOR_HPP

#include <Lz/detail/iterables/c_string.hpp>

namespace lz {
namespace detail {
struct c_string_adaptor {
#ifdef LZ_HAS_CXX_11
    static c_string_adaptor c_string;
#endif

    using adaptor = c_string_adaptor;

    template<class C>
    LZ_NODISCARD constexpr c_string_iterable<decay_t<C>> operator()(C&& str) const noexcept {
        return { std::forward<C>(str) };
    }
};
} // namespace detail
} // namespace lz

#endif