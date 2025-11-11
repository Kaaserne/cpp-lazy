#pragma once

#ifndef LZ_ALGORITHM_COPY_HPP
#define LZ_ALGORITHM_COPY_HPP

#include <Lz/detail/algorithm/copy.hpp>
#include <Lz/detail/traits/is_sentinel.hpp>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Copies elements from an iterable to the output iterator.
 *
 * @param iterable The iterable to copy from.
 * @param out The output iterator to copy to.
 */
template<class Iterable, class OutputIterator>
void copy(Iterable&& iterable, OutputIterator out) {
    detail::copy(detail::begin(iterable), detail::end(iterable), std::move(out));
}

} // namespace lz

#endif
