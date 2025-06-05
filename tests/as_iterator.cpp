#include <Lz/as_iterator.hpp>
#include <Lz/c_string.hpp>
#include <catch2/catch.hpp>
// TODO add static_assert default constructors
TEST_CASE("With sentinels") {
    SECTION("Equal to c_string") {
        auto cstr = lz::c_string("Hello, World!");
        lz::string_view expected = "Hello, World!";
        auto it = lz::as_iterator(cstr);
        using iterator = decltype(*it.begin());

        REQUIRE(lz::equal(it, expected, [](iterator iter, char c) { return *iter == c; }));
    }

    SECTION("Equal to random access iterable") {
        const std::vector<int> vec = { 1, 2, 3, 4, 5 };
        auto expected = { 1, 2, 3, 4, 5 };
        auto it = vec | lz::as_iterator(std::random_access_iterator_tag{});
        using iterator = decltype(*it.begin());
        REQUIRE(lz::equal(it, expected, [](iterator iter, int value) { return *iter == value; }));

        lz::as_iterator_iterable<const std::vector<int>, std::forward_iterator_tag> as_iterable =
            vec | lz::as_iterator(std::forward_iterator_tag{});
        static_assert(std::is_same<decltype(as_iterable.begin()), decltype(as_iterable.end())>::value,
                      "begin and end should return the same type");
        static_assert(!lz::detail::is_ra<decltype(as_iterable.begin())>::value,
                      "as_iterator_iterable should return a random access iterator");
    }

    SECTION("Operator minus basics") {
        const std::vector<int> vec = { 1, 2, 3, 4, 5 };
        auto it = vec | lz::as_iterator;
        auto first = it.begin();
        auto last = it.end();
        REQUIRE(last - first == 5);
        REQUIRE(first - last == -5);
    }

    SECTION("operator minus with number") {
        const std::vector<int> vec = { 1, 2, 3, 4, 5 };
        auto it = lz::as_iterator(vec);
        auto first = it.begin();
        auto last = it.end();
        REQUIRE(first + 2 == std::next(first, 2));
        REQUIRE(last - 2 == std::next(last, -2));
    }
}
