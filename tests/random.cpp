#include <Lz/map.hpp>
#include <Lz/random.hpp>
#include <catch2/catch.hpp>
#include <cstddef>
#include <list>
#include <map>
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
    auto r = lz::random(d, gen, 3);
    static_assert(!std::is_same<decltype(r.begin()), decltype(r.end())>::value, "Should not be the same");
    REQUIRE(lz::distance(r.begin(), r.end()) == 3);

    const auto current_rand = *r.begin();
    auto next_rand = *r.begin();
    while (current_rand == next_rand) {
        next_rand = *r.begin();
    }
    REQUIRE(current_rand != next_rand);
}

TEST_CASE("Empty or one element random") {
    SECTION("Empty random") {
        auto r = lz::random(0, 0, 0);
        REQUIRE(lz::empty(r));
    }

    SECTION("One element random") {
        auto r = lz::random(0, 0, 1);
        REQUIRE(!lz::empty(r));
        REQUIRE(lz::has_one(r));
        REQUIRE(!lz::has_many(r));
    }
}

TEST_CASE("random_iterable binary operations") {
    constexpr std::ptrdiff_t size = 5;
    auto random = lz::common_random(0., 1., size);
    static_assert(std::is_same<decltype(random.begin()), decltype(random.end())>::value, "Should be the same");

    SECTION("Operator++") {
        auto it = random.begin();
        ++it;
        REQUIRE(*it >= 0.);
        REQUIRE(*it <= 1.);
        REQUIRE(lz::distance(it, random.end()) == 4);
    }

    SECTION("Operator--") {
        auto it = random.end();
        --it;
        REQUIRE(*it >= 0.);
        REQUIRE(*it <= 1.);
        REQUIRE(lz::distance(random.begin(), it) == 4);
    }

    SECTION("Operator== & Operator!=") {
        auto it = random.begin();
        REQUIRE(it != random.end());
        while (it != random.end()) {
            ++it;
        }
        REQUIRE(it == random.end());
    }

    SECTION("Operator*") {
        auto it = random.begin();
        REQUIRE(*it >= 0.);
        REQUIRE(*it <= 1.);
    }

    SECTION("Operator+") {
        auto begin = random.begin();
        auto end = random.end();

        for (std::size_t i = 0; i < lz::size(random) - 1; ++i) {
            INFO("With i = " << i);
            REQUIRE(*(begin + static_cast<std::ptrdiff_t>(i)) >= 0.);
            REQUIRE(*(begin + static_cast<std::ptrdiff_t>(i)) <= 1.);
        }
        REQUIRE(begin + static_cast<std::ptrdiff_t>(lz::size(random)) == random.end());
        for (std::size_t i = 1; i <= lz::size(random); ++i) {
            INFO("With i = " << i);
            REQUIRE(*(end - static_cast<std::ptrdiff_t>(i)) >= 0.);
            REQUIRE(*(end - static_cast<std::ptrdiff_t>(i)) <= 1.);
        }
        REQUIRE(end - static_cast<std::ptrdiff_t>(lz::size(random)) == random.begin());

        std::advance(begin, static_cast<std::ptrdiff_t>(lz::size(random)));
        std::advance(end, -static_cast<std::ptrdiff_t>(lz::size(random)));

        for (std::size_t i = 0; i < lz::size(random) - 1; ++i) {
            INFO("With i = " << i);
            REQUIRE(*(end + static_cast<std::ptrdiff_t>(i)) >= 0.);
            REQUIRE(*(end + static_cast<std::ptrdiff_t>(i)) <= 1.);
        }
        REQUIRE(end + static_cast<std::ptrdiff_t>(lz::size(random)) == random.end());
        for (std::size_t i = 1; i <= lz::size(random); ++i) {
            INFO("With i = " << i);
            REQUIRE(*(begin - static_cast<std::ptrdiff_t>(i)) >= 0.);
            REQUIRE(*(begin - static_cast<std::ptrdiff_t>(i)) <= 1.);
        }
        REQUIRE(begin - static_cast<std::ptrdiff_t>(lz::size(random)) == random.begin());
    }

    SECTION("Operator-") {
        auto begin = random.begin();
        auto end = random.end();
        for (std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>(lz::size(random)); ++i) {
            INFO("With i = " << i);
            REQUIRE((end - i) - begin == static_cast<std::ptrdiff_t>(lz::size(random)) - i);
            REQUIRE(end - (begin + i) == static_cast<std::ptrdiff_t>(lz::size(random)) - i);
            REQUIRE((begin + i) - end == -(static_cast<std::ptrdiff_t>(lz::size(random)) - i));
            REQUIRE(begin - (end - i) == -(static_cast<std::ptrdiff_t>(lz::size(random)) - i));
        }

        for (std::size_t i = 0; i < lz::size(random); ++i) {
            INFO("With i = " << i);
            REQUIRE((end - static_cast<std::ptrdiff_t>(i)) - (begin + static_cast<std::ptrdiff_t>(i)) ==
                    static_cast<std::ptrdiff_t>(lz::size(random)) - 2 * static_cast<std::ptrdiff_t>(i));
            REQUIRE((begin + static_cast<std::ptrdiff_t>(i)) - (end - static_cast<std::ptrdiff_t>(i)) ==
                    -(static_cast<std::ptrdiff_t>(lz::size(random)) - 2 * static_cast<std::ptrdiff_t>(i)));
        }
    }
}

TEST_CASE("random_iterable to containers") {
    constexpr std::size_t size = 10;
    auto range = lz::random(0., 1., size);

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
        auto actual = range | lz::map([](const double i) { return std::make_pair(i, i); }) | lz::to<std::unordered_map<double, double>>();
        REQUIRE(actual.size() == size);
    }
}
