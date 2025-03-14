#pragma once

#ifndef LZ_REVERSE_ADAPTOR_HPP
#define LZ_REVERSE_ADAPTOR_HPP

#include <Lz/detail/iterables/reverse.hpp>

namespace lz {
namespace detail {
struct reverse_adaptor {
    using adaptor = reverse_adaptor;

#ifdef LZ_HAS_CXX_11

    static constexpr adaptor reverse{};

#endif

    /**
     * @brief Reveres the iterable. Contains a size method if the iterable has a size method. Example:
     * ```cpp
     * std::vector<int> v = { 1, 2, 3, 4, 5 };
     * auto reversed = lz::reverse(v); // { 5, 4, 3, 2, 1 }
     * // or
     * auto reversed = v | lz::reverse; // { 5, 4, 3, 2, 1 }
     * ```
     * @param iterable
     */
    template<class Iterable>
    LZ_NODISCARD constexpr reverse_iterable<Iterable> operator()(Iterable&& iterable) const {
        return { std::forward<Iterable>(iterable) };
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_REVERSE_ADAPTOR_HPP
