#ifndef LZ_DUPLICATES_HPP
#define LZ_DUPLICATES_HPP

#include <Lz/detail/adaptors/duplicates.hpp>

// TODO docs

// TODO remove all ifdef 11 for global adaptors

namespace lz {

LZ_INLINE_VAR constexpr detail::duplicates_adaptor duplicates{};

template<class Iterable, class BinaryPredicate>
using duplicates_iterable = detail::duplicates_iterable<Iterable, BinaryPredicate>;

} // namespace lz

#endif
