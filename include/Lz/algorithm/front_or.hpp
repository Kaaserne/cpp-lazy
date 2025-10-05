#pragma once

#ifndef LZ_ALGORITHM_FRONT_OR_HPP
#define LZ_ALGORITHM_FRONT_OR_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/procs/begin_end.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>

LZ_MODULE_EXPORT namespace lz {

template<class Iterator, class S, class T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::val_t<Iterator> front_or(Iterator begin, S end, T&& default_value) {
    return begin == end ? std::forward<T>(default_value) : *begin;
}

/**
 * Gets the first element of the sequence, or `default_value` if the sequence is empty.
 * @param iterable The iterable to get the first value of, or `value` in case it is empty.
 * @param default_value The value to return if `iterable` is empty.
 * @return Either the first element of `iterable` or `value` if the sequence is empty.
 */
template<class Iterable, class T>
LZ_NODISCARD constexpr detail::val_iterable_t<Iterable> front_or(Iterable&& iterable, T&& default_value) { // TODO common_ref?
    return front_or(detail::begin(iterable), detail::end(iterable), std::forward<T>(default_value));
}
} // namespace lz
#endif
