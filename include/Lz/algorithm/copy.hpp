#pragma once

#ifndef LZ_ALGORITHM_COPY_HPP
#define LZ_ALGORITHM_COPY_HPP

#include <Lz/detail/algorithm/copy.hpp>
#include <Lz/detail/traits/is_sentinel.hpp>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

LZ_MODULE_EXPORT namespace lz {

#ifdef LZ_HAS_CXX_17

template<class Iterator, class S, class OutputIterator>
constexpr void copy(Iterator begin, S end, OutputIterator out) {
    if constexpr (detail::is_sentinel_v<Iterator, S>) {
        detail::algorithm::copy(std::move(begin), std::move(end), std::move(out));
    }
    else {
        static_cast<void>(std::copy(std::move(begin), std::move(end), std::move(out)));
    }
}

#else

template<class Iterator, class S, class OutputIterator>
LZ_CONSTEXPR_CXX_14 detail::enable_if_t<detail::is_sentinel<Iterator, S>::value> copy(Iterator begin, S end, OutputIterator out) {
    detail::algorithm::copy(std::move(begin), std::move(end), std::move(out));
}

template<class Iterator, class S, class OutputIterator>
detail::enable_if_t<!detail::is_sentinel<Iterator, S>::value> copy(Iterator begin, S end, OutputIterator out) {
    static_cast<void>(std::copy(std::move(begin), std::move(end), std::move(out)));
}

#endif

/**
 * @brief Copies elements from an iterable to the output iterator.
 *
 * @param iterable The iterable to copy from.
 * @param out The output iterator to copy to.
 */
template<class Iterable, class OutputIterator>
void copy(Iterable&& iterable, OutputIterator out) {
    lz::copy(detail::begin(iterable), detail::end(iterable), std::move(out));
}

} // namespace lz

#endif
