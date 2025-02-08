#pragma once

#ifndef LZ_TAKE_EVERY_ADAPTOR_HPP
#define LZ_TAKE_EVERY_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/iterables/take_every.hpp>

namespace lz {
namespace detail {
struct take_every_adaptor {
    using adaptor = take_every_adaptor;

    template<class Iterable>
    LZ_NODISCARD constexpr take_every_iterable<Iterable>
    operator()(Iterable&& iterable, const diff_iterable_t<Iterable> offset) const {
        return { std::forward<Iterable>(iterable), offset };
    }

    LZ_NODISCARD constexpr fn_args_holder<adaptor, std::ptrdiff_t> operator()(const std::ptrdiff_t offset) const {
        return { offset };
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_TAKE_EVERY_ADAPTOR_HPP