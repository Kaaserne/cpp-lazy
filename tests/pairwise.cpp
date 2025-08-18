#include <Lz/algorithm.hpp>
#include <Lz/filter.hpp>
#include <Lz/pairwise.hpp>
#include <Lz/repeat.hpp>
#include <Lz/reverse.hpp>
#include <cpp-lazy-ut-helper/c_string.hpp>
#include <cpp-lazy-ut-helper/test_procs.hpp>
#include <doctest/doctest.h>
#include <pch.hpp>

TEST_CASE("Empty or one element") {
    SUBCASE("Empty vector") {
        std::vector<int> vec;
        auto it = lz::pairwise(vec, 2);
        REQUIRE(lz::size(it) == 0);
        REQUIRE(lz::empty(it));
        REQUIRE_FALSE(lz::has_many(it));
        REQUIRE_FALSE(lz::has_one(it));
        REQUIRE(lz::distance(it) == 0); // TODO add lz::distance everywhere for empty
    }

    SUBCASE("Single element vector 1") {
        std::vector<int> vec{ 1 };
        auto it = lz::pairwise(vec, 1);
        static_assert(!lz::detail::has_sentinel<decltype(vec)>::value, "");
        REQUIRE(lz::size(it) == 1);
        REQUIRE(lz::has_one(it));
        REQUIRE_FALSE(lz::has_many(it));
        REQUIRE_FALSE(lz::empty(it));
        auto expected = { 1 };
        REQUIRE(lz::equal(*it.begin(), expected));
        REQUIRE(lz::distance(it) == 1);
    }

    SUBCASE("Single element vector 2") {
        std::vector<int> vec{ 1 };
        auto it = lz::pairwise(vec, 2);
        REQUIRE(lz::size(it) == 0);
        REQUIRE(lz::empty(it));
        REQUIRE_FALSE(lz::has_many(it));
        REQUIRE_FALSE(lz::has_one(it));
        REQUIRE(lz::distance(it) == 0);
    }

    SUBCASE("Two elements vector 2") {
        std::vector<int> vec{ 1, 2 };
        auto it = lz::pairwise(vec, 2);
        REQUIRE(lz::size(it) == 1);
        REQUIRE_FALSE(lz::empty(it));
        REQUIRE_FALSE(lz::has_many(it));
        REQUIRE(lz::has_one(it));
        auto expected = { 1, 2 };
        REQUIRE(lz::equal(*it.begin(), expected));
        REQUIRE(lz::distance(it) == 1);
    }

    SUBCASE("Forward non sized sentinelled") {
        auto cstr = lz::c_string("");
        auto it = lz::pairwise(cstr, 2);
        static_assert(lz::detail::has_sentinel<decltype(cstr)>::value, "");
        REQUIRE(lz::empty(it));
        REQUIRE_FALSE(lz::has_many(it));
        REQUIRE_FALSE(lz::has_one(it));
    }

    SUBCASE("Forward non sized sentinelled 1") {
        auto cstr = lz::c_string("a");
        auto it = lz::pairwise(cstr, 1);
        REQUIRE_FALSE(lz::empty(it));
        REQUIRE_FALSE(lz::has_many(it));
        REQUIRE(lz::has_one(it));
        auto expected = { 'a' };
        REQUIRE(lz::equal(*it.begin(), expected));
    }

    SUBCASE("Forward non sized sentinelled 2") {
        auto cstr = lz::c_string("a");
        auto it = lz::pairwise(cstr, 2);
        REQUIRE(lz::empty(it));
        REQUIRE_FALSE(lz::has_many(it));
        REQUIRE_FALSE(lz::has_one(it));
    }

    SUBCASE("Two elements forward non sized sentinelled") {
        auto cstr = lz::c_string("ab");
        auto it = lz::pairwise(cstr, 2);
        REQUIRE_FALSE(lz::empty(it));
        REQUIRE_FALSE(lz::has_many(it));
        REQUIRE(lz::has_one(it));
        auto expected = { 'a', 'b' };
        REQUIRE(lz::equal(*it.begin(), expected));
    }

    SUBCASE("Empty sized bidi iterable") {
        std::list<int> lst;
        auto it = lz::pairwise(lst, 2);
        REQUIRE(lz::size(it) == 0);
        REQUIRE(lz::empty(it));
        REQUIRE_FALSE(lz::has_many(it));
        REQUIRE_FALSE(lz::has_one(it));
        REQUIRE(lz::distance(it) == 0);
    }

    SUBCASE("Single element sized bidi iterable 1") {
        std::list<int> lst{ 1 };
        auto it = lz::pairwise(lst, 1);
        REQUIRE(lz::size(it) == 1);
        REQUIRE(lz::has_one(it));
        REQUIRE_FALSE(lz::has_many(it));
        REQUIRE_FALSE(lz::empty(it));
        auto expected = { 1 };
        REQUIRE(lz::equal(*it.begin(), expected));
        REQUIRE(lz::distance(it) == 1);
    }

    SUBCASE("Single element sized bidi iterable 2") {
        std::list<int> lst{ 1 };
        auto it = lz::pairwise(lst, 2);
        REQUIRE(lz::size(it) == 0);
        REQUIRE(lz::empty(it));
        REQUIRE_FALSE(lz::has_many(it));
        REQUIRE_FALSE(lz::has_one(it));
    }

    SUBCASE("Two elements sized bidi iterable 2") {
        std::list<int> lst{ 1, 2 };
        auto it = lz::pairwise(lst, 2);
        REQUIRE(lz::size(it) == 1);
        REQUIRE_FALSE(lz::empty(it));
        REQUIRE_FALSE(lz::has_many(it));
        REQUIRE(lz::has_one(it));
        auto expected = { 1, 2 };
        REQUIRE(lz::equal(*it.begin(), expected));
    }

    SUBCASE("Empty non sized bidi iterable") {
        std::vector<int> vec;
        auto it = lz::pairwise(vec | lz::filter([](int) { return true; }), 2);
        static_assert(!lz::detail::has_sentinel<decltype(vec)>::value, "");
        REQUIRE(lz::empty(it));
        REQUIRE_FALSE(lz::has_many(it));
        REQUIRE_FALSE(lz::has_one(it));
        REQUIRE(lz::distance(it) == 0);
    }

    SUBCASE("Single element non sized bidi iterable 1") {
        std::vector<int> vec{ 1 };
        auto it = lz::pairwise(vec | lz::filter([](int) { return true; }), 1);
        REQUIRE(lz::has_one(it));
        REQUIRE_FALSE(lz::has_many(it));
        REQUIRE_FALSE(lz::empty(it));
        auto expected = { 1 };
        REQUIRE(lz::equal(*it.begin(), expected));
        REQUIRE(lz::distance(it) == 1);
    }

    SUBCASE("Single element non sized bidi iterable 2") {
        std::vector<int> vec{ 1 };
        auto it = lz::pairwise(vec | lz::filter([](int) { return true; }), 2);
        REQUIRE(lz::empty(it));
        REQUIRE_FALSE(lz::has_many(it));
        REQUIRE_FALSE(lz::has_one(it));
        REQUIRE(lz::distance(it) == 0);
    }

    SUBCASE("Two elements non sized bidi iterable 2") {
        std::vector<int> vec{ 1, 2 };
        auto it = lz::pairwise(vec | lz::filter([](int) { return true; }), 2);
        REQUIRE_FALSE(lz::empty(it));
        REQUIRE_FALSE(lz::has_many(it));
        REQUIRE(lz::has_one(it));
        auto expected = { 1, 2 };
        REQUIRE(lz::equal(*it.begin(), expected));
        REQUIRE(lz::distance(it) == 1);
    }
}

TEST_CASE("pairwise non sentinelled iterator random access") {
    SUBCASE("Operator=(default_sentinel_t)") {
        std::vector<int> v;
        auto p = lz::pairwise(v, 1);
        auto it = p.begin();
        it = lz::default_sentinel;
        REQUIRE(it == p.end());
        REQUIRE(p.end() == it);

        v = { 1 };
        p = lz::pairwise(v, 1);
        it = p.begin();
        it = lz::default_sentinel;
        REQUIRE(it == p.end());
        REQUIRE(p.end() == it);

        p = lz::pairwise(v, 2);
        it = p.begin();
        it = lz::default_sentinel;
        REQUIRE(it == p.end());
        REQUIRE(p.end() == it);
    }

    std::vector<int> vec{ 1, 2, 3, 4, 5 };

    SUBCASE("Forward") {
        std::vector<std::vector<int>> expected = { { 1 }, { 2 }, { 3 }, { 4 }, { 5 } };
        auto it = lz::pairwise(vec, 1);
        using value_type = typename decltype(it.begin())::value_type;

        REQUIRE(lz::size(it) == vec.size());
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 1, 2 }, { 2, 3 }, { 3, 4 }, { 4, 5 } };
        it = lz::pairwise(vec, 2);
        REQUIRE(lz::size(it) == vec.size() - 1);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 1, 2, 3 }, { 2, 3, 4 }, { 3, 4, 5 } };
        it = vec | lz::pairwise(3);
        REQUIRE(lz::size(it) == vec.size() - 2);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 1, 2, 3, 4 }, { 2, 3, 4, 5 } };
        it = vec | lz::pairwise(4);
        REQUIRE(lz::size(it) == vec.size() - 3);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 1, 2, 3, 4, 5 } };
        it = vec | lz::pairwise(5);
        REQUIRE(lz::size(it) == vec.size() - 4);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));
    }

    SUBCASE("Backward") {
        std::vector<std::vector<int>> expected = { { 5 }, { 4 }, { 3 }, { 2 }, { 1 } };
        auto it = lz::pairwise(vec, 1) | lz::reverse;
        using value_type = typename decltype(it.begin())::value_type;

        REQUIRE(lz::size(it) == vec.size());
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 4, 5 }, { 3, 4 }, { 2, 3 }, { 1, 2 } };
        it = lz::pairwise(vec, 2) | lz::reverse;
        REQUIRE(lz::size(it) == vec.size() - 1);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 3, 4, 5 }, { 2, 3, 4 }, { 1, 2, 3 } };
        it = vec | lz::pairwise(3) | lz::reverse;
        REQUIRE(lz::size(it) == vec.size() - 2);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 2, 3, 4, 5 }, { 1, 2, 3, 4 } };
        it = vec | lz::pairwise(4) | lz::reverse;
        REQUIRE(lz::size(it) == vec.size() - 3);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 1, 2, 3, 4, 5 } };
        it = vec | lz::pairwise(5) | lz::reverse;
        REQUIRE(lz::size(it) == vec.size() - 4);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));
    }

    SUBCASE("Operator+/-") {
        auto it = lz::pairwise(vec, 1);
        using value_type = typename decltype(it.begin())::value_type;
        const auto eq_compare = [](value_type a, const std::vector<int>& b) {
            return lz::equal(a, b);
        };

        std::vector<std::vector<int>> expected = { { 1 }, { 2 }, { 3 }, { 4 }, { 5 } };
        test_procs::test_operator_plus(it, expected, eq_compare);
        test_procs::test_operator_minus(it);

        expected = { { 1, 2 }, { 2, 3 }, { 3, 4 }, { 4, 5 } };
        it = lz::pairwise(vec, 2);
        test_procs::test_operator_plus(it, expected, eq_compare);
        test_procs::test_operator_minus(it);

        expected = { { 1, 2, 3 }, { 2, 3, 4 }, { 3, 4, 5 } };
        it = vec | lz::pairwise(3);
        test_procs::test_operator_plus(it, expected, eq_compare);
        test_procs::test_operator_minus(it);

        expected = { { 1, 2, 3, 4 }, { 2, 3, 4, 5 } };
        it = vec | lz::pairwise(4);
        test_procs::test_operator_plus(it, expected, eq_compare);
        test_procs::test_operator_minus(it);

        expected = { { 1, 2, 3, 4, 5 } };
        it = vec | lz::pairwise(5);
        test_procs::test_operator_plus(it, expected, eq_compare);
        test_procs::test_operator_minus(it);
    }
}

TEST_CASE("pairwise sentinelled iterator random access") {
    SUBCASE("Operator=(default_sentinel_t)") {
        auto rep = lz::repeat(1, 0);
        auto p = lz::pairwise(rep, 1);
        auto it = p.begin();
        it = lz::default_sentinel;
        REQUIRE(it == p.end());
        REQUIRE(p.end() == it);

        rep = lz::repeat(1, 1);
        p = lz::pairwise(rep, 1);
        it = p.begin();
        it = lz::default_sentinel;
        REQUIRE(it == p.end());
        REQUIRE(p.end() == it);

        p = lz::pairwise(rep, 2);
        it = p.begin();
        it = lz::default_sentinel;
        REQUIRE(it == p.end());
        REQUIRE(p.end() == it);
    }

    auto repeater = lz::repeat(1, 5);

    SUBCASE("Forward") {
        std::vector<std::vector<int>> expected = { { 1 }, { 1 }, { 1 }, { 1 }, { 1 } };
        auto it = lz::pairwise(repeater, 1);
        static_assert(lz::detail::has_sentinel<decltype(repeater)>::value, "");
        using value_type = typename decltype(it.begin())::value_type;

        REQUIRE(lz::size(it) == repeater.size());
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 } };
        it = lz::pairwise(repeater, 2);
        REQUIRE(lz::size(it) == repeater.size() - 1);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } };
        it = repeater | lz::pairwise(3);
        REQUIRE(lz::size(it) == repeater.size() - 2);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 1, 1, 1, 1 }, { 1, 1, 1, 1 } };
        it = repeater | lz::pairwise(4);
        REQUIRE(lz::size(it) == repeater.size() - 3);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 1, 1, 1, 1, 1 } };
        it = repeater | lz::pairwise(5);
        REQUIRE(lz::size(it) == repeater.size() - 4);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));
    }

    SUBCASE("Operator+/-") {
        auto it = lz::pairwise(repeater, 1);
        using value_type = typename decltype(it.begin())::value_type;
        const auto eq_compare = [](value_type a, const std::vector<int>& b) {
            return lz::equal(a, b);
        };

        std::vector<std::vector<int>> expected = { { 1 }, { 1 }, { 1 }, { 1 }, { 1 } };
        test_procs::test_operator_plus(it, expected, eq_compare);
        test_procs::test_operator_minus(it);

        expected = { { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 } };
        it = lz::pairwise(repeater, 2);
        test_procs::test_operator_plus(it, expected, eq_compare);
        test_procs::test_operator_minus(it);

        expected = { { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } };
        it = repeater | lz::pairwise(3);
        test_procs::test_operator_plus(it, expected, eq_compare);
        test_procs::test_operator_minus(it);

        expected = { { 1, 1, 1, 1 }, { 1, 1, 1, 1 } };
        it = repeater | lz::pairwise(4);
        test_procs::test_operator_plus(it, expected, eq_compare);
        test_procs::test_operator_minus(it);

        expected = { { 1, 1, 1, 1, 1 } };
        it = repeater | lz::pairwise(5);
        test_procs::test_operator_plus(it, expected, eq_compare);
        test_procs::test_operator_minus(it);
    }
}

TEST_CASE("Pairwise non sentinelled bidi sized") {
    std::list<int> lst{ 1, 2, 3, 4, 5 };

    SUBCASE("Operator=(default_sentinel_t)") {
        auto p = lz::pairwise(lst, 1);
        auto it = p.begin();
        REQUIRE(it == p.begin());
        it = lz::default_sentinel;
        REQUIRE(it == p.end());
        REQUIRE(p.end() == it);
        it = p.end();
        REQUIRE(it == p.end());

        lst = { 1 };
        p = lz::pairwise(lst, 1);
        it = p.begin();
        REQUIRE(it == p.begin());
        it = lz::default_sentinel;
        REQUIRE(it == p.end());
        REQUIRE(p.end() == it);
        it = p.end();
        REQUIRE(it == p.end());

        p = lz::pairwise(lst, 2);
        it = p.begin();
        REQUIRE(it == p.begin());
        it = lz::default_sentinel;
        REQUIRE(it == p.end());
        REQUIRE(p.end() == it);
        it = p.end();
        REQUIRE(it == p.end());
    }

    SUBCASE("Forward") {
        std::vector<std::vector<int>> expected = { { 1 }, { 2 }, { 3 }, { 4 }, { 5 } };
        auto it = lz::pairwise(lst, 1);
        using value_type = typename decltype(it.begin())::value_type;

        REQUIRE(lz::size(it) == lst.size());
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 1, 2 }, { 2, 3 }, { 3, 4 }, { 4, 5 } };
        it = lz::pairwise(lst, 2);
        REQUIRE(lz::size(it) == lst.size() - 1);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 1, 2, 3 }, { 2, 3, 4 }, { 3, 4, 5 } };
        it = lst | lz::pairwise(3);
        REQUIRE(lz::size(it) == lst.size() - 2);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 1, 2, 3, 4 }, { 2, 3, 4, 5 } };
        it = lst | lz::pairwise(4);
        REQUIRE(lz::size(it) == lst.size() - 3);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 1, 2, 3, 4, 5 } };
        it = lst | lz::pairwise(5);
        REQUIRE(lz::size(it) == lst.size() - 4);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));
    }

    SUBCASE("Backward") {
        std::vector<std::vector<int>> expected = { { 5 }, { 4 }, { 3 }, { 2 }, { 1 } };
        auto it = lz::pairwise(lst, 1) | lz::reverse;
        using value_type = typename decltype(it.begin())::value_type;

        REQUIRE(lz::size(it) == lst.size());
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 4, 5 }, { 3, 4 }, { 2, 3 }, { 1, 2 } };
        it = lz::pairwise(lst, 2) | lz::reverse;
        REQUIRE(lz::size(it) == lst.size() - 1);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 3, 4, 5 }, { 2, 3, 4 }, { 1, 2, 3 } };
        it = lst | lz::pairwise(3) | lz::reverse;
        REQUIRE(lz::size(it) == lst.size() - 2);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 2, 3, 4, 5 }, { 1, 2, 3, 4 } };
        it = lst | lz::pairwise(4) | lz::reverse;
        REQUIRE(lz::size(it) == lst.size() - 3);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 1, 2, 3, 4, 5 } };
        it = lst | lz::pairwise(5) | lz::reverse;
        REQUIRE(lz::size(it) == lst.size() - 4);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));
    }
}

TEST_CASE("Non sized fwd sentinelled") {
    auto cstr = lz::c_string("abcde");

    SUBCASE("Operator=(default_sentinel_t)") {
        auto p = lz::pairwise(cstr, 1);
        auto it = p.begin();
        REQUIRE(it == p.begin());
        it = lz::default_sentinel;
        REQUIRE(it == p.end());
        REQUIRE(p.end() == it);

        cstr = lz::c_string("a");
        p = lz::pairwise(cstr, 1);
        it = p.begin();
        REQUIRE(it == p.begin());
        it = lz::default_sentinel;
        REQUIRE(it == p.end());
        REQUIRE(p.end() == it);

        p = lz::pairwise(cstr, 2);
        it = p.begin();
        REQUIRE(it == p.begin());
        it = lz::default_sentinel;
        REQUIRE(it == p.end());
        REQUIRE(p.end() == it);
    }
    
    SUBCASE("Forward") {
        std::vector<std::vector<char>> expected = { { 'a' }, { 'b' }, { 'c' }, { 'd' }, { 'e' } };
        const std::size_t cstr_size = lz::eager_size(cstr);
        auto it = lz::pairwise(cstr, 1);
        using value_type = typename decltype(it.begin())::value_type;

        REQUIRE(lz::eager_size(it) == cstr_size);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<char>& b) { return lz::equal(a, b); }));

        expected = { { 'a', 'b' }, { 'b', 'c' }, { 'c', 'd' }, { 'd', 'e' } };
        it = lz::pairwise(cstr, 2);
        REQUIRE(lz::eager_size(it) == cstr_size - 1);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<char>& b) { return lz::equal(a, b); }));

        expected = { { 'a', 'b', 'c' }, { 'b', 'c', 'd' }, { 'c', 'd', 'e' } };
        it = cstr | lz::pairwise(3);
        REQUIRE(lz::eager_size(it) == cstr_size - 2);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<char>& b) { return lz::equal(a, b); }));

        expected = { { 'a', 'b', 'c', 'd' }, { 'b', 'c', 'd', 'e' } };
        it = cstr | lz::pairwise(4);
        REQUIRE(lz::eager_size(it) == cstr_size - 3);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<char>& b) { return lz::equal(a, b); }));

        expected = { { 'a', 'b', 'c', 'd', 'e' } };
        it = cstr | lz::pairwise(5);
        REQUIRE(lz::eager_size(it) == cstr_size - 4);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<char>& b) { return lz::equal(a, b); }));
    }
}