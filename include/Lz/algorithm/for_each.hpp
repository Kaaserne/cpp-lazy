#pragma once

#ifndef LZ_ALGORITHM_FOR_EACH_HPP
#define LZ_ALGORITHM_FOR_EACH_HPP

#include <Lz/detail/algorithm/for_each.hpp>
#include <Lz/detail/traits/is_sentinel.hpp>

LZ_MODULE_EXPORT namespace lz {

#ifdef LZ_HAS_CXX_17

template<class Iterator, class S, class Func>
void for_each(Iterator begin, S end, Func func) {
    if constexpr (detail::is_sentinel_v<Iterator, S>) {
        detail::algorithm::for_each(std::move(begin), std::move(end), std::move(func));
    }
    else {
        static_cast<void>(std::for_each(std::move(begin), std::move(end), std::move(func)));
    }
}

#else

template<class Iterator, class S, class Func>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::enable_if_t<!detail::is_sentinel<Iterator, S>::value>
for_each(Iterator begin, S end, Func func) {
    static_cast<void>(std::for_each(std::move(begin), std::move(end), std::move(func)));
}

template<class Iterator, class S, class Func>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::enable_if_t<detail::is_sentinel<Iterator, S>::value>
for_each(Iterator begin, S end, Func func) {
    detail::algorithm::for_each(std::move(begin), std::move(end), std::move(func));
}

#endif

/**
 * @brief Applies a function to each element in an iterable.
 *
 * @param iterable The iterable to process.
 * @param func The function to apply to each element.
 */
template<class Iterable, class Func>
void for_each(Iterable&& iterable, Func func) { // TODO replace lz:: and remove && with std::move, std::forward
    lz::for_each(detail::begin(iterable), detail::end(iterable), std::move(func));
}
} // namespace lz

#endif // LZ_ALGORITHM_FOR_EACH_HPP
