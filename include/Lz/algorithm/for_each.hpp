#pragma once

#ifndef LZ_ALGORITHM_FOR_EACH_HPP
#define LZ_ALGORITHM_FOR_EACH_HPP

#include <Lz/detail/algorithm/for_each.hpp>
#include <Lz/detail/traits/is_sentinel.hpp>

LZ_MODULE_EXPORT namespace lz {

template<class Iterable, class Func>
LZ_CONSTEXPR_CXX_14 void for_each(Iterable&& iterable, Func func) {
    detail::for_each(detail::begin(iterable), detail::end(iterable), std::move(func));
}

} // namespace lz

#endif // LZ_ALGORITHM_FOR_EACH_HPP
