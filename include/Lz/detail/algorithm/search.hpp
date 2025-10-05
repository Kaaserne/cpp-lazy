#pragma once

#ifndef LZ_DETAIL_ALGORITHM_SEARCH_HPP
#define LZ_DETAIL_ALGORITHM_SEARCH_HPP

#include <Lz/detail/traits/iterator_categories.hpp>
#include <utility>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

namespace lz {
namespace detail {
namespace algorithm {

template<class Iterator, class Iterator2, class BinaryPredicate>
std::pair<Iterator, Iterator>
std_search(Iterator begin, Iterator end, Iterator2 begin2, Iterator2 end2, BinaryPredicate binary_predicate) {
    auto pos = std::search(std::move(begin), std::move(end), begin2, end2, std::move(binary_predicate));
    return pos == end ? std::make_pair(pos, pos) : std::make_pair(pos, std::next(pos, std::distance(begin2, end2)));
}

#ifdef LZ_HAS_CXX_17

// TODO add overload for cxx 14 with ra
template<class Iterator, class S, class Iterator2, class S2, class BinaryPredicate>
constexpr std::pair<Iterator, Iterator>
search(Iterator begin, S end, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate) {
    if constexpr (!std::is_same_v<Iterator, S> || !std::is_same_v<Iterator2, S2>) {
        if constexpr (is_ra_v<Iterator> || is_ra_v<Iterator2>) {
            auto end_common = begin + (end - begin);
            auto end2_common = begin2 + (end2 - begin2);
            return std_search(std::move(begin), std::move(end_common), std::move(begin2), std::move(end2_common),
                              std::move(binary_predicate));
        }
        else {
            while (begin != end) {
                auto it = begin;
                auto it2 = begin2;
                while (it2 != end2 && it != end && binary_predicate(*it, *it2)) {
                    ++it;
                    ++it2;
                }
                if (it2 == end2) {
                    return { begin, it };
                }
                ++begin;
            }
            return { begin, begin };
        }
    }
    else {
        return std_search(std::move(begin), std::move(end), std::move(begin2), std::move(end2), std::move(binary_predicate));
    }
}

#else

template<class Iterator, class S, class Iterator2, class S2, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14
    enable_if_t<!std::is_same<Iterator, S>::value || !std::is_same<Iterator2, S2>::value, std::pair<Iterator, Iterator>>
    search(Iterator begin, S end, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate) {
    while (begin != end) {
        auto it = begin;
        auto it2 = begin2;
        while (it2 != end2 && it != end && binary_predicate(*it, *it2)) {
            ++it;
            ++it2;
        }
        if (it2 == end2) {
            return { begin, it };
        }
        ++begin;
    }
    return { begin, begin };
}

template<class Iterator, class S, class Iterator2, class S2, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14
    enable_if_t<std::is_same<Iterator, S>::value && std::is_same<Iterator2, S2>::value, std::pair<Iterator, Iterator>>
    search(Iterator begin, S end, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate) {
    return std_search(std::move(begin), std::move(end), std::move(begin2), std::move(end2), std::move(binary_predicate));
}

#endif // LZ_HAS_CXX_17

} // namespace algorithm
} // namespace detail
} // namespace lz

#endif
