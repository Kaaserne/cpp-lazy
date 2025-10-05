#pragma once

#ifndef LZ_ALGORITHM_FOR_EACH_HPP
#define LZ_ALGORITHM_FOR_EACH_HPP

#include <Lz/detail/algorithm/for_each.hpp>
#include <Lz/detail/traits/is_sentinel.hpp>

LZ_MODULE_EXPORT namespace lz {
    template<class Iterator, class S, class Func>
    void for_each(Iterator begin, S end, Func func) {
        if constexpr (detail::is_sentinel_v<Iterator, S>) {
            detail::algorithm::for_each(begin, end, func);
        }
        else {
            std::for_each(begin, end, func);
        }
    }

    /**
     * @brief Applies a function to each element in an iterable.
     * 
     * @param iterable The iterable to process.
     * @param func The function to apply to each element.
     */
    template<class Iterable, class Func>
    void for_each(const Iterable& iterable, Func func) {
        using std::for_each;
        detail::algorithm::for_each(detail::begin(iterable), detail::end(iterable), func);
    }
}

#endif // LZ_ALGORITHM_FOR_EACH_HPP
