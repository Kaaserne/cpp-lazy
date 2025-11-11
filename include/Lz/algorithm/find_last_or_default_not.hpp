#pragma once

#ifndef LZ_ALGORITHM_FIND_LAST_OR_DEFAULT_NOT_HPP
#define LZ_ALGORITHM_FIND_LAST_OR_DEFAULT_NOT_HPP

#include <Lz/algorithm/find_last_or_default_if.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Find the last element in the iterable that is not equal to the given value. If no such element is found, return the
 * default value.
 *
 * @param iterable The iterable to search.
 * @param value The value to compare each element against.
 * @param default_value The value to return if no element is found.
 * @return The found element or the default value.
 */
template<class Iterable, class T, class U>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::val_iterable_t<Iterable>
find_last_or_default_not(Iterable&& iterable, T&& value, U&& default_value) {
    return lz::find_last_or_default_if(
        std::forward<Iterable>(iterable), [&value](detail::ref_t<iter_t<Iterable>> val) { return !(val == value); },
        std::forward<U>(default_value));
}

} // namespace lz

#endif
