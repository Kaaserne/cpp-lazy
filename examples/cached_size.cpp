#include <Lz/cached_size.hpp>
#include <Lz/filter.hpp>
#include <Lz/take_every.hpp>
#include <Lz/zip.hpp>
#include <iostream>
#include <vector>

int main() {
    /**
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
     */
    std::vector<int> to_filter = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    // filter isn't sized; it does not contain size method
    auto filtered = lz::filter(to_filter, [](int i) { return i % 2 == 0; }); // {0, 2, 4, 6, 8}
    // const auto size = filtered.size(); // ERROR filtered.size() does not compile, since it is not sized

    // Here, filtered is traversed only once O(n) to get the size, which is 5, then cached
    auto filtered_cached_size = filtered | lz::cache_size; // filtered_cached_size now contains a size method
    // Therefore, calling size() is O(1)
    std::cout << "Size of filtered (O(1)): " << filtered_cached_size.size() << '\n'; // 5, cheap operation

    // lz::zip also requires a size, so it will use the cached size, instead of traversing the filtered iterable again
    auto zipped = lz::zip(filtered_cached_size, filtered_cached_size); // O(1) operation when calling end()
    auto zipped_slow = lz::zip(filtered, filtered);                    // O(n) operation when calling end()

    std::cout << "Size of zipped (O(1)): " << zipped.size() << '\n'; // 5, cheap operation
    // Because filtered isn't sized, the example below will not compile
    // std::cout << "Size of zipped_slow: " << zipped_slow.size() << '\n';
    // instead you can use lz::eager_size
    std::cout << "Size of zipped_slow using eager size (O(n)): " << lz::eager_size(zipped_slow) << '\n'; // 5, O(n) operation

    // Again, zipped is sized because filtered_cached_size is sized
    auto take_every = lz::take_every(zipped, 2); // O(1) operation when calling end(); { {0, 2}, {4, 6}, {8, 10} }
    std::cout << "Size of take_every (O(1)): " << take_every.size() << '\n'; // 3, cheap operation

    // zipped_slow is not sized, so it will traverse the entire zipped_slow iterable
    auto take_every_slow = lz::take_every(zipped_slow, 2); // O(n) operation; { {0, 0}, {2, 2}, {4, 4}, {6, 6}, {8, 8} }
    std::cout << "Size of take_every_slow (O(n)): " << lz::eager_size(take_every_slow) << '\n'; // 5, O(n) operation

    /** Use lz::cache_size if:
     * - Your iterable is exactly bidirectional (so forward and random access excluded) and;
     * - Your iterable is not sized and (like lz::filter) either OR
     * - You use *multiple* / a combination of the following iterables:
     * - Your iterable is not sentinelled
     *   - `lz::chunks`
     *   - `lz::enumerate`
     *   - `lz::exclude`
     *   - `lz::interleave`
     *   - `lz::rotate`
     *   - `lz::take`
     *   - `lz::take_every`
     *   - `lz::zip_longest`
     *   - `lz::zip`
     * - OR Are planning to call begin() or end() multiple times on the same instance (with one or more of the above iterable
     * combinations)
     */
    // So, by calling .end() on the same instance, each time .end() is called, the size is calculated again
    auto zipped_non_cached = lz::zip(to_filter, to_filter);
    auto end = zipped_non_cached.end(); // O(n) operation, since to_filter is not sized and bidirectional

    static_cast<void>(end); // to avoid unused variable warning

    end = zipped_non_cached.end(); // another O(n) operation, since to_filter is not sized and bidirectional

    static_cast<void>(end); // to avoid unused variable warning
}
