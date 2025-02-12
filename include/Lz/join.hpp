#pragma once

#ifndef LZ_JOIN_HPP
#define LZ_JOIN_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/join.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

static const detail::join_adaptor join{};

#else

LZ_INLINE_VAR constexpr detail::join_adaptor join{};

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif