#pragma once

#ifndef LZ_TAKE_WHILE_HPP
#define LZ_TAKE_WHILE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/take_while.hpp>

namespace lz {
LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

static const detail::take_while_adaptor take_while{};

#else

LZ_INLINE_VAR constexpr detail::take_while_adaptor take_while{};

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_TAKE_WHILE_HPP