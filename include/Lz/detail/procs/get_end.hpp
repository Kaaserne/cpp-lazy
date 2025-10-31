#pragma once

#ifndef LZ_DETAIL_PROCS_GET_END_HPP
#define LZ_DETAIL_PROCS_GET_END_HPP


namespace lz {
namespace detail {

template<class Iterator, class S>
LZ_NODISCARD constexpr Iterator get_end(Iterator begin, S end) {
    return begin + (end - begin);
}

template<class Iterator>
LZ_NODISCARD constexpr Iterator get_end(Iterator, Iterator end) {
    return end;
}

} // namespace detail
} // namespace lz

#endif
