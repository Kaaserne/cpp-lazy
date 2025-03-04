#pragma once

#ifndef LZ_LOOP_ADAPTOR_HPP
#define LZ_LOOP_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterables/loop.hpp>

namespace lz {
namespace detail {
struct loop_adaptor {
    using adaptor = loop_adaptor;

#ifdef LZ_HAS_CXX_11

    static constexpr adaptor loop{};

#endif

    /**
     * @brief Loops over an iterable infinately. Does not contain a .size() method. Loop infinitely over the input iterable. Its
     * input iterator category will always be forward. It also returns a default_sentinel. Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4 };
     * auto looper = lz::loop(vec); // {1, 2, 3, 4, 1, 2, 3, 4, 1, 2, ...}
     * // or
     * auto looper = vec | lz::loop; // {1, 2, 3, 4, 1, 2, 3, 4, 1, 2, ...}
     * ```
     * @param iterable The iterable to loop over
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD constexpr detail::enable_if<!std::is_integral<Iterable>::value, loop_iterable<Iterable, true>>
    operator()(Iterable&& iterable) const {
        return { std::forward<Iterable>(iterable) };
    }

    /**
     * @brief Loops n times over an iterable. Contains a .size() method if the input iterable has a .size()
     * method. Will return an actual iterator if the input iterable is at least bidirectional. Otherwise it will return a
     * default_sentinel. Its input iterator category will be the same as the input iterator category. Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4 };
     * auto looper = lz::loop(vec, 2); // {1, 2, 3, 4, 1, 2, 3, 4}
     * // or
     * auto looper = vec | lz::loop(2); // {1, 2, 3, 4, 1, 2, 3, 4}
     * ```
     * @param iterable The iterable to loop over
     * @param amount The amount of times to loop over the iterable
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD constexpr loop_iterable<Iterable, false> operator()(Iterable&& iterable, const std::size_t amount) const {
        return { std::forward<Iterable>(iterable), amount };
    }

    /**
     * @brief Loops n times over an iterable. Contains a .size() method if the input iterable has a
     * .size() method. Will return an actual iterator if the input iterable is at least bidirectional. Otherwise it will return a
     * default_sentinel. Its input iterator category will be the same as the input iterator category. Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4 };
     * auto looper = vec | lz::loop(2); // {1, 2, 3, 4, 1, 2, 3, 4}
     * ```
     * @param iterable The iterable to loop over
     * @param amount The amount of times to loop over the iterable
     */
    LZ_NODISCARD constexpr fn_args_holder<adaptor, std::size_t> operator()(const std::size_t amount) const {
        return { amount };
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_LOOP_ADAPTOR_HPP
