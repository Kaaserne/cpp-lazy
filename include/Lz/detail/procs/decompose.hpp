#pragma once

#ifndef LZ_DETAIL_PROCS_DECOMPOSE_HPP
#define LZ_DETAIL_PROCS_DECOMPOSE_HPP

#include <Lz/detail/compiler_config.hpp>

#ifndef LZ_HAS_CXX_17

namespace lz {
namespace detail {

template<class... Ts>
LZ_CONSTEXPR_CXX_14 void decompose(const Ts&...) noexcept {
}

} // namespace detail
} // namespace lz

#endif // LZ_HAS_CXX_17

#endif
