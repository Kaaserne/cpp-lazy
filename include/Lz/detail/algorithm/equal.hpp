#pragma once

#ifndef LZ_DETAIL_ALGORITHM_EQUAL_HPP
#define LZ_DETAIL_ALGORITHM_EQUAL_HPP

#include <Lz/algorithm/mismatch.hpp>
#include <Lz/detail/procs/operators.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>

namespace lz {
namespace detail {
namespace algorithm {

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool
equal(Iterator1 begin1, S1 end1, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate) {
    const auto mm = lz::mismatch(begin1, end1, begin2, end2, binary_predicate);
    return mm.first == end1 && mm.second == end2;
}

} // namespace algorithm
} // namespace detail
} // namespace lz

#endif
