#pragma once

#ifndef LZ_ALGORITHM_FRONT_OR_HPP
#define LZ_ALGORITHM_FRONT_OR_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/procs/begin_end.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * Gets the first element of the sequence, or `default_value` if the sequence is empty.
 * @param iterable The iterable to get the first value of, or `value` in case it is empty.
 * @param default_value The value to return if `iterable` is empty.
 * @return Either the first element of `iterable` or `value` if the sequence is empty.
 */
template<class Iterable, class T>
LZ_NODISCARD constexpr detail::val_iterable_t<Iterable> front_or(Iterable && iterable, T && default_value) {
    auto begin = detail::begin(iterable);
    auto end = detail::end(iterable);
    return begin == end ? std::forward<T>(default_value) : *begin;
}

} // namespace lz
#endif
