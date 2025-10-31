#pragma once

#ifndef LZ_DETAIL_ALGORITHM_ENDS_WITH_HPP
#define LZ_DETAIL_ALGORITHM_ENDS_WITH_HPP

#include <Lz/algorithm/starts_with.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>
#include <Lz/algorithm/equal.hpp>

namespace lz {
namespace detail {

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate>
LZ_CONSTEXPR_CXX_17 bool ends_with(Iterator1 begin, S1 end, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate) {
    std::reverse_iterator<S1> rbegin{ std::move(end) };
    std::reverse_iterator<Iterator1> rend{ std::move(begin) };
    return starts_with(std::move(rbegin), std::move(rend), std::reverse_iterator<Iterator2>(std::move(end2)),
                       std::reverse_iterator<S2>(std::move(begin2)), std::move(binary_predicate));
}

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 bool ends_with(Iterator1 begin, S1 end, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate,
                                   size_t size_of_iterable1, size_t size_of_iterable2) {
    if (size_of_iterable1 < size_of_iterable2) {
        return false;
    }

    const auto distance = static_cast<diff_type<Iterator1>>(size_of_iterable1 - size_of_iterable2);
    begin = std::next(begin, distance);
    using std::equal;
    using lz::equal;
    return equal(std::move(begin), std::move(end), std::move(begin2), std::move(end2), std::move(binary_predicate));
}

} // namespace detail
} // namespace lz

#endif // LZ_DETAIL_ALGORITHM_ENDS_WITH_HPP
