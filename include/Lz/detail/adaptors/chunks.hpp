#pragma once

#ifndef LZ_CHUNKS_ADAPTOR_HPP
#define LZ_CHUNKS_ADAPTOR_HPP

#include <Lz/detail/iterables/chunks.hpp>

namespace lz {
namespace detail {

struct chunks_adaptor {
#ifdef LZ_HAS_CXX_11
    static chunks_adaptor chunks;
#endif

    using adaptor = chunks_adaptor;

    template<LZ_CONCEPT_ITERABLE Iterable>
    constexpr detail::chunks_iterable<Iterable> operator()(Iterable&& iterable, std::size_t chunk_size) const {
        return { std::forward<Iterable>(iterable), chunk_size };
    }

    constexpr detail::fn_args_holder<adaptor, std::size_t> operator()(std::size_t chunk_size) const {
        return { chunk_size };
    }
};

} // namespace detail
} // namespace lz

#endif