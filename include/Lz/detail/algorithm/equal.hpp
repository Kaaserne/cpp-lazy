#pragma once

#ifndef LZ_DETAIL_ALGORITHM_EQUAL_HPP
#define LZ_DETAIL_ALGORITHM_EQUAL_HPP

#include <Lz/detail/procs/get_end.hpp>
#include <Lz/detail/traits/enable_if.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <Lz/detail/traits/std_algo_compat.hpp>

namespace lz {
namespace detail {

#ifdef LZ_HAS_CXX_17

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate>
constexpr bool equal(Iterator1 begin, S1 end, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate) {
    if constexpr (std_algo_compat2_v<Iterator1, S1, Iterator2, S2>) {
        auto last1 = detail::get_end(begin, end);
        auto last2 = detail::get_end(begin2, end2);
        return std_equal_helper(begin, last1, begin2, last2, std::move(binary_predicate));
    }
    else {
        for (; begin2 != end2; ++begin, (void)++begin2) {
            if (begin == end || !binary_predicate(*begin, *begin2)) {
                return false;
            }
        }
        return true;
    }
}

#elif defined(LZ_HAS_CXX_14)

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<std_algo_compat2<Iterator1, S1, Iterator2, S2>::value, bool>
equal(Iterator1 begin, S1 end, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate) {
    auto last1 = detail::get_end(begin, end);
    auto last2 = detail::get_end(begin2, end2);
    return std_equal_helper(begin, last1, begin2, last2, std::move(binary_predicate));
}

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<!std_algo_compat2<Iterator1, S1, Iterator2, S2>::value, bool>
equal(Iterator1 begin, S1 end, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate) {
    for (; begin != end && begin2 != end2; ++begin, (void)++begin2) {
        if (!binary_predicate(*begin, *begin2)) {
            return false;
        }
    }
    return begin == end && begin2 == end2;
}

#else
// cxx 11

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<!is_ra<Iterator1>::value || !is_ra<Iterator2>::value, bool>
equal(Iterator1 begin, S1 end, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate) {
    for (; begin != end && begin2 != end2; ++begin, (void)++begin2) {
        if (!binary_predicate(*begin, *begin2)) {
            return false;
        }
    }
    return begin == end && begin2 == end2;
}

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<is_ra<Iterator1>::value && is_ra<Iterator2>::value, bool>
equal(Iterator1 begin, S1 end, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate) {
    if ((end2 - begin2) != (end - begin)) {
        return false;
    }
    return std_equal_helper(begin, detail::get_end(begin, end), begin2, detail::get_end(begin2, end2),
                            std::move(binary_predicate));
}

#endif

} // namespace detail
} // namespace lz

#endif
