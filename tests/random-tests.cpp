#include <Lz/Random.hpp>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("Random should be random", "[Random][Basic functionality]") {
    constexpr std::size_t size = 5;

    SECTION("Random doubles") {
        const auto randomArray = lz::random<long double>(0., 1., size).toArray<size>();
        auto randomArray2 = lz::random<long double>(0., 1., size).toArray<size>();
        while (randomArray == randomArray2) {
            randomArray2 = lz::random<long double>(0., 1., size).toArray<size>();
        }
        REQUIRE(randomArray != randomArray2);
    }

    SECTION("Random ints") {
        const auto randomArray =
            lz::random((std::numeric_limits<int>::min)(), (std::numeric_limits<int>::max)(), size).toArray<size>();
        const auto randomArray2 =
            lz::random((std::numeric_limits<int>::min)(), (std::numeric_limits<int>::max)(), size).toArray<size>();
        REQUIRE(randomArray != randomArray2);
    }
}

TEST_CASE("Random with custom distro's and custom engine") {
    static std::random_device rd;
    std::mt19937_64 gen(rd());
    std::poisson_distribution<> d(500'000);
    auto r = lz::random(d, gen, 3);
    CHECK(std::distance(r.begin(), r.end()) == 3);

    const auto currentRand = r.nextRandom();
    auto nextRand = r.nextRandom();
    while (currentRand == nextRand) {
        nextRand = r.nextRandom();
    }
    REQUIRE(currentRand != nextRand);
}

TEST_CASE("Random binary operations", "[Random][Binary ops]") {
    constexpr std::ptrdiff_t size = 5;
    auto random = lz::random(0., 1., size);
    auto it = random.begin();

    SECTION("Operator++") {
        ++it;
        CHECK(std::distance(it, random.end()) == 4);
    }

    SECTION("Operator--") {
        ++it;
        CHECK(std::distance(it, random.end()) == 4);
        --it;
        CHECK(std::distance(it, random.end()) == 5);
    }

    SECTION("Operator== & Operator!=") {
        CHECK(it != random.end());
        it = random.end();
        CHECK(it == random.end());
    }

    SECTION("Operator+(int), tests += as well") {
        std::ptrdiff_t offset = 1;
        CHECK(std::distance(it + offset, random.end()) == size - offset);
    }

    SECTION("Operator-(int), tests -= as well") {
        ++it;
        CHECK(std::distance(it - 1, random.end()) == size);
    }

    SECTION("Operator-(Iterator)") {
        CHECK(random.end() - it == size);
        CHECK(std::distance(it, random.end()) == size);
    }

    SECTION("Operator[]()") {
        double prev = *it;
        double cur = it[1];
        while (cur == prev) {
            cur = it[1];
        }
        CHECK(cur != prev);
    }

    SECTION("Operator<, '<, <=, >, >='") {
        CHECK(it < random.end());
        CHECK(it + size + 1 > random.end());
        CHECK(it + size <= random.end());
        CHECK(it + size >= random.end());
    }
}

TEST_CASE("Random to containers", "[Random][To container]") {
    constexpr std::size_t size = 10;
    auto range = lz::random(0., 1., size);

    SECTION("To array") {
        CHECK(range.toArray<size>().size() == size);
    }

    SECTION("To vector") {
        CHECK(range.toVector().size() == size);
    }

    SECTION("To other container using to<>()") {
        CHECK(range.to<std::list>().size() == size);
    }

    SECTION("To map") {
        std::map<double, double> actual = range.toMap([](const double i) { return i; });
        CHECK(actual.size() == size);
    }

    SECTION("To unordered map") {
        std::unordered_map<double, double> actual = range.toUnorderedMap([](const double i) { return i; });
        CHECK(actual.size() == size);
    }
}
