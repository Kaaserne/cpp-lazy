#pragma once

#ifndef LZ_MAP_HPP
#define LZ_MAP_HPP

#include <Lz/detail/iterators/map.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

struct map_adaptor {
#ifdef LZ_HAS_CXX_11
    static map_adaptor map;
#endif

    using adaptor = map_adaptor;

    template<class Iterable, class Function>
    constexpr detail::map_iterable<Iterable, detail::decay_t<Function>>
    operator()(Iterable&& iterable, Function&& function) const {
        return { std::forward<Iterable>(iterable), std::forward<Function>(function) };
    }

    template<class Function>
    LZ_CONSTEXPR_CXX_14 detail::fn_args_holder<map_adaptor, detail::decay_t<Function>> operator()(Function&& function) const {
        return { std::forward<Function>(function) };
    }
};

#ifdef LZ_HAS_CXX_11

/**
 * @brief This adaptor is used to apply a function to each element in an iterable. The iterator category is the same as the input
 * iterator category. Its end() function will return a sentinel, if the input iterable has a forward iterator. If its input
 * iterable has a .size() method, then this iterable will also have a .size() method. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto map = vec | lz::map([](int i) { return i * 2; }); // map = { 2, 4, 6, 8, 10 }
 * // or
 * auto map = lz::map(vec, [](int i) { return i * 2; }); // map = { 2, 4, 6, 8, 10 }
 * ```
 */
map_adaptor map_adaptor::map{};

#else

/**
 * @brief This adaptor is used to apply a function to each element in an iterable. The iterator category is the same as the input
 * iterator category. Its end() function will return a sentinel, if the input iterable has a forward iterator. If its input
 * iterable has a .size() method, then this iterable will also have a .size() method. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto map = vec | lz::map([](int i) { return i * 2; }); // map = { 2, 4, 6, 8, 10 }
 * // or
 * auto map = lz::map(vec, [](int i) { return i * 2; }); // map = { 2, 4, 6, 8, 10 }
 * ```
 */
LZ_INLINE_VAR constexpr map_adaptor map{};

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif