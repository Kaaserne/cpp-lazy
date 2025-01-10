#pragma once

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/traits.hpp>

namespace lz {
namespace ut {

template<class Iterable>
constexpr void has_sentinel(Iterable&&) {
#ifndef LZ_HAS_CXX_11
    static_assert(!std::is_same<iter_t<Iterable>, sentinel_t<Iterable>>::value, "Iterator and Sentinel must be different types");
#else
    static_assert(std::is_same<iter_t<Iterable>, sentinel_t<Iterable>>::value, "Iterator and Sentinel must be the same type");
#endif
}

} // namespace ut
} // namespace lz