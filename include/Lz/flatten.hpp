#ifndef LZ_FLATTEN_HPP
#define LZ_FLATTEN_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/flatten.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Flattens a nested iterable. For instance a vector of vectors will be flattened to a single iterable. Returns a
 * bidirectional iterable if the input is bidirectional, otherwise the same as the input iterable. If its input iterable is
 * forward or less, the end iterator will be a sentinel. Contains a .size() method if all the input iterables are sized. Example:
 * ```cpp
 * std::vector<std::vector<int>> vectors = { { 1, 2, 3 }, { 4, 5, 6 }, { 7 } };
 * auto flattened = lz::flatten(vectors); // { 1, 2, 3, 4, 5, 6, 7 }
 * // or
 * auto flattened = vectors | lz::flatten; // { 1, 2, 3, 4, 5, 6, 7 }
 * ```
 */
LZ_INLINE_VAR constexpr detail::flatten_adaptor flatten{};

/**
 * @brief Flatten iterable helper alias.
 * @tparam Iterable The type of the iterable to flatten.
 * ```cpp
 * std::vector<std::vector<int>> vectors = { { 1, 2, 3 }, { 4, 5, 6 }, { 7 } };
 * lz::flatten_iterable<std::vector<std::vector<int>>> flattened = lz::flatten(vectors);
 * ```
 */
template<class Iterable>
using flatten_iterable = detail::flatten_iterable<Iterable, dimensions<Iterable>::value - !std::is_array<Iterable>::value>;

} // namespace lz

#endif // LZ_FLATTEN_HPP
