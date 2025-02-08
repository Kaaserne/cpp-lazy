#pragma once

#ifndef LZ_EXCLUSIVE_SCAN_ADAPTOR_HPP
#define LZ_EXCLUSIVE_SCAN_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/iterables/exclusive_scan.hpp>

namespace lz {
namespace detail {
struct exclusive_scan_adaptor {
    using adaptor = exclusive_scan_adaptor;

    template<class Iterable, class T = val_iterable_t<Iterable>, class BinaryOp = MAKE_BIN_PRED(plus)>
    LZ_NODISCARD constexpr enable_if<is_invocable<BinaryOp, ref_iterable_t<Iterable>, decay_t<T>>::value,
                                     exclusive_scan_iterable<Iterable, decay_t<T>, BinaryOp>>
    operator()(Iterable&& iterable, T&& init = {}, BinaryOp binary_op = {}) const {
        return { std::forward<Iterable>(iterable), std::forward<T>(init), std::move(binary_op) };
    }

    // clang-format off
    template<class T, class BinaryOp = MAKE_BIN_PRED(plus)>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14
    enable_if<is_invocable<BinaryOp, decay_t<T>, decay_t<T>>::value, fn_args_holder<adaptor, decay_t<T>, BinaryOp>>
    operator()(T&& init, BinaryOp binary_op = {}) const {
        return { std::forward<T>(init), std::move(binary_op) };
    }
    // clang-format on
};
} // namespace detail
} // namespace lz
#endif
