#pragma once

#ifndef LZ_ZIP_LONGEST_HPP
#define LZ_ZIP_LONGEST_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/zip_longest.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

static const detail::zip_longest_adaptor zip_longest{};

#else

LZ_INLINE_VAR constexpr detail::zip_longest_adaptor zip_longest{};

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_ZIP_LONGEST_HPP