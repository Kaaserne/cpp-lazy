#pragma once

#ifndef LZ_PAIRWISE_HPP
#define LZ_PAIRWISE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/pairwise.hpp>

LZ_MODULE_EXPORT namespace lz {
// TODO docs
LZ_INLINE_VAR constexpr detail::pairwise_adaptor pairwise{};

}

#endif
