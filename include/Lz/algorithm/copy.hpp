#pragma once

#ifndef LZ_ALGORITHM_COPY_HPP
#define LZ_ALGORITHM_COPY_HPP

#include <Lz/detail/algorithm/copy.hpp>
#include <Lz/detail/traits/is_sentinel.hpp>

LZ_MODULE_EXPORT namespace lz {

template<class Iterator, class S, class OutputIterator>
void copy(Iterator begin, S end, OutputIterator out) {
    if constexpr (detail::is_sentinel_v<Iterator, S>) {
        detail::algorithm::copy(begin, end, out);
    }
    else {
        static_cast<void>(std::copy(begin, end, out));
    }
}

/**
 * @brief Copies elements from an iterable to the output iterator.
 *
 * @param iterable The iterable to copy from.
 * @param out The output iterator to copy to.
 */
template<class Iterable, class OutputIterator>
void copy(const Iterable& iterable, OutputIterator out) {
    using std::copy;
    detail::algorithm::copy(detail::begin(iterable), detail::end(iterable), out);
}

} // namespace lz

#endif
