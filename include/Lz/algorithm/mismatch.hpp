#pragma once

#ifndef LZ_ALGORITHM_MISMATCH_HPP
#define LZ_ALGORITHM_MISMATCH_HPP

#include <Lz/detail/algorithm/mismatch.hpp>
#include <Lz/detail/traits/is_sentinel.hpp>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

LZ_MODULE_EXPORT namespace lz {

#ifdef LZ_HAS_CXX_17

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate = detail::equal_to>
[[nodiscard]] constexpr std::pair<Iterator1, Iterator2>
mismatch(Iterator1 begin1, S1 end1, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate = {}) {
    if constexpr (detail::is_sentinel_v<Iterator1, S1> || detail::is_sentinel_v<Iterator2, S2>) {
        return detail::algorithm::mismatch(std::move(begin1), std::move(end1), std::move(begin2), std::move(end2),
                                           std::move(binary_predicate));
    }
    else {
        return std::mismatch(std::move(begin1), std::move(end1), std::move(begin2), std::move(end2), std::move(binary_predicate));
    }
}

#elif defined(LZ_HAS_CXX_14)

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate = detail::equal_to>
LZ_CONSTEXPR_CXX_14 detail::enable_if_t<!detail::is_sentinel<Iterator1, S1>::value && !detail::is_sentinel<Iterator2, S2>::value,
                                        std::pair<Iterator1, Iterator2>>
mismatch(Iterator1 begin1, S1 end1, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate = {}) {
    return std::mismatch(std::move(begin1), std::move(end1), std::move(begin2), std::move(end2), std::move(binary_predicate));
}

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate = detail::equal_to>
LZ_CONSTEXPR_CXX_14 detail::enable_if_t<detail::is_sentinel<Iterator1, S1>::value || detail::is_sentinel<Iterator2, S2>::value,
                                        std::pair<Iterator1, Iterator2>>
mismatch(Iterator1 begin1, S1 end1, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate = {}) {
    return detail::algorithm::mismatch(std::move(begin1), std::move(end1), std::move(begin2), std::move(end2),
                                       std::move(binary_predicate));
}

#else

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate = detail::equal_to>
LZ_CONSTEXPR_CXX_14 detail::enable_if_t<!detail::is_sentinel<Iterator1, S1>::value && !detail::is_sentinel<Iterator2, S2>::value,
                                        std::pair<Iterator1, Iterator2>>
mismatch(Iterator1 begin1, S1 end1, Iterator2 begin2, S2, BinaryPredicate binary_predicate = {}) {
    return std::mismatch(std::move(begin1), std::move(end1), std::move(begin2), std::move(binary_predicate));
}

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate = detail::equal_to>
LZ_CONSTEXPR_CXX_14 detail::enable_if_t<detail::is_sentinel<Iterator1, S1>::value || detail::is_sentinel<Iterator2, S2>::value,
                                        std::pair<Iterator1, Iterator2>>
mismatch(Iterator1 begin1, S1 end1, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate = {}) {
    return detail::algorithm::mismatch(std::move(begin1), std::move(end1), std::move(begin2), std::move(end2),
                                       std::move(binary_predicate));
}

#endif

template<class Iterable1, class Iterable2, class BinaryPredicate = detail::equal_to>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool
mismatch(Iterable1&& iterable1, Iterable2&& iterable2, BinaryPredicate binary_predicate = {}) {
    return lz::mismatch(detail::begin(iterable1), detail::end(iterable1), detail::begin(iterable2), detail::end(iterable2),
                        std::move(binary_predicate));
}
} // namespace lz

#endif
