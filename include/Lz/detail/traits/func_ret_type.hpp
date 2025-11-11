#pragma once

#ifndef LZ_DETAIL_TRAITS_FUNC_RET_TYPE_HPP
#define LZ_DETAIL_TRAITS_FUNC_RET_TYPE_HPP

#include <utility>

namespace lz {
namespace detail {

template<class Function, class... Args>
using func_ret_type = decltype(std::declval<Function>()(std::declval<Args>()...));

template<class Function, class Iterator>
using func_ret_type_iter = decltype(std::declval<Function>()(*std::declval<Iterator>()));

template<class Function, class... Iterator>
using func_ret_type_iters = decltype(std::declval<Function>()(*(std::declval<Iterator>())...));

}}

#endif
