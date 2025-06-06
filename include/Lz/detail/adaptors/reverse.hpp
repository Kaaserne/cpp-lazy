#pragma once

#ifndef LZ_REVERSE_ADAPTOR_HPP
#define LZ_REVERSE_ADAPTOR_HPP

#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterables/reverse.hpp>

namespace lz {
namespace detail {

template<bool Cached>
struct reverse_adaptor {
    using adaptor = reverse_adaptor;

    /**
     * @brief Reveres the iterable. Contains a size method if the iterable has a size method. Example:
     * ```cpp
     * std::vector<int> v = { 1, 2, 3, 4, 5 };
     * auto reversed = lz::reverse(v); // { 5, 4, 3, 2, 1 }
     * // or
     * auto reversed = v | lz::reverse; // { 5, 4, 3, 2, 1 }
     * auto reversed_cached = v | lz::cached_reverse; // { 5, 4, 3, 2, 1 }
     * // or
     * auto reversed_cached = lz::cached_reverse(v); // { 5, 4, 3, 2, 1 }
     * ```
     * @param iterable The iterable to reverse.
     * @return A (cached_)reverse_iterable object that can be used to iterate over the reversed iterable.
     */
    template<LZ_CONCEPT_BIDIRECTIONAL_ITERABLE Iterable>
    LZ_NODISCARD constexpr reverse_iterable<remove_ref<Iterable>, Cached> operator()(Iterable&& iterable) const {
        return { std::forward<Iterable>(iterable) };
    }
};

} // namespace detail
} // namespace lz

#endif // LZ_REVERSE_ADAPTOR_HPP
