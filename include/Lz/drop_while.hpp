#pragma once

#ifndef LZ_DROP_WHILE_HPP
#define LZ_DROP_WHILE_HPP

#include <Lz/detail/adaptors/drop_while.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief This adaptor is used to make an iterable where the iterator keeps dropping elements as long as the predicate returns
 * true. Once it has returned false, it will no longer do such thing. The iterator category is the same as its input iterable. Its
 * end() function will return the same type as its begin() function, regardless of the iterator category. If its input iterable is
 * random access, then this iterable will have a .size() method. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto dropped = lz::drop_while(vec, [](int i) { return i < 3; }); // dropped = { 3, 4, 5 }
 * // or
 * auto dropped = vec | lz::drop_while([](int i) { return i < 3; }); // dropped = { 3, 4, 5 }
 * ```
 */
detail::drop_while_adaptor detail::drop_while_adaptor::drop_while{};

#else

/**
 * @brief This adaptor is used to make an iterable where the iterator keeps dropping elements as long as the predicate returns
 * `true`. Once it has returned `false`, it will no longer do such thing. The iterator category is the same as its input iterable. Its
 * end() function will return the same type as its begin() function, regardless of the iterator category. If its input iterable is
 * random access, then this iterable will have a .size() method. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto dropped = lz::drop_while(vec, [](int i) { return i < 3; }); // dropped = { 3, 4, 5 }
 * // or
 * auto dropped = vec | lz::drop_while([](int i) { return i < 3; }); // dropped = { 3, 4, 5 }
 * ```
 */
LZ_INLINE_VAR constexpr detail::drop_while_adaptor drop_while{};

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_DROP_WHILE_HPP