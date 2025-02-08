#pragma once

#ifndef LZ_JOIN_WHERE_HPP
#define LZ_JOIN_WHERE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/join_where.hpp>

namespace lz {
LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

static const detail::join_where_adaptor join_where{};

#else

LZ_INLINE_VAR constexpr detail::join_where_adaptor join_where{};

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_JOIN_WHERE_HPP
