#ifndef LZ_ALGORITHM_PEEK_HPP
#define LZ_ALGORITHM_PEEK_HPP

#include <Lz/detail/algorithm/peek.hpp>
#include <Lz/detail/compiler_checks.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Returns an optional containing the second element of the given iterable, or nullopt if the iterable has less than
 * two elements.
 *
 * @param iterable The iterable to peek into.
 * @return An optional containing the next element of the iterable, or nullopt if the iterable has less than two elements.
 */
template<class Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 auto
peek(Iterable&& iterable) -> decltype(detail::peek(detail::begin(iterable), detail::end(iterable))) {
    return detail::peek(detail::begin(iterable), detail::end(iterable));
}

} // namespace lz

#endif
