#pragma once

#ifndef LZ_INCLUSIVE_SCAN_ADAPTOR_HPP
#define LZ_INCLUSIVE_SCAN_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/iterables/inclusive_scan.hpp>

namespace lz {
namespace detail {
struct inclusive_scan_adaptor {
    using adaptor = inclusive_scan_adaptor;

    // clang-format off

    template<class Iterable, class T = val_iterable_t<Iterable>, class BinaryOp = MAKE_BIN_PRED(plus)>
    LZ_NODISCARD constexpr 
    enable_if<is_invocable<BinaryOp, decay_t<T>, decay_t<T>>::value, inclusive_scan_iterable<Iterable, decay_t<T>, BinaryOp>>
    operator()(Iterable&& iterable, T init = {}, BinaryOp binary_op = {}) const {
        return { std::forward<Iterable>(iterable), std::move(init), std::move(binary_op) };
    }

    template<class T, class BinaryOp = MAKE_BIN_PRED(plus)>
    LZ_NODISCARD constexpr 
    enable_if<is_invocable<BinaryOp, decay_t<T>, decay_t<T>>::value, fn_args_holder<adaptor, decay_t<T>, BinaryOp>>
    operator()(T&& init, BinaryOp binary_op = {}) const {
        return { std::forward<T>(init), std::move(binary_op) };
    }

    // clang-format on
};
} // namespace detail
} // namespace lz

#endif