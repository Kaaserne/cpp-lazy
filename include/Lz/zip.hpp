#pragma once

#ifndef LZ_ZIP_HPP
#define LZ_ZIP_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/zip.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

static const detail::zip_adaptor zip{};

#else

LZ_INLINE_VAR constexpr detail::zip_adaptor zip{};

#endif // LZ_HAS_CXX_11

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif
