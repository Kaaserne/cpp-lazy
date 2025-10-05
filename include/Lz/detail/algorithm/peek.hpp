#pragma once

#ifndef LZ_DETAIL_ALGORITHM_PEEK_HPP
#define LZ_DETAIL_ALGORITHM_PEEK_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/traits/remove_ref.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>
#include <Lz/util/optional.hpp>

namespace lz {
namespace detail {
namespace algorithm {

#ifdef LZ_HAS_CXX_17

template<class Iterator>
constexpr auto get_value_or_reference(Iterator begin) {
    if constexpr (std::is_lvalue_reference_v<ref_t<Iterator>>) {
        return optional<std::reference_wrapper<remove_ref_t<ref_t<Iterator>>>>{ *begin };
    }
    else {
        return optional<val_t<Iterator>>{ *begin };
    }
}

#else

template<class Iterator>
LZ_CONSTEXPR_CXX_14
    enable_if_t<std::is_lvalue_reference<ref_t<Iterator>>::value, optional<std::reference_wrapper<remove_ref_t<ref_t<Iterator>>>>>
    get_value_or_reference(Iterator begin) {
    return std::reference_wrapper<remove_ref_t<ref_t<Iterator>>>{ *begin };
}

template<class Iterator>
LZ_CONSTEXPR_CXX_14 enable_if_t<!std::is_lvalue_reference<ref_t<Iterator>>::value, optional<val_t<Iterator>>>
get_value_or_reference(Iterator begin) {
    return optional<val_t<Iterator>>{ *begin };
}

#endif

template<class Iterator, class S>
LZ_CONSTEXPR_CXX_14 auto peek(Iterator begin, S end) -> decltype(get_value_or_reference(begin)) {
    if (begin == end) {
        return nullopt;
    }
    ++begin;
    if (begin == end) {
        return nullopt;
    }
    return get_value_or_reference(begin);
}

} // namespace algorithm
} // namespace detail
} // namespace lz

#endif
