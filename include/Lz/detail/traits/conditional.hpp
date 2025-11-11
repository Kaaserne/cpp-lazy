#pragma once

#ifndef LZ_DETAIL_TRAITS_CONDITIONAL_HPP
#define LZ_DETAIL_TRAITS_CONDITIONAL_HPP

namespace lz {
namespace detail {

template<bool B>
struct conditional_impl;

template<>
struct conditional_impl<true> {
    template<class IfTrue, class /* IfFalse */>
    using type = IfTrue;
};

template<>
struct conditional_impl<false> {
    template<class /* IfTrue */, class IfFalse>
    using type = IfFalse;
};

template<bool B, class IfTrue, class IfFalse>
using conditional_t = typename conditional_impl<B>::template type<IfTrue, IfFalse>;

} // namespace detail
} // namespace lz

#endif
