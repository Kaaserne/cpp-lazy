#include <Lz/any_iterable.hpp>
#include <Lz/enumerate.hpp>
#include <Lz/iter_tools.hpp>
#include <Lz/take.hpp>
#include <catch2/catch.hpp>
#include <cpp-lazy-ut-helper/c_string.hpp>
#include <list>
#include <test_procs.hpp>

TEST_CASE("Any iterable with sentinels") {
    lz::any_iterable<char, const char&> iterable = "Hello, World!" | lz::c_string;
    auto expected = "Hello, World!";
    REQUIRE(lz::equal(iterable, lz::c_string(expected)));
}

TEST_CASE("Empty or one element any iterable") {
    SECTION("Empty") {
        std::vector<int> vec;
        lz::any_iterable<int, int&> iterable = vec;
        REQUIRE(lz::empty(iterable));
        REQUIRE_FALSE(lz::has_one(iterable));
        REQUIRE_FALSE(lz::has_many(iterable));
    }

    SECTION("One element") {
        std::vector<int> vec = { 1 };
        lz::any_iterable<int, int&> iterable = vec;
        REQUIRE_FALSE(lz::empty(iterable));
        REQUIRE(lz::has_one(iterable));
        REQUIRE_FALSE(lz::has_many(iterable));
    }
}

TEST_CASE("Creating a basic any iterable from std::vector, random access iterator") {
    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    lz::any_iterable<int, int&, std::random_access_iterator_tag> view = vec;
    auto expected = { 1, 2, 3, 4, 5 };

    REQUIRE(lz::equal(view, expected));
    REQUIRE(lz::equal(view | lz::reverse, expected | lz::reverse));

    test_procs::test_operator_plus(view, expected);
    test_procs::test_operator_minus(view);
}

TEST_CASE("Creating a basic any iterable from std::list, forward iterator") {
    std::list<int> lst = { 1, 2, 3, 4, 5 };
    lz::any_iterable<int, int&, std::forward_iterator_tag> view = lst;
    auto expected = { 1, 2, 3, 4, 5 };

    REQUIRE(lz::equal(view, expected));
}

TEST_CASE("Creating a basic any iterable from std::list, bidirectional iterator") {
    std::list<int> lst = { 1, 2, 3, 4, 5 };
    lz::any_iterable<int, int&, std::bidirectional_iterator_tag> view = lst;
    auto expected = { 1, 2, 3, 4, 5 };

    REQUIRE(lz::equal(view, expected));
    REQUIRE(lz::equal(view | lz::reverse, expected | lz::reverse));
}

const auto eq_fn = [](const std::pair<int, int&> p1, const std::pair<int, int>& p2) {
    return p1.first == p2.first && p1.second == p2.second;
};

TEST_CASE("Creating a complex any iterable, std::forward_iterator_tag") {
    std::vector<int> vec = { 1, 2, 3, 4, 5, 6 };

    lz::any_iterable<std::pair<int, int>, std::pair<int, int&>, std::forward_iterator_tag> view =
#ifdef LZ_HAS_CXX_11
        vec | lz::as<int&>{} | lz::enumerate | lz::take(vec.size());
#else
        vec | lz::as<int&> | lz::enumerate | lz::take(vec.size());
#endif

    auto expected = { std::make_pair(0, 1), std::make_pair(1, 2), std::make_pair(2, 3),
                      std::make_pair(3, 4), std::make_pair(4, 5), std::make_pair(5, 6) };
    REQUIRE(lz::equal(view, expected, eq_fn));
}

TEST_CASE("Creating a complex any iterable, std::bidirectional_iterator_tag") {
    std::vector<int> vec = { 1, 2, 3, 4, 5, 6 };

    lz::any_iterable<std::pair<int, int>, std::pair<int, int&>, std::bidirectional_iterator_tag> view =
#ifdef LZ_HAS_CXX_11
        vec | lz::as<int&>{} | lz::enumerate | lz::take(vec.size());
#else
        vec | lz::as<int&> | lz::enumerate | lz::take(vec.size());
#endif

    auto expected = { std::make_pair(0, 1), std::make_pair(1, 2), std::make_pair(2, 3),
                      std::make_pair(3, 4), std::make_pair(4, 5), std::make_pair(5, 6) };
    REQUIRE(lz::equal(view, expected, eq_fn));
}

TEST_CASE("Creating a complex any iterable, std::random_access_iterator_tag") {
    std::vector<int> vec = { 1, 2, 3, 4, 5, 6 };

    lz::any_iterable<std::pair<int, int>, std::pair<int, int&>, std::random_access_iterator_tag> view =
#ifdef LZ_HAS_CXX_11
        vec | lz::as<int&>{} | lz::enumerate | lz::take(vec.size());
#else
        vec | lz::as<int&> | lz::enumerate | lz::take(vec.size());
#endif

    auto expected = { std::make_pair(0, 1), std::make_pair(1, 2), std::make_pair(2, 3),
                      std::make_pair(3, 4), std::make_pair(4, 5), std::make_pair(5, 6) };
    REQUIRE(lz::equal(view, expected, eq_fn));
    REQUIRE(lz::equal(view | lz::reverse, expected | lz::reverse, eq_fn));
    test_procs::test_operator_plus(view, expected, eq_fn);
    test_procs::test_operator_minus(view);
}

TEST_CASE("Any iterable with different SBO sizes") {
    SECTION("Both fit") {
        std::vector<int> vec = { 1, 2, 3, 4, 5 };
        static_assert(sizeof(vec.begin()) <= 64, "");
        static_assert(sizeof(vec.end()) <= 64, "");
        lz::any_iterable<int, int&, std::random_access_iterator_tag> view = vec;
        auto expected = { 1, 2, 3, 4, 5 };
        REQUIRE(lz::equal(view, expected));
        REQUIRE(lz::equal(view | lz::reverse, expected | lz::reverse));
        test_procs::test_operator_plus(view, expected);
        test_procs::test_operator_minus(view);
    }

    SECTION("Both do not fit") {
        std::vector<int> vec = { 1, 2, 3, 4, 5 };

        std::array<char, 64> buf{};
        auto mapper = lz::map(vec, [buf](int& i) -> int& {
            static_cast<void>(buf);
            return i;
        });
        static_assert(sizeof(mapper.begin()) > 64, "");
        static_assert(sizeof(mapper.end()) > 64, "");

        lz::any_iterable<int, int&, std::random_access_iterator_tag> view = mapper;
        auto expected = { 1, 2, 3, 4, 5 };
        REQUIRE(lz::equal(view, expected));
        REQUIRE(lz::equal(view | lz::reverse, expected | lz::reverse));
        test_procs::test_operator_plus(view, expected);
        test_procs::test_operator_minus(view);
    }

    SECTION("Iter does not fit, sentinel does fit") {
        std::vector<int> vec = { 1, 2, 3, 4, 5 };

        std::array<char, 64> buf{};
        // Decay to forward so that filt returns sentinel
        auto filt = vec | lz::iter_decay(std::forward_iterator_tag{}) | lz::filter([buf](int&) {
                        static_cast<void>(buf);
                        return true;
                    });

        static_assert(sizeof(filt.begin()) > 64, "");
        static_assert(sizeof(filt.end()) == sizeof(lz::default_sentinel_t), "");

        lz::any_iterable<int, int&, std::forward_iterator_tag> view = filt;
        auto expected = { 1, 2, 3, 4, 5 };
        REQUIRE(lz::equal(view, expected));
    }
}
