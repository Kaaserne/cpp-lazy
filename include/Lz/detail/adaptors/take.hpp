#pragma once

#ifndef LZ_TAKE_ADAPTOR_HPP
#define LZ_TAKE_ADAPTOR_HPP

#include <Lz/detail/iterables/take.hpp>
#include <Lz/detail/adaptors/fn_args_holder.hpp>

namespace lz {
namespace detail {
struct take_adaptor {
    using adaptor = take_adaptor;

    template<class Iterable>
    constexpr take_iterable<Iterable> operator()(Iterable&& iterable, const diff_iterable_t<Iterable> n) const {
        return { std::forward<Iterable>(iterable), n };
    }

    template<class DiffT>
    constexpr fn_args_holder<adaptor, DiffT> operator()(const DiffT n) const {
        return { n };
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_TAKE_ADAPTOR_HPP