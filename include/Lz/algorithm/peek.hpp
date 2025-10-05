#ifndef LZ_ALGORITHM_PEEK_HPP
#define LZ_ALGORITHM_PEEK_HPP

#include <Lz/detail/algorithm/peek.hpp>
#include <Lz/detail/compiler_checks.hpp>
#include <Lz/util/optional.hpp>

LZ_MODULE_EXPORT namespace lz {
template<class Iterator, class S>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 auto peek(Iterator begin, S end) -> decltype(detail::algorithm::peek(begin, end)) {
    return detail::algorithm::peek(begin, end);
}

/**
 * @brief Returns an optional containing the second element of the given iterable, or nullopt if the iterable has less than
 * two elements.
 *
 * @param iterable The iterable to peek into.
 * @return An optional containing the next element of the iterable, or nullopt if the iterable has less than two elements.
 */
template<class Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 auto
peek(Iterable&& iterable) -> decltype(lz::peek(detail::begin(iterable), detail::end(iterable))) {
    return lz::peek(detail::begin(iterable), detail::end(iterable));
}
} // namespace lz

#endif
