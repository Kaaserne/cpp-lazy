#pragma once

#ifndef LZ_INTERLEAVE_LONGEST_ADAPTOR_HPP
#define LZ_INTERLEAVE_LONGEST_ADAPTOR_HPP

#include <Lz/detail/iterables/interleave_longest.hpp>

namespace lz {
namespace detail {

struct interleave_longest_adaptor {
    using adaptor = interleave_longest_adaptor;

    template<class... Iterables>
    constexpr auto operator()(Iterables&&... iterables) const {
        return interleave_longest_iterable<std::decay_t<Iterables>...>{ std::forward<Iterables>(iterables)... };
    }
};

} // namespace detail
} // namespace lz

#endif // LZ_INTERLEAVE_LONGEST_ADAPTOR_HPP
