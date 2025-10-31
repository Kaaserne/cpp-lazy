#pragma once

#ifndef LZ_ALGORITHM_ENDS_WITH_HPP
#define LZ_ALGORITHM_ENDS_WITH_HPP

#include <Lz/detail/algorithm/ends_with.hpp>
#include <Lz/procs/eager_size.hpp>

LZ_MODULE_EXPORT namespace lz {

#ifdef LZ_HAS_CXX_17

/**
 * @brief Checks if the range [begin(iterable), end(iterable)) ends with the bidirectional range [begin(iterable2),
 * end(iterable2)). Needs to know the size of the range if its input iterables aren't bidirectional
 * so it may be worth your while to use `lz::cache_size` if the input iterable @p iterable and @p iterable2 aren't sized and
 * going to use this function multiple times. Does not use `lz::cached_reverse` to reverse the elements (if bidirectional).
 * @param iterable The iterable to check
 * @param iterable2 The bidirectional iterable to check for
 * @param binary_predicate The binary binary_predicate to check the values with
 * @return `true` if the value is found, `false` otherwise
 */
template<class Iterable, class Iterable2, class BinaryPredicate = LZ_BIN_OP(equal_to, detail::val_iterable_t<Iterable>)>
[[nodiscard]] constexpr bool ends_with(Iterable && iterable, Iterable2 && iterable2, BinaryPredicate binary_predicate = {}) {
    if constexpr (detail::is_bidi_v<iter_t<Iterable>>) {
        return detail::ends_with(detail::begin(iterable), detail::end(iterable), detail::begin(iterable2), detail::end(iterable2),
                                 std::move(binary_predicate));
    }
    else {
        return detail::ends_with(detail::begin(iterable), detail::end(iterable), detail::begin(iterable2), detail::end(iterable2),
                                 std::move(binary_predicate), lz::eager_size(iterable), lz::eager_size(iterable2));
    }
}

#else

/**
 * @brief Checks if the range [begin(iterable), end(iterable)) ends with the bidirectional range [begin(iterable2),
 * end(iterable2))
 *
 * @param iterable The iterable to check
 * @param iterable2 The bidirectional iterable to check for
 * @param binary_predicate The binary binary_predicate to check the values with
 * @return `true` if the value is found, `false` otherwise
 */
template<class Iterable, class Iterable2, class BinaryPredicate = LZ_BIN_OP(equal_to, detail::val_iterable_t<Iterable>)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::enable_if_t<detail::is_bidi<iter_t<Iterable>>::value, bool> ends_with(
    Iterable && iterable, Iterable2 && iterable2, BinaryPredicate binary_predicate = {}) {
    return detail::ends_with(detail::begin(iterable), detail::end(iterable), detail::begin(iterable2), detail::end(iterable2),
                             std::move(binary_predicate));
}

/**
 * @brief Checks if the range [begin(iterable), end(iterable)) ends with the forward iterable range [begin(iterable2),
 * end(iterable2)). Needs to know the size of the range if its input iterables aren't bidirectional so it may be worth your
 * while to use `lz::cache_size` if the input iterable @p iterable and @p iterable2 aren't sized and going to use this
 * function multiple times.
 *
 * @param iterable The iterable to check
 * @param iterable2 The forward iterable to check for
 * @param binary_predicate The binary binary_predicate to check the values with
 * @return `true` if the value is found, `false` otherwise
 */
template<class Iterable, class Iterable2, class BinaryPredicate = LZ_BIN_OP(equal_to, detail::val_iterable_t<Iterable>)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::enable_if_t<!detail::is_bidi<iter_t<Iterable>>::value, bool> ends_with(
    Iterable && iterable, Iterable2 && iterable2, BinaryPredicate binary_predicate = {}) {
    return detail::ends_with(detail::begin(iterable), detail::end(iterable), detail::begin(iterable2), detail::end(iterable2),
                             std::move(binary_predicate), lz::eager_size(iterable), lz::eager_size(iterable2));
}

#endif // LZ_HAS_CXX_17
} // namespace lz

#endif
