#pragma once

#ifndef LZ_MAP_ADAPTOR_HPP
#define LZ_MAP_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/iterables/map.hpp>

namespace lz {
namespace detail {
struct map_adaptor {
    using adaptor = map_adaptor;

    /**
     * @brief This adaptor is used to apply a function to each element in an iterable. The iterator category is the same as the
     * input iterator category. Its end() function will return a sentinel, if the input iterable has a forward iterator or has a
     * sentinel. If its input iterable has a .size() method, then this iterable will also have a .size() method. Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5 };
     * auto map = lz::map(vec, [](int i) { return i * 2; }); // map = { 2, 4, 6, 8, 10 }
     * ```
     * @param iterable The iterable to apply the function to
     * @param function The function to apply to each element in the iterable
     * @return A map_iterable that applies the function to each element in the iterable
     */
    template<class Iterable, class Function>
    constexpr map_iterable<remove_ref<Iterable>, Function> operator()(Iterable&& iterable, Function function) const {
        return { std::forward<Iterable>(iterable), std::move(function) };
    }

    /**
     * @brief This adaptor is used to apply a function to each element in an iterable. The iterator category is the same as the
     * input iterator category. Its end() function will return a sentinel, if the input iterable has a forward iterator or has a
     * sentinel. If its input iterable has a .size() method, then this iterable will also have a .size() method. Example:
     * ```cpp
     * std::vector<int> vec = { 1, 2, 3, 4, 5 };
     * auto map = vec | lz::map([](int i) { return i * 2; }); // map = { 2, 4, 6, 8, 10 }
     * ```
     * @param function The function to apply to each element in the iterable
     * @return An adaptor that can be used in pipe expressions
     */
    template<class Function>
    LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, Function> operator()(Function&& function) const {
        return { std::move(function) };
    }
};
} // namespace detail
} // namespace lz

#endif