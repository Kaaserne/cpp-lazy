#pragma once

#ifndef LZ_REPEAT_ADAPTOR_HPP
#define LZ_REPEAT_ADAPTOR_HPP

#include <Lz/detail/iterables/repeat.hpp>

namespace lz {
namespace detail {
struct repeat_adaptor {
    using adaptor = repeat_adaptor;

    template<class T>
    LZ_NODISCARD constexpr repeat_iterable<T> operator()(T value, std::size_t amount) const {
        return { std::move(value), amount };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, std::size_t> operator()(std::size_t amount) const noexcept {
        return { amount };
    }
};
} // namespace detail
} // namespace lz
#endif