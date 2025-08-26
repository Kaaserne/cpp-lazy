#include <Lz/common.hpp>
#include <Lz/map.hpp>
#include <Lz/random.hpp>
#include <Lz/reverse.hpp>
#include <cpp-lazy-ut-helper/test_procs.hpp>
#include <doctest/doctest.h>
#include <pch.hpp>
#include <random>
// TODO reverse
TEST_CASE("random_iterable should be random") {
    constexpr std::size_t size = 5;

    SUBCASE("random_iterable doubles") {
        constexpr long double start = 0;
        constexpr long double end = 1;
        const auto random_array = lz::random(start, end, size) | lz::to<std::array<long double, size>>();
        REQUIRE(random_array.size() == size);
        auto randomArray2 = lz::random(start, end, size) | lz::to<std::array<long double, size>>();
        while (random_array == randomArray2) {
            randomArray2 = lz::random(start, end, size) | lz::to<std::array<long double, size>>();
        }
        REQUIRE(random_array != randomArray2);
    }

    SUBCASE("random_iterable ints") {
        const auto random_array = lz::random((std::numeric_limits<int>::min)(), (std::numeric_limits<int>::max)(), size) |
                                  lz::to<std::array<int, size>>();
        const auto random_array_2 = lz::random((std::numeric_limits<int>::min)(), (std::numeric_limits<int>::max)(), size) |
                                    lz::to<std::array<int, size>>();
        REQUIRE(random_array != random_array_2);
    }

    SUBCASE("Operator=") {
        auto random = lz::random(1, 10, 5);
        auto common = lz::common(random);

        REQUIRE(lz::size(common) == 5);
        auto dummy = { 0, 0, 0, 0, 0 };
        REQUIRE(lz::equal(common, dummy, [](int a, int) { return a <= 10 && a >= 0; }));
    }
}

TEST_CASE("random_iterable with custom distro's and custom engine") {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::poisson_distribution<> d(500000);
    lz::random_iterable<int, std::poisson_distribution<>, std::mt19937_64> r = lz::random(d, gen, 3);
    static_assert(!std::is_same<decltype(r.begin()), decltype(r.end())>::value, "Should not be the same");
    REQUIRE(lz::distance(r) == 3);

    const auto current_rand = *r.begin();
    auto next_rand = *r.begin();
    while (current_rand == next_rand) {
        next_rand = *r.begin();
    }
    REQUIRE(current_rand != next_rand);
}

TEST_CASE("Empty or one element random") {
    SUBCASE("Empty random") {
        lz::random_iterable<int, std::uniform_int_distribution<>, std::mt19937> r = lz::random(0, 0, 0);
        REQUIRE(lz::empty(r));
    }

    SUBCASE("One element random") {
        auto r = lz::random(0, 0, 1);
        REQUIRE_FALSE(lz::empty(r));
        REQUIRE(lz::has_one(r));
        REQUIRE_FALSE(lz::has_many(r));
    }
}

TEST_CASE("random_iterable binary operations") {
    constexpr std::ptrdiff_t size = 5;
    lz::common_random_iterable<double, std::uniform_real_distribution<>, std::mt19937> random = lz::common_random(0., 1., size);
    static_assert(std::is_same<decltype(random.begin()), decltype(random.end())>::value, "Should be the same");

    SUBCASE("Operator++") {
        auto it = random.begin();
        REQUIRE(lz::distance(it, random.end()) == 5);
        ++it;
        REQUIRE(*it >= 0.);
        REQUIRE(*it <= 1.);
        REQUIRE(lz::distance(it, random.end()) == 4);
    }

    SUBCASE("Operator--") {
        auto it = random.end();
        REQUIRE(-lz::distance(it, random.begin()) == 5);
        --it;
        REQUIRE(*it >= 0.);
        REQUIRE(*it <= 1.);
        REQUIRE(lz::distance(random.begin(), it) == 4);
    }

    SUBCASE("Operator== & Operator!=") {
        auto it = random.begin();
        REQUIRE(it != random.end());
        REQUIRE(it == random.begin());
        REQUIRE(random.end() != it);
        REQUIRE(random.begin() == it);

        while (it != random.end()) {
            ++it;
        }

        REQUIRE(it == random.end());
        REQUIRE(it != random.begin());
        REQUIRE(random.end() == it);
        REQUIRE(random.begin() != it);
    }

    SUBCASE("Operator*") {
        auto it = random.begin();
        REQUIRE(*it >= 0.);
        REQUIRE(*it <= 1.);
    }

    SUBCASE("Operator+") {
        std::vector<double> dummy = { 1., 1., 1., 1., 1. };
        test_procs::test_operator_plus(random, dummy, [](double a, double) { return a >= 0. && a <= 1.; });
    }

    SUBCASE("Operator-") {
        test_procs::test_operator_minus(random);
    }

    SUBCASE("Operator-(default_sentinel_t)") {
        auto rand = lz::random(0., 1., size);
        test_procs::test_operator_minus(rand);
    }

    SUBCASE("Operator+(default_sentinel_t)") {
        auto rand = lz::random(0., 1., size);
        std::vector<double> dummy(size, 0.);
        test_procs::test_operator_plus(rand, dummy, [](double a, double) { return a >= 0. && a <= 1.; });
    }
}

TEST_CASE("random_iterable to containers") {
    constexpr std::size_t size = 10;
    lz::default_random_iterable<double> range = lz::random(0., 1., size);

    SUBCASE("To array") {
        REQUIRE((range | lz::to<std::array<double, size>>()).size() == size);
        REQUIRE(lz::all_of(range, [](double val) { return val >= 0. && val <= 1.; }));
    }

    SUBCASE("To vector") {
        REQUIRE((range | lz::to<std::vector>()).size() == size);
    }

    SUBCASE("To other container using to<>()") {
        REQUIRE((range | lz::to<std::list>()).size() == size);
    }

    SUBCASE("To map") {
        auto actual = range | lz::map([](const double i) { return std::make_pair(i, i); }) | lz::to<std::map<double, double>>();
        REQUIRE(actual.size() == size);
    }

    SUBCASE("To unordered map") {
        auto actual =
            range | lz::map([](const double i) { return std::make_pair(i, i); }) | lz::to<std::unordered_map<double, double>>();
        REQUIRE(actual.size() == size);
    }
}
