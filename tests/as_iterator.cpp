#include <Lz/as_iterator.hpp>
#include <Lz/c_string.hpp>
#include <Lz/pairwise.hpp>
#include <Lz/repeat.hpp>
#include <Lz/reverse.hpp>
#include <Lz/string_view.hpp>
#include <cpp-lazy-ut-helper/pch.hpp>
#include <cpp-lazy-ut-helper/test_procs.hpp>
#include <cpp-lazy-ut-helper/ut_helper.hpp>
#include <doctest/doctest.h>

// TODO reduce includes, make smaller files

TEST_CASE("operator=(default_sentinel_t)") {
    SUBCASE("forward") {
        auto cstr = lz::c_string("hello, world!");
        auto as_it = lz::as_iterator(cstr);
        auto common = make_sentinel_assign_op_tester(as_it);
        auto expected = { 'h', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!' };
        REQUIRE(lz::equal(common, expected, [](decltype(*common.begin()) a, char b) { return *a == b; }));
    }

    SUBCASE("bidirectional") {
        std::list<int> lst = { 1, 2, 3, 4, 5 };
        auto sentinelled = make_sized_bidi_sentinelled(lst);
        auto common = make_sentinel_assign_op_tester(lz::as_iterator(sentinelled));
        auto expected = { 1, 2, 3, 4, 5 };
        REQUIRE(lz::equal(common, expected, [](decltype(*common.begin()) a, int b) { return *a == b; }));
        REQUIRE(
            lz::equal(common | lz::reverse, expected | lz::reverse, [](decltype(*common.begin()) a, int b) { return *a == b; }));
    }

    SUBCASE("random access") {
        auto repeat = lz::repeat(20, 5);
        auto as_it = make_sentinel_assign_op_tester(lz::as_iterator(repeat));
        auto expected = { 20, 20, 20, 20, 20 };
        REQUIRE(lz::equal(as_it, expected, [](decltype(*as_it.begin()) a, int b) { return *a == b; }));
        REQUIRE(
            lz::equal(as_it | lz::reverse, expected | lz::reverse, [](decltype(*as_it.begin()) a, int b) { return *a == b; }));
        test_procs::test_operator_minus(as_it);
        test_procs::test_operator_plus(as_it, expected, [](decltype(*as_it.begin()) a, int b) { return *a == b; });
    }
}

TEST_CASE("With sentinels") {
    SUBCASE("Equal to c_string") {
        auto cstr = lz::c_string("Hello, World!");
        lz::string_view expected = "Hello, World!";
        auto it = lz::as_iterator(cstr);
        using iterator = decltype(*it.begin());

        REQUIRE(lz::equal(it, expected, [](iterator iter, char c) { return *iter == c; }));
    }

    SUBCASE("Equal to random access iterable") {
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

    SUBCASE("Operator minus basics") {
        const std::vector<int> vec = { 1, 2, 3, 4, 5 };
        auto it = vec | lz::as_iterator;
        auto first = it.begin();
        auto last = it.end();
        REQUIRE(last - first == 5);
        REQUIRE(first - last == -5);
    }

    SUBCASE("operator minus with number") {
        const std::vector<int> vec = { 1, 2, 3, 4, 5 };
        auto it = lz::as_iterator(vec);
        auto first = it.begin();
        auto last = it.end();
        REQUIRE(first + 2 == std::next(first, 2));
        REQUIRE(last - 2 == std::next(last, -2));
    }
}
