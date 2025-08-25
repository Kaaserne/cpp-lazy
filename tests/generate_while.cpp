#include <Lz/generate_while.hpp>
#include <Lz/map.hpp>
#include <doctest/doctest.h>
#include <pch.hpp>

TEST_CASE("Generate while changing and creating elements") {
    const auto compile_test1 = lz::generate_while([]() { return std::make_pair(false, false); });
    static_cast<void>(compile_test1);

    const int i2 = 0;
    const auto compile_test2 = lz::generate_while([&i2]() {
        static_cast<void>(i2);
        return std::make_pair(false, false);
    });
    static_cast<void>(compile_test2);
    static_assert(!std::is_same<decltype(compile_test1.begin()), decltype(compile_test1.end())>::value, "Should be sentinel");

    int i = 0;
    auto gen_with_ref = lz::generate_while(
        [&i]() -> std::pair<int, bool> { return {i, i == 4}; });

    SUBCASE("Should be 0, 1, 2, 3") {
        auto generator = lz::generate_while([&i]() {
            auto copy = i++;
            return std::make_pair(copy, copy != 4);
        });
        std::array<int, 4> expected = { 0, 1, 2, 3 };
        auto actual = generator | lz::to<std::array<int, expected.size()>>();
        REQUIRE(expected == actual);
        i = 0;
    }

    SUBCASE("Operator=") {
      SUBCASE("Empty") {
        auto begin = gen_with_ref.begin();
        // this range is empty
        REQUIRE(begin == gen_with_ref.end());
        REQUIRE(gen_with_ref.end() == begin);
        REQUIRE_FALSE(begin != gen_with_ref.end());
        REQUIRE_FALSE(gen_with_ref.end() != begin);

        begin = lz::default_sentinel;
        REQUIRE(begin == gen_with_ref.end());
        REQUIRE(gen_with_ref.end() == begin);
        REQUIRE_FALSE(begin != gen_with_ref.end());
        REQUIRE_FALSE(gen_with_ref.end() != begin);
      }

        SUBCASE("Non empty") {
          i = 0;
          auto generator = lz::generate_while([&i]() {
            auto copy = i++;
            return std::make_pair(copy, copy != 4);
          });
          auto it = generator.begin();
          REQUIRE(it != generator.end());
          REQUIRE(generator.end() != it);
          REQUIRE_FALSE(it == generator.end());
          REQUIRE_FALSE(generator.end() == it);

          it = lz::default_sentinel;
          REQUIRE(it == generator.end());
          REQUIRE(generator.end() == it);
          REQUIRE_FALSE(it != generator.end());
          REQUIRE_FALSE(generator.end() != it);

          auto end = generator.begin();
          end = lz::default_sentinel;
          REQUIRE(end != generator.begin());
          REQUIRE(generator.begin() != end);
          REQUIRE_FALSE(end == generator.begin());
          REQUIRE_FALSE(generator.begin() == end);

          std::vector<int> expected = {0, 1, 2, 3};
          std::vector<int> actual(generator.begin(), end);
          REQUIRE(expected == actual);

          i = 0;
        }
    }
}

TEST_CASE("Empty or one element generate while") {
    SUBCASE("Empty") {
      std::function<std::pair<bool, int>()> func = []() {
        return std::make_pair(0, false);
      };
      lz::generate_while_iterable<decltype(func)> generator =
          lz::generate_while(std::move(func));
      REQUIRE(lz::empty(generator));
      REQUIRE_FALSE(lz::has_one(generator));
      REQUIRE_FALSE(lz::has_many(generator));
    }

    SUBCASE("One element") {
        bool b = true;
        auto generator = lz::generate_while([&b]() {
          auto p = std::make_pair(0, b);
          b = false;
          return p;
        });
        REQUIRE_FALSE(lz::empty(generator));
        REQUIRE(lz::has_one(generator));
        REQUIRE_FALSE(lz::has_many(generator));
    }
}

TEST_CASE("Generate while binary operations") {
  auto test = lz::generate_while([]() { return std::make_pair(0, true); });
  static_assert(std::is_same<decltype(*test.begin()), int>::value,
                "int and decltype(*generator.begin()) are not the same");

  SUBCASE("Operator++") {
    int i = 0;
    auto generator = lz::generate_while([&i]() {
      auto copy = i++;
      return std::make_pair(copy, copy != 4);
    });
    auto expected = std::vector<int>{0, 1, 2, 3};
    REQUIRE(lz::equal(generator, expected));
    }

    SUBCASE("Operator== & Operator!=") {
        int i = 0;
        auto generator = lz::generate_while([&i]() {
            auto copy = i++;
            return std::make_pair(copy, copy != 4);
        });
        auto begin = generator.begin();
        ++begin;
        REQUIRE(begin != generator.begin());
        REQUIRE(generator.begin() != begin);
        REQUIRE_FALSE(begin == generator.begin());
        REQUIRE_FALSE(generator.begin() == begin);
        ++begin;
        ++begin;
        ++begin;
        REQUIRE(begin == generator.end());
        begin = generator.begin();
        i = 0;
        REQUIRE(*begin == 0);
        while (begin != generator.end()) {
            ++begin;
        }
        REQUIRE(begin == generator.end());
    }
}

TEST_CASE("Generate while to containers") {
    SUBCASE("To array") {
        int i = 0;
        auto generator = lz::generate_while([&i]() {
            auto copy = i++;
            return std::make_pair(copy, copy != 4);
        });
        std::array<int, 4> expected = { 0, 1, 2, 3 };
        auto actual = generator | lz::to<std::array<int, 4>>();
        REQUIRE(expected == actual);
    }

    SUBCASE("To vector") {
        int i = 0;
        auto generator = lz::generate_while([&i]() {
            auto copy = i++;
            return std::make_pair(copy, copy != 4);
        });
        auto it = generator.begin();
        it = generator.end();
        REQUIRE(it == generator.end());
        REQUIRE(generator.end() == it);
        REQUIRE_FALSE(it == generator.begin());
        REQUIRE(it != generator.begin());
        REQUIRE(generator.begin() != it);
        std::vector<int> expected = { 0, 1, 2, 3 };
        auto actual = generator | lz::to<std::vector>();
        REQUIRE(expected == actual);
    }

    SUBCASE("To other container using to<>()") {
        int i = 0;
        auto generator = lz::generate_while([&i]() {
            auto copy = i++;
            return std::make_pair(copy, copy != 4);
        });
        std::list<int> expected = { 0, 1, 2, 3 };
        auto actual = generator | lz::to<std::list<int>>();
        REQUIRE(expected == actual);
    }

    SUBCASE("To map") {
        int i = 0;
        auto generator = lz::generate_while([&i]() {
            auto copy = i++;
            return std::make_pair(copy, copy != 4);
        });
        std::map<int, int> expected = { { 0, 0 }, { 1, 1 }, { 2, 2}, { 3, 3 }};
        auto actual = generator | lz::map([](int x) { return std::make_pair(x, x); }) | lz::to<std::map<int, int>>();
        REQUIRE(actual == expected);
    }

    SUBCASE("To unordered map") {
        int i = 0;
        auto generator = lz::generate_while([&i]() {
            auto copy = i++;
            return std::make_pair(copy, copy != 4);
        });
        std::unordered_map<int, int> expected = { { 0, 0 }, { 1, 1 }, { 2, 2}, { 3, 3 }};
        auto actual = generator | lz::map([](int x) { return std::make_pair(x, x); }) | lz::to<std::unordered_map<int, int>>();
        REQUIRE(actual == expected);
    }
}
