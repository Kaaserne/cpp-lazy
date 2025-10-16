#pragma once

#ifndef LZ_ALGORITHM_FIND_LAST_OR_DEFAULT_HPP
#define LZ_ALGORITHM_FIND_LAST_OR_DEFAULT_HPP

#include <Lz/algorithm/find_last_or_default_if.hpp>

LZ_MODULE_EXPORT namespace lz {

template<class Iterator, class S, class T, class U>
LZ_CONSTEXPR_CXX_14 detail::val_t<Iterator> find_last_or_default(Iterator begin, S end, T&& to_find, U&& default_value) {
    return lz::find_last_or_default_if(
        std::move(begin), std::move(end), [&to_find](detail::ref_t<Iterator> value) { return value == to_find; },
        std::forward<U>(default_value));
}

/**
 * @brief Finds the first element in the range [begin(iterable), end(iterable)) that satisfies the value @p to_find. If the
 * element is found, it returns the value, otherwise it returns @p default_value
 * @param iterable The iterable to find the element in
 * @param to_find The value to find
 * @param default_value The value to return if the element is not found, defaults to `T()`
 * @return The value @p to_find if it is found, otherwise @p default_value
 */
template<class Iterable, class T, class U>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::val_iterable_t<Iterable>
find_last_or_default(Iterable&& iterable, T&& to_find, U&& default_value) {
    return lz::find_last_or_default(detail::begin(iterable), detail::end(iterable), std::forward<T>(to_find),
                                    std::forward<U>(default_value));
}

} // namespace lz

#endif
