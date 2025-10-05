#pragma once

#ifndef LZ_DETAIL_ALGORITHM_FIND_LAST_IF_HPP
#define LZ_DETAIL_ALGORITHM_FIND_LAST_IF_HPP

#include <Lz/detail/iterables/reverse.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <Lz/detail/traits/remove_ref.hpp>
#include <Lz/algorithm/find_if.hpp>

namespace lz {
namespace detail {
namespace algorithm {

template<class Iterator, class S>
LZ_CONSTEXPR_CXX_14 Iterator unwrap_reverse_iterator(std::reverse_iterator<Iterator> it, Iterator begin, S end) {
    auto base = it.base();
    if (base == begin) {
        return end;
    }
    return --base;
}

template<class Iterable, class UnaryPredicate>
constexpr iter_t<Iterable> find_last_if(Iterable&& iterable, UnaryPredicate unary_predicate) {
    if constexpr (is_bidi_v<iter_t<remove_ref_t<Iterable>>>) {
        using std::find_if;
        using lz::find_if;

        reverse_iterable<remove_ref_t<Iterable>, false> rev{ iterable };
        auto pos = find_if(detail::begin(rev), detail::end(rev), std::move(unary_predicate));
        return unwrap_reverse_iterator(std::move(pos), detail::begin(iterable), detail::end(iterable));
    }
    else {
        auto current = detail::begin(iterable);
        auto it = current;
        auto end = detail::end(iterable);
        for (; it != end; ++it) {
            if (unary_predicate(*it)) {
                current = it;
            }
        }
        // Check if current was never updated
        if (it == end && current == detail::begin(iterable) && !unary_predicate(*current)) {
            return it;
        }
        return current;
    }
}

} // namespace algorithm
} // namespace detail
} // namespace lz

#endif // LZ_DETAIL_ALGORITHM_FIND_LAST_IF_HPP
