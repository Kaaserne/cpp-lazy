#pragma once

#ifndef LZ_DETAIL_PROCS_OPERATORS_HPP
#define LZ_DETAIL_PROCS_OPERATORS_HPP

#include <Lz/detail/compiler_config.hpp>
#include <functional>

#ifdef LZ_HAS_CXX_14
#define LZ_BIN_OP(OP, T) std::OP<>
#else
#define LZ_BIN_OP(OP, T) std::OP<T>
#endif

#endif // LZ_DETAIL_PROCS_OPERATORS_HPP
