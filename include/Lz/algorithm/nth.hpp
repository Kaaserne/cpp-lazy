#pragma once

#ifndef LZ_ALGORITHM_NTH_HPP
#define LZ_ALGORITHM_NTH_HPP

#include <Lz/detail/procs/next_fast.hpp>
#include <Lz/traits/iter_type.hpp>

LZ_MODULE_EXPORT namespace lz {

    /**
     * @brief Returns an iterator to the n-th element of the iterable.
     * 
     * @param iterable The iterable to get the n-th element from
     * @param n The index of the element to get, starting from 0
     * @return An iterator to the n-th element of the iterable
     */
    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iter_t<Iterable> nth(Iterable&& iterable, detail::diff_iterable_t<Iterable> n) {
        return detail::next_fast(std::forward<Iterable>(iterable), n);
    }
}
#endif
