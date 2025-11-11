#pragma once

#ifndef LZ_ALGORITHM_NPOS_HPP
#define LZ_ALGORITHM_NPOS_HPP

#include <Lz/detail/compiler_config.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * This value is returned when index_of(_if) does not find the value specified.
 */
LZ_INLINE_VAR constexpr size_t npos = static_cast<size_t>(-1);

}

#endif
