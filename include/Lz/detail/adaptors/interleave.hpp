#pragma once

#ifndef LZ_INTERLEAVED_ADAPTOR_HPP
#define LZ_INTERLEAVED_ADAPTOR_HPP

#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterables/interleave.hpp>

namespace lz {
namespace detail {

struct interleave_adaptor {
    using adaptor = interleave_adaptor;

    // TODO docs
    template<LZ_CONCEPT_ITERABLE Iterable, LZ_CONCEPT_ITERABLE... Iterables>
    LZ_NODISCARD constexpr interleave_iterable<remove_ref<Iterable>, remove_ref<Iterables>...>
    operator()(Iterable&& iterable, Iterables&&... iterables) const {
        return { std::forward<Iterable>(iterable), std::forward<Iterables>(iterables)... };
    }
};

} // namespace detail
} // namespace lz

#endif // LZ_INTERLEAVED_ADAPTOR_HPP
