#pragma once

#ifndef LZ_TAKE_EVERY_HPP
#define LZ_TAKE_EVERY_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/take_every.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

static const detail::take_every_adaptor take_every{};

#else

LZ_INLINE_VAR constexpr detail::take_every_adaptor take_every{};

#endif // LZ_HAS_CXX_11

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif