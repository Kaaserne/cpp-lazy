#pragma once

#ifndef LZ_LOOP_ADAPTOR_HPP
#define LZ_LOOP_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/iterables/loop.hpp>

namespace lz {
namespace detail {
struct loop_n_adaptor {
    using adaptor = loop_n_adaptor;

    template<class Iterable>
    LZ_NODISCARD constexpr loop_iterable<Iterable, true> operator()(Iterable&& iterable) const {
        return { std::forward<Iterable>(iterable) };
    }
    template<class Iterable>
    LZ_NODISCARD constexpr loop_iterable<Iterable, false> operator()(Iterable&& iterable, const std::size_t amount) const {
        return { std::forward<Iterable>(iterable), amount };
    }

    LZ_NODISCARD constexpr fn_args_holder<adaptor, std::size_t> operator()(const std::size_t amount) const {
        return { amount };
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_LOOP_ADAPTOR_HPP
