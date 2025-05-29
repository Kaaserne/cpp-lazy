#include <Lz/c_string.hpp>
#include <Lz/join_where.hpp>
#include <Lz/map.hpp>
#include <catch2/catch.hpp>
#include <list>
#include <map>
#include <unordered_map>

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

    SECTION("Operator=") {
        auto begin = joined.begin();
        REQUIRE(begin == joined.begin());
        begin = joined.end();
        REQUIRE(begin == joined.end());
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

    SECTION("Should initialized with first match") {
        std::tuple<customer, payment_bill> match = *joined.begin();
        customer& customer = std::get<0>(match);
        payment_bill& payment_bill = std::get<1>(match);

        REQUIRE(customer.id == payment_bill.customer_id);
        REQUIRE(customer.id == 25);
        REQUIRE(payment_bill.id == 0);
    }
}

TEST_CASE("Empty or one element join where") {
    SECTION("Empty join") {
        std::vector<customer> customers;
        std::vector<payment_bill> payment_bills;
        auto joined = lz::join_where(
            customers, payment_bills, [](const customer& p) { return p.id; }, [](const payment_bill& c) { return c.customer_id; },
            [](const customer& p, const payment_bill& c) { return std::make_tuple(p, c); });
        REQUIRE(lz::empty(joined));
        REQUIRE(!lz::has_many(joined));
        REQUIRE(!lz::has_one(joined));
    }

    SECTION("One element join 1") {
        std::vector<customer> customers{ customer{ 25 } };
        std::vector<payment_bill> payment_bills;
        auto joined = lz::join_where(
            customers, payment_bills, [](const customer& p) { return p.id; }, [](const payment_bill& c) { return c.customer_id; },
            [](const customer& p, const payment_bill& c) { return std::make_tuple(p, c); });
        REQUIRE(lz::empty(joined));
    }

    SECTION("One element join 2") {
        std::vector<customer> customers;
        std::vector<payment_bill> payment_bills{ payment_bill{ 25, 0 } };
        auto joined = lz::join_where(
            customers, payment_bills, [](const customer& p) { return p.id; }, [](const payment_bill& c) { return c.customer_id; },
            [](const customer& p, const payment_bill& c) { return std::make_tuple(p, c); });
        REQUIRE(lz::empty(joined));
    }

    SECTION("One element join 3") {
        std::vector<customer> customers{ customer{ 25 } };
        std::vector<payment_bill> payment_bills{ payment_bill{ 25, 0 } };
        auto joined = lz::join_where(
            customers, payment_bills, [](const customer& p) { return p.id; }, [](const payment_bill& c) { return c.customer_id; },
            [](const customer& p, const payment_bill& c) { return std::make_tuple(p, c); });
        REQUIRE(lz::has_one(joined));
        REQUIRE(!lz::has_many(joined));
        REQUIRE(!lz::empty(joined));
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
    auto it = joined.begin();

    SECTION("Operator++") {
        auto expected = std::vector<std::tuple<customer, payment_bill>>{ std::make_tuple(customer{ 25 }, payment_bill{ 25, 0 }),
                                                                         std::make_tuple(customer{ 25 }, payment_bill{ 25, 2 }),
                                                                         std::make_tuple(customer{ 25 }, payment_bill{ 25, 3 }),
                                                                         std::make_tuple(customer{ 99 }, payment_bill{ 99, 1 }) };

        REQUIRE(lz::equal(joined, expected,
                          [](const std::tuple<customer, payment_bill>& a, const std::tuple<customer, payment_bill>& b) {
                              auto& a_fst = std::get<0>(a);
                              auto& a_snd = std::get<1>(a);
                              auto& b_fst = std::get<0>(b);
                              auto& b_snd = std::get<1>(b);
                              return a_fst.id == b_fst.id && a_snd.id == b_snd.id && a_snd.customer_id == b_snd.customer_id;
                          }));
    }

    SECTION("Operator== & operator!=") {
        REQUIRE(it != joined.end());
        while (it != joined.end()) {
            ++it;
        }
        REQUIRE(it == joined.end());
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

    SECTION("To array") {
        std::array<std::tuple<customer, payment_bill>, 4> expected = { std::make_tuple(customer{ 25 }, payment_bill{ 25, 0 }),
                                                                       std::make_tuple(customer{ 25 }, payment_bill{ 25, 2 }),
                                                                       std::make_tuple(customer{ 25 }, payment_bill{ 25, 3 }),
                                                                       std::make_tuple(customer{ 99 }, payment_bill{ 99, 1 }) };

        auto array = joined | lz::to<std::array<std::tuple<customer, payment_bill>, 4>>();
        REQUIRE(std::equal(array.begin(), array.end(), expected.begin(),
                           [](const std::tuple<customer, payment_bill>& a, const std::tuple<customer, payment_bill>& b) {
                               auto& a_fst = std::get<0>(a);
                               auto& a_snd = std::get<1>(a);
                               auto& b_fst = std::get<0>(b);
                               auto& b_snd = std::get<1>(b);
                               return a_fst.id == b_fst.id && a_snd.id == b_snd.id && a_snd.customer_id == b_snd.customer_id;
                           }));
    }

    SECTION("To vector") {
        std::vector<std::tuple<customer, payment_bill>> expected = { std::make_tuple(customer{ 25 }, payment_bill{ 25, 0 }),
                                                                     std::make_tuple(customer{ 25 }, payment_bill{ 25, 2 }),
                                                                     std::make_tuple(customer{ 25 }, payment_bill{ 25, 3 }),
                                                                     std::make_tuple(customer{ 99 }, payment_bill{ 99, 1 }) };

        auto vec = joined | lz::to<std::vector>();
        REQUIRE(std::equal(vec.begin(), vec.end(), expected.begin(),
                           [](const std::tuple<customer, payment_bill>& a, const std::tuple<customer, payment_bill>& b) {
                               auto& a_fst = std::get<0>(a);
                               auto& a_snd = std::get<1>(a);
                               auto& b_fst = std::get<0>(b);
                               auto& b_snd = std::get<1>(b);
                               return a_fst.id == b_fst.id && a_snd.id == b_snd.id && a_snd.customer_id == b_snd.customer_id;
                           }));
    }

    SECTION("To other container using to<>()") {
        std::list<std::tuple<customer, payment_bill>> expected = { std::make_tuple(customer{ 25 }, payment_bill{ 25, 0 }),
                                                                   std::make_tuple(customer{ 25 }, payment_bill{ 25, 2 }),
                                                                   std::make_tuple(customer{ 25 }, payment_bill{ 25, 3 }),
                                                                   std::make_tuple(customer{ 99 }, payment_bill{ 99, 1 }) };

        auto list = joined | lz::to<std::list>();
        REQUIRE(std::equal(list.begin(), list.end(), expected.begin(),
                           [](const std::tuple<customer, payment_bill>& a, const std::tuple<customer, payment_bill>& b) {
                               auto& a_fst = std::get<0>(a);
                               auto& a_snd = std::get<1>(a);
                               auto& b_fst = std::get<0>(b);
                               auto& b_snd = std::get<1>(b);
                               return a_fst.id == b_fst.id && a_snd.id == b_snd.id && a_snd.customer_id == b_snd.customer_id;
                           }));
    }

    SECTION("To map") {
        using pair = std::pair<int, std::tuple<customer, payment_bill>>;

        std::map<int, std::tuple<customer, payment_bill>> expected = {
            { 0, std::make_tuple(customer{ 25 }, payment_bill{ 25, 0 }) },
            { 2, std::make_tuple(customer{ 25 }, payment_bill{ 25, 2 }) },
            { 3, std::make_tuple(customer{ 25 }, payment_bill{ 25, 3 }) },
            { 1, std::make_tuple(customer{ 99 }, payment_bill{ 99, 1 }) }
        };

        auto actual = joined | lz::map([](const std::tuple<customer, payment_bill>& val) {
                          return std::make_pair(std::get<1>(val).id, val);
                      }) |
                      lz::to<std::map<int, std::tuple<customer, payment_bill>>>();

        REQUIRE(std::equal(expected.begin(), expected.end(), actual.begin(), [](const pair& a, const pair& b) {
            return a.first == b.first && std::get<1>(a.second).id == std::get<1>(b.second).id &&
                   std::get<1>(a.second).customer_id == std::get<1>(b.second).customer_id;
        }));
    }

    SECTION("To unordered map") {
        using pair = std::pair<int, std::tuple<customer, payment_bill>>;

        std::unordered_map<int, std::tuple<customer, payment_bill>> expected = {
            { 0, std::make_tuple(customer{ 25 }, payment_bill{ 25, 0 }) },
            { 2, std::make_tuple(customer{ 25 }, payment_bill{ 25, 2 }) },
            { 3, std::make_tuple(customer{ 25 }, payment_bill{ 25, 3 }) },
            { 1, std::make_tuple(customer{ 99 }, payment_bill{ 99, 1 }) }
        };

        auto actual = joined | lz::map([](const std::tuple<customer, payment_bill>& val) {
                          return std::make_pair(std::get<1>(val).id, val);
                      }) |
                      lz::to<std::unordered_map<int, std::tuple<customer, payment_bill>>>();

        REQUIRE(std::equal(expected.begin(), expected.end(), actual.begin(), [](const pair& a, const pair& b) {
            return a.first == b.first && std::get<1>(a.second).id == std::get<1>(b.second).id &&
                   std::get<1>(a.second).customer_id == std::get<1>(b.second).customer_id;
        }));
    }
}