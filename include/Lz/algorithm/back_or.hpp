#pragma once

#ifndef LZ_ALGORITHM_BACK_OR_HPP
#define LZ_ALGORITHM_BACK_OR_HPP

#include <Lz/algorithm/back.hpp>
#include <Lz/algorithm/empty.hpp>

LZ_MODULE_EXPORT namespace lz {

template<class Iterator, class T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::val_t<Iterator> back_or(Iterator begin, Iterator end, T&& default_value) {
    return lz::empty(begin, end) ? std::forward<T>(default_value) : lz::back(std::move(begin), std::move(end));
}

/**
 * This function returns the end element. If the sequence is empty, it returns `value`.
 * @param iterable The iterable to get the end value of, or `value` in case it is empty.
 * @param default_value The value to return if `iterable` is empty.
 * @return Either the end element of `iterable` or `value` if the sequence is empty.
 */
template<class Iterable, class T>
LZ_NODISCARD constexpr detail::val_iterable_t<Iterable> back_or(Iterable&& iterable, T&& default_value) {
    return back_or(detail::begin(iterable), detail::end(iterable), std::forward<T>(default_value));
}

} // namespace lz

#endif
