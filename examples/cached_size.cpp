#include <Lz/cached_size.hpp>
#include <Lz/filter.hpp>
#include <Lz/take_every.hpp>
#include <Lz/zip.hpp>
#include <iostream>
#include <vector>

int main() {
    /**
     * The following iterables require a distance/size of an iterable:
     * - `lz::chunks`
     * - `lz::enumerate`
     * - `lz::exclude`
     * - `lz::interleaved`
     * - `lz::take`
     * - `lz::take_every`
     * - `lz::zip_longest`
     * - `lz::zip`
     * If your iterable is exactly bidirectional (so forward/random access excluded) and not sized (like `lz::filter` for example,
     * if the input iterable is also bidirectional / random access), you may cache the size of the iterable. If you use multiple
     * of these, it can be handy to cache the size. The size will be calculated when calling begin()/end() on the iterable. So if
     * you call begin() multiple times, the size will be calculated multiple times.
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

    /** So, all in all: use lz::cache_size if all of the following are true:
     * - Your iterable is exactly bidirectional (so forward and random access excluded)
     * - Your iterable is not sized (like lz::filter)
     * - You use *multiple* / a combination of the following iterables:
     *   - lz::chunks
     *   - lz::enumerate
     *   - lz::exclude
     *   - lz::interleaved
     *   - lz::take
     *   - lz::take_every
     *   - lz::zip_longest
     *   - lz::zip
     * - Are planning to call begin() or end() multiple times
     */
}
