#pragma once

#ifndef LZ_DETAIL_ALGORITHM_LOWER_BOUND_HPP
#define LZ_DETAIL_ALGORITHM_LOWER_BOUND_HPP

#include <Lz/detail/compiler_config.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>
#include <Lz/procs/eager_size.hpp>

namespace lz {
namespace detail {

template<class Iterator, class T, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 Iterator sized_lower_bound(Iterator begin, const T& value, BinaryPredicate binary_predicate,
                                               diff_type<Iterator> count) {
    while (count > 0) {
        const auto step = count / 2;
        auto it = std::next(begin, step);
        if (binary_predicate(*it, value)) {
            begin = ++it;
            count -= step + 1;
        }
        else {
            count = step;
        }
    }
    return begin;
}

template<class Iterable, class T, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 iter_t<Iterable> lower_bound(Iterable&& iterable, const T& value, BinaryPredicate binary_predicate) {
    using diff = diff_type<iter_t<Iterable>>;
    return sized_lower_bound(detail::begin(iterable), value, std::move(binary_predicate),
                             static_cast<diff>(lz::eager_size(iterable)));
}

} // namespace detail
} // namespace lz

#endif // LZ_DETAIL_ALGORITHM_LOWER_BOUND_HPP
