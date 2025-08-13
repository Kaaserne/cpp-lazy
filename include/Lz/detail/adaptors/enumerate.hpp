#pragma once

#ifndef LZ_ENUMERATE_ADAPTOR_HPP
#define LZ_ENUMERATE_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/iterables/enumerate.hpp>

namespace lz {
namespace detail {
struct enumerate_adaptor {
    using adaptor = enumerate_adaptor;

    /**
     * @brief Returns an iterable that enumerates the elements of the input iterable, meaning it returns a std::pair<IntType,
     * ValueType>, where std::pair::first_type is an integer (corresponding to the current index) and std::pair::second_type is
     * the value of the input iterable (by reference). The index starts at 0 by default, but can be changed by passing a start
     * value to the function.
     * If the input iterable is exactly bidirectional and not sized (like `lz::filter` for example), the entire sequence is
     * traversed to get its end size (using `lz::eager_size`); this can be inefficient. To prevent this traversal alltogether, you
     * can use `lz::iter_decay` defined in `<Lz/iter_tools.hpp>` or you can use `lz::cache_size` to cache the size of the
     * iterable. `lz::iter_decay` can decay the iterable into a forward one and since forward iterators cannot go backward, its
     * entire size is therefore also not needed to create an iterator from its end() function. `lz::cache_size` however will
     * traverse the iterable once and cache the size, so that subsequent calls to `end()` will not traverse the iterable again,
     * but will return the cached size instead. The following iterables require a(n) (eagerly)sized iterable:
     * - `lz::chunks`
     * - `lz::enumerate`
     * - `lz::exclude`
     * - `lz::interleave`
     * - `lz::rotate`
     * - `lz::take`
     * - `lz::take_every`
     * - `lz::zip_longest`
     * - `lz::zip`
     *
     * Example:
     * ```cpp
     * std::forward_list<int> list = {1, 2, 3, 4, 5};
     * auto enumerated = lz::enumerate(list); // enumerated = { {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5} }
     * auto enumerated = lz::enumerate(list, 5); // enumerated = { {5, 1}, {6, 2}, {7, 3}, {8, 4}, {9, 5} }
     * ```
     * Use lz::cache_size if:
     * - Your iterable is exactly bidirectional (so forward/random access excluded) and
     * - Your iterable is not sized and
     * - Your iterable is not sentinelled
     * - You either use multiple/a combination of the following iterables OR (see last point):
     * - `lz::chunks`
     * - `lz::enumerate`
     * - `lz::exclude`
     * - `lz::interleave`
     * - `lz::rotate`
     * - `lz::take`
     * - `lz::take_every`
     * - `lz::zip_longest`
     * - `lz::zip`
     * - Are planning call end() multiple times on the same instance (with one or more of the above iterable
     * combinations)
     * @param iterable The iterable to enumerate
     * @param start The start index of the enumeration
     * @return An iterable that enumerates the elements of the input iterable
     */
    template<class Iterable, class IntType = int>
    LZ_NODISCARD constexpr enumerate_iterable<remove_ref<Iterable>, IntType>
    operator()(Iterable&& iterable, const IntType start = 0) const {
        return { std::forward<Iterable>(iterable), start };
    }

#ifdef LZ_HAS_CONCEPTS

    /**
     * @brief Returns an iterable that enumerates the elements of the input iterable, meaning it returns a std::pair<IntType,
     * ValueType>, where std::pair::first_type is an integer (corresponding to the current index) and std::pair::second_type is
     * the value of the input iterable (by reference). The index starts at 0 by default, but can be changed by passing a start
     * value to the function.
     *
     * If the input iterable is exactly bidirectional and not sized (like `lz::filter` for example), the entire sequence is
     * traversed to get its end size (using `lz::eager_size`); this can be inefficient. To prevent this traversal alltogether, you
     * can use `lz::iter_decay` defined in `<Lz/iter_tools.hpp>` or you can use `lz::cache_size` to cache the size of the
     * iterable. `lz::iter_decay` can decay the iterable into a forward one and since forward iterators cannot go backward, its
     * entire size is therefore also not needed to create an iterator from its end() function. `lz::cache_size` however will
     * traverse the iterable once and cache the size, so that subsequent calls to `end()` will not traverse the iterable again,
     * but will return the cached size instead. The following iterables require a(n) (eagerly)sized iterable:
     * - `lz::chunks`
     * - `lz::enumerate`
     * - `lz::exclude`
     * - `lz::interleave`
     * - `lz::rotate`
     * - `lz::take`
     * - `lz::take_every`
     * - `lz::zip_longest`
     * - `lz::zip`
     *
     * Example:
     * ```cpp
     * std::forward_list<int> list = {1, 2, 3, 4, 5};
     * auto enumerated = list | lz::enumerate; // enumerated = { {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5} }
     * auto enumerated = list | lz::enumerate(5); // enumerated = { {5, 1}, {6, 2}, {7, 3}, {8, 4}, {9, 5} }
     * ```
     * Use lz::cache_size if:
     * - Your iterable is exactly bidirectional (so forward/random access excluded) and
     * - Your iterable is not sized and
     * - Your iterable is not sentinelled
     * - You either use multiple/a combination of the following iterables OR (see last point):
     * - `lz::chunks`
     * - `lz::enumerate`
     * - `lz::exclude`
     * - `lz::interleave`
     * - `lz::rotate`
     * - `lz::take`
     * - `lz::take_every`
     * - `lz::zip_longest`
     * - `lz::zip`
     * - Are planning call end() multiple times on the same instance (with one or more of the above iterable
     * combinations)
     * @param start The start index of the enumeration
     * @return An adaptor that can be used with a pipe operator
     */
    template<class IntType = int>
    [[nodiscard]] constexpr fn_args_holder<adaptor, IntType> operator()(const IntType start = 0) const
        requires(!is_iterable<IntType>::value)
    {
        return { start };
    }

#else

    /**
     * @brief Returns an iterable that enumerates the elements of the input iterable, meaning it returns a std::pair<IntType,
     * ValueType>, where std::pair::first_type is an integer (corresponding to the current index) and std::pair::second_type is
     * the value of the input iterable (by reference). The index starts at 0 by default, but can be changed by passing a start
     * value to the function.
     *
     * If the input iterable is exactly bidirectional and not sized (like `lz::filter` for example), the entire sequence is
     * traversed to get its end size (using `lz::eager_size`); this can be inefficient. To prevent this traversal alltogether, you
     * can use `lz::iter_decay` defined in `<Lz/iter_tools.hpp>` or you can use `lz::cache_size` to cache the size of the
     * iterable. `lz::iter_decay` can decay the iterable into a forward one and since forward iterators cannot go backward, its
     * entire size is therefore also not needed to create an iterator from its end() function. `lz::cache_size` however will
     * traverse the iterable once and cache the size, so that subsequent calls to `end()` will not traverse the iterable again,
     * but will return the cached size instead. The following iterables require a(n) (eagerly)sized iterable:
     * - `lz::chunks`
     * - `lz::enumerate`
     * - `lz::exclude`
     * - `lz::interleave`
     * - `lz::rotate`
     * - `lz::take`
     * - `lz::take_every`
     * - `lz::zip_longest`
     * - `lz::zip`
     *
     * Example:
     * ```cpp
     * std::forward_list<int> list = {1, 2, 3, 4, 5};
     * auto enumerated = list | lz::enumerate; // enumerated = { {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5} }
     * auto enumerated = list | lz::enumerate(5); // enumerated = { {5, 1}, {6, 2}, {7, 3}, {8, 4}, {9, 5} }
     * ```
     * Use lz::cache_size if:
     * - Your iterable is exactly bidirectional (so forward/random access excluded) and
     * - Your iterable is not sized and
     * - Your iterable is not sentinelled
     * - You either use multiple/a combination of the following iterables OR (see last point):
     * - `lz::chunks`
     * - `lz::enumerate`
     * - `lz::exclude`
     * - `lz::interleave`
     * - `lz::rotate`
     * - `lz::take`
     * - `lz::take_every`
     * - `lz::zip_longest`
     * - `lz::zip`
     * - Are planning call end() multiple times on the same instance (with one or more of the above iterable
     * combinations)
     * @param start The start index of the enumeration
     * @return An adaptor that can be used with a pipe operator
     */
    template<class IntType = int>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_iterable<IntType>::value, fn_args_holder<adaptor, IntType>>
    operator()(const IntType start = 0) const {
        return { start };
    }

#endif
};
} // namespace detail
} // namespace lz

#endif // LZ_ENUMERATE_ADAPTOR_HPP
