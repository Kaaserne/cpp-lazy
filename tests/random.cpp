#include <Lz/map.hpp>
#include <Lz/random.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <list>
#include <map>
#include <random>
#include <test_procs.hpp>
#include <unordered_map>

TEST_CASE("random_iterable should be random") {
    constexpr std::size_t size = 5;

    SECTION("random_iterable doubles") {
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

    SECTION("random_iterable ints") {
        const auto random_array =
            lz::random((std::numeric_limits<int>::min)(), (std::numeric_limits<int>::max)(), size) | lz::to<std::array<int, size>>();
        const auto randomArray2 =
            lz::random((std::numeric_limits<int>::min)(), (std::numeric_limits<int>::max)(), size) | lz::to<std::array<int, size>>();
        REQUIRE(random_array != randomArray2);
    }

    SECTION("Operator=") {
        auto random = lz::random(0., 1., size);
        auto it = random.begin();
        REQUIRE(it != random.end());
        it = random.end();
        REQUIRE(it == random.end());
    }
}

TEST_CASE("random_iterable with custom distro's and custom engine") {
    static std::random_device rd;
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
    SECTION("Empty random") {
        lz::random_iterable<int, std::uniform_int_distribution<>, std::mt19937> r = lz::random(0, 0, 0);
        REQUIRE(lz::empty(r));
    }

    SECTION("One element random") {
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

    SECTION("Operator++") {
        auto it = random.begin();
        REQUIRE(lz::distance(it, random.end()) == 5);
        ++it;
        REQUIRE(*it >= 0.);
        REQUIRE(*it <= 1.);
        REQUIRE(lz::distance(it, random.end()) == 4);
    }

    SECTION("Operator--") {
        auto it = random.end();
        REQUIRE(-lz::distance(it, random.begin()) == 5);
        --it;
        REQUIRE(*it >= 0.);
        REQUIRE(*it <= 1.);
        REQUIRE(lz::distance(random.begin(), it) == 4);
    }

    SECTION("Operator== & Operator!=") {
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

    SECTION("Operator*") {
        auto it = random.begin();
        REQUIRE(*it >= 0.);
        REQUIRE(*it <= 1.);
    }

    SECTION("Operator+") {
        std::vector<double> dummy = { 1., 1., 1., 1., 1. };
        test_procs::test_operator_plus(random, dummy, [](double a, double) { return a >= 0. && a <= 1.; });
    }

    SECTION("Operator-") {
        test_procs::test_operator_minus(random);
    }

    SECTION("Operator-(default_sentinel_t)") {
        auto rand = lz::random(0., 1., size);
        test_procs::test_operator_minus(rand);
    }
}

TEST_CASE("random_iterable to containers") {
    constexpr std::size_t size = 10;
    lz::default_random_iterable<double> range = lz::random(0., 1., size);

    SECTION("To array") {
        REQUIRE((range | lz::to<std::array<double, size>>()).size() == size);
        REQUIRE(lz::all_of(range, [](double val) { return val >= 0. && val <= 1.; }));
    }

    SECTION("To vector") {
        REQUIRE((range | lz::to<std::vector>()).size() == size);
    }

    SECTION("To other container using to<>()") {
        REQUIRE((range | lz::to<std::list>()).size() == size);
    }

    SECTION("To map") {
        auto actual = range | lz::map([](const double i) { return std::make_pair(i, i); }) | lz::to<std::map<double, double>>();
        REQUIRE(actual.size() == size);
    }

    SECTION("To unordered map") {
        auto actual =
            range | lz::map([](const double i) { return std::make_pair(i, i); }) | lz::to<std::unordered_map<double, double>>();
        REQUIRE(actual.size() == size);
    }
}
