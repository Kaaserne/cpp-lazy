#include <Lz/c_string.hpp>
#include <Lz/join_where.hpp>
#include <Lz/map.hpp>
#include <cpp-lazy-ut-helper/pch.hpp>
#include <cpp-lazy-ut-helper/ut_helper.hpp>
#include <doctest/doctest.h>

struct customer {
    int id;
};

struct payment_bill {
    int customer_id;
    int id;
};

TEST_CASE("Join where with sentinels") {
    auto c_str = lz::c_string("To join on");
    auto sorted_seq = lz::c_string("Toxzzzz");

    using f1 = std::function<char(char)>;
    using f2 = std::function<char(char)>;
    using f3 = std::function<std::tuple<char, char>(char, char)>;

    lz::join_where_iterable<decltype(c_str), decltype(sorted_seq), f1, f2, f3> joined =
        lz::join_where(c_str, sorted_seq, f1([](char c) { return c; }), f2([](char c) { return c; }),
                       f3([](char c, char c2) { return std::make_tuple(c, c2); }));

    static_assert(!std::is_same<decltype(joined.begin()), decltype(joined.end())>::value, "Should be sentinel");
    std::vector<std::tuple<char, char>> expected = {
        std::make_tuple('T', 'T'),
        std::make_tuple('o', 'o'),
        std::make_tuple('o', 'o'),
        std::make_tuple('o', 'o'),
    };

    auto vec = joined | lz::to<std::vector>();
    REQUIRE(vec == expected);

    SUBCASE("Operator=") {
        std::vector<std::pair<int, int>> vec3 = { { 1, 2 }, { 2, 3 }, { 3, 4 }, { 4, 5 } };
        std::vector<std::pair<int, int>> vec4 = { { 1, 2 }, { 3, 4 }, { 4, 5 }, { 5, 6 } };
        auto joined2 = lz::join_where(
            vec3, vec4, [](const std::pair<int, int>& a) { return a.first; },
            [](const std::pair<int, int>& a) { return a.first; },
            [](const std::pair<int, int>& a, const std::pair<int, int>& b) { return std::make_pair(a.first, b.second); });

        auto common = make_sentinel_assign_op_tester(joined2);
        using reference = lz::ref_iterable_t<decltype(common)>;

        std::vector<std::pair<int, int>> expected2 = { { 1, 2 }, { 3, 4 }, { 4, 5 } };
        REQUIRE(lz::equal(common, expected2,
                          [](reference a, const std::pair<int, int>& b) { return a.first == b.first && a.second == b.second; }));
    }
}

TEST_CASE("Join changing and creating elements") {
    std::vector<customer> customers{
        customer{ 25 }, customer{ 1 }, customer{ 39 }, customer{ 103 }, customer{ 99 },
    };
    std::vector<payment_bill> payment_bills{
        payment_bill{ 25, 0 }, payment_bill{ 25, 2 },    payment_bill{ 25, 3 },
        payment_bill{ 99, 1 }, payment_bill{ 2523, 52 }, payment_bill{ 2523, 53 },
    };

    auto joined = lz::join_where(
        customers, payment_bills, [](const customer& p) { return p.id; }, [](const payment_bill& c) { return c.customer_id; },
        [](const customer& p, const payment_bill& c) { return std::make_tuple(p, c); });

    SUBCASE("Should initialized with first match") {
        std::tuple<customer, payment_bill> match = *joined.begin();
        customer& customer = std::get<0>(match);
        payment_bill& payment_bill = std::get<1>(match);

        REQUIRE(customer.id == payment_bill.customer_id);
        REQUIRE(customer.id == 25);
        REQUIRE(payment_bill.id == 0);
    }
}

TEST_CASE("Empty or one element join where") {
    SUBCASE("Empty join") {
        std::vector<customer> customers;
        std::vector<payment_bill> payment_bills;
        auto joined = lz::join_where(
            customers, payment_bills, [](const customer& p) { return p.id; }, [](const payment_bill& c) { return c.customer_id; },
            [](const customer& p, const payment_bill& c) { return std::make_tuple(p, c); });
        REQUIRE(lz::empty(joined));
        REQUIRE_FALSE(lz::has_many(joined));
        REQUIRE_FALSE(lz::has_one(joined));
    }

    SUBCASE("One element join 1") {
        std::vector<customer> customers{ customer{ 25 } };
        std::vector<payment_bill> payment_bills;
        auto joined = lz::join_where(
            customers, payment_bills, [](const customer& p) { return p.id; }, [](const payment_bill& c) { return c.customer_id; },
            [](const customer& p, const payment_bill& c) { return std::make_tuple(p, c); });
        REQUIRE(lz::empty(joined));
    }

    SUBCASE("One element join 2") {
        std::vector<customer> customers;
        std::vector<payment_bill> payment_bills{ payment_bill{ 25, 0 } };
        auto joined = lz::join_where(
            customers, payment_bills, [](const customer& p) { return p.id; }, [](const payment_bill& c) { return c.customer_id; },
            [](const customer& p, const payment_bill& c) { return std::make_tuple(p, c); });
        REQUIRE(lz::empty(joined));
    }

    SUBCASE("One element join 3") {
        std::vector<customer> customers{ customer{ 25 } };
        std::vector<payment_bill> payment_bills{ payment_bill{ 25, 0 } };
        auto joined = lz::join_where(
            customers, payment_bills, [](const customer& p) { return p.id; }, [](const payment_bill& c) { return c.customer_id; },
            [](const customer& p, const payment_bill& c) { return std::make_tuple(p, c); });
        REQUIRE(lz::has_one(joined));
        REQUIRE_FALSE(lz::has_many(joined));
        REQUIRE_FALSE(lz::empty(joined));
    }
}

TEST_CASE("Left join binary operations") {
    std::vector<customer> customers{
        customer{ 25 }, customer{ 1 }, customer{ 39 }, customer{ 103 }, customer{ 99 },
    };
    std::vector<payment_bill> payment_bills{
        payment_bill{ 25, 0 }, payment_bill{ 25, 2 },    payment_bill{ 25, 3 },
        payment_bill{ 99, 1 }, payment_bill{ 2523, 52 }, payment_bill{ 2523, 53 },
    };

    auto joined = customers |
                  lz::join_where(
                      payment_bills, [](const customer& p) { return p.id; }, [](const payment_bill& c) { return c.customer_id; },
                      [](const customer& p, const payment_bill& c) { return std::make_tuple(p, c); });

    auto expected = std::vector<std::tuple<customer, payment_bill>>{ std::make_tuple(customer{ 25 }, payment_bill{ 25, 0 }),
                                                                     std::make_tuple(customer{ 25 }, payment_bill{ 25, 2 }),
                                                                     std::make_tuple(customer{ 25 }, payment_bill{ 25, 3 }),
                                                                     std::make_tuple(customer{ 99 }, payment_bill{ 99, 1 }) };
    SUBCASE("Operator++") {
        REQUIRE(lz::equal(joined, expected,
                          [](const std::tuple<customer, payment_bill>& a, const std::tuple<customer, payment_bill>& b) {
                              auto& a_fst = std::get<0>(a);
                              auto& a_snd = std::get<1>(a);
                              auto& b_fst = std::get<0>(b);
                              auto& b_snd = std::get<1>(b);
                              return a_fst.id == b_fst.id && a_snd.id == b_snd.id && a_snd.customer_id == b_snd.customer_id;
                          }));
    }
}

TEST_CASE("join_where_iterable to containers") {
    std::vector<customer> customers{
        customer{ 25 }, customer{ 1 }, customer{ 39 }, customer{ 103 }, customer{ 99 },
    };
    std::vector<payment_bill> payment_bills{
        payment_bill{ 25, 0 }, payment_bill{ 25, 2 },    payment_bill{ 25, 3 },
        payment_bill{ 99, 1 }, payment_bill{ 2523, 52 }, payment_bill{ 2523, 53 },
    };

    auto joined = lz::join_where(
        customers, payment_bills, [](const customer& p) { return p.id; }, [](const payment_bill& c) { return c.customer_id; },
        [](const customer& p, const payment_bill& c) { return std::make_tuple(p, c); });

    SUBCASE("To array") {
        std::array<std::tuple<customer, payment_bill>, 4> expected = { std::make_tuple(customer{ 25 }, payment_bill{ 25, 0 }),
                                                                       std::make_tuple(customer{ 25 }, payment_bill{ 25, 2 }),
                                                                       std::make_tuple(customer{ 25 }, payment_bill{ 25, 3 }),
                                                                       std::make_tuple(customer{ 99 }, payment_bill{ 99, 1 }) };

        auto array = joined | lz::to<std::array<std::tuple<customer, payment_bill>, 4>>();
        REQUIRE(lz::equal(array, expected,
                          [](const std::tuple<customer, payment_bill>& a, const std::tuple<customer, payment_bill>& b) {
                              auto& a_fst = std::get<0>(a);
                              auto& a_snd = std::get<1>(a);
                              auto& b_fst = std::get<0>(b);
                              auto& b_snd = std::get<1>(b);
                              return a_fst.id == b_fst.id && a_snd.id == b_snd.id && a_snd.customer_id == b_snd.customer_id;
                          }));
    }

    SUBCASE("To vector") {
        std::vector<std::tuple<customer, payment_bill>> expected = { std::make_tuple(customer{ 25 }, payment_bill{ 25, 0 }),
                                                                     std::make_tuple(customer{ 25 }, payment_bill{ 25, 2 }),
                                                                     std::make_tuple(customer{ 25 }, payment_bill{ 25, 3 }),
                                                                     std::make_tuple(customer{ 99 }, payment_bill{ 99, 1 }) };

        auto vec = joined | lz::to<std::vector>();
        REQUIRE(lz::equal(vec, expected,
                          [](const std::tuple<customer, payment_bill>& a, const std::tuple<customer, payment_bill>& b) {
                              auto& a_fst = std::get<0>(a);
                              auto& a_snd = std::get<1>(a);
                              auto& b_fst = std::get<0>(b);
                              auto& b_snd = std::get<1>(b);
                              return a_fst.id == b_fst.id && a_snd.id == b_snd.id && a_snd.customer_id == b_snd.customer_id;
                          }));
    }

    SUBCASE("To other container using to<>()") {
        std::list<std::tuple<customer, payment_bill>> expected = { std::make_tuple(customer{ 25 }, payment_bill{ 25, 0 }),
                                                                   std::make_tuple(customer{ 25 }, payment_bill{ 25, 2 }),
                                                                   std::make_tuple(customer{ 25 }, payment_bill{ 25, 3 }),
                                                                   std::make_tuple(customer{ 99 }, payment_bill{ 99, 1 }) };

        auto list = joined | lz::to<std::list>();
        REQUIRE(lz::equal(list, expected,
                          [](const std::tuple<customer, payment_bill>& a, const std::tuple<customer, payment_bill>& b) {
                              auto& a_fst = std::get<0>(a);
                              auto& a_snd = std::get<1>(a);
                              auto& b_fst = std::get<0>(b);
                              auto& b_snd = std::get<1>(b);
                              return a_fst.id == b_fst.id && a_snd.id == b_snd.id && a_snd.customer_id == b_snd.customer_id;
                          }));
    }

    SUBCASE("To map") {
        using pair = std::pair<int, std::tuple<customer, payment_bill>>;

        std::map<int, std::tuple<customer, payment_bill>> expected = {
            { 0, std::make_tuple(customer{ 25 }, payment_bill{ 25, 0 }) },
            { 2, std::make_tuple(customer{ 25 }, payment_bill{ 25, 2 }) },
            { 3, std::make_tuple(customer{ 25 }, payment_bill{ 25, 3 }) },
            { 1, std::make_tuple(customer{ 99 }, payment_bill{ 99, 1 }) }
        };

        auto actual =
            joined |
            lz::map([](const std::tuple<customer, payment_bill>& val) { return std::make_pair(std::get<1>(val).id, val); }) |
            lz::to<std::map<int, std::tuple<customer, payment_bill>>>();

        REQUIRE(lz::equal(expected, actual, [](const pair& a, const pair& b) {
            return a.first == b.first && std::get<1>(a.second).id == std::get<1>(b.second).id &&
                   std::get<1>(a.second).customer_id == std::get<1>(b.second).customer_id;
        }));
    }

    SUBCASE("To unordered map") {
        using pair = std::pair<int, std::tuple<customer, payment_bill>>;

        std::unordered_map<int, std::tuple<customer, payment_bill>> expected = {
            { 0, std::make_tuple(customer{ 25 }, payment_bill{ 25, 0 }) },
            { 2, std::make_tuple(customer{ 25 }, payment_bill{ 25, 2 }) },
            { 3, std::make_tuple(customer{ 25 }, payment_bill{ 25, 3 }) },
            { 1, std::make_tuple(customer{ 99 }, payment_bill{ 99, 1 }) }
        };

        auto actual =
            joined |
            lz::map([](const std::tuple<customer, payment_bill>& val) { return std::make_pair(std::get<1>(val).id, val); }) |
            lz::to<std::unordered_map<int, std::tuple<customer, payment_bill>>>();

        REQUIRE(lz::equal(expected, actual, [](const pair& a, const pair& b) {
            return a.first == b.first && std::get<1>(a.second).id == std::get<1>(b.second).id &&
                   std::get<1>(a.second).customer_id == std::get<1>(b.second).customer_id;
        }));
    }
}
