#include <Lz/map.hpp>
#include <Lz/repeat.hpp>
#include <Lz/reverse.hpp>
#include <cpp-lazy-ut-helper/c_string.hpp>
#include <cpp-lazy-ut-helper/test_procs.hpp>
#include <doctest/doctest.h>
#include <pch.hpp>

struct TestStruct {
    std::string test_field_str;
    int test_field_int;
};

TEST_CASE("Map with sentinels") {
    auto cstr = lz::c_string("Hello, World!");
    auto map = lz::map(cstr, [](char c) { return std::toupper(c); });
    static_assert(!std::is_same<decltype(map.end()), decltype(map.begin())>::value, "Should be sentinels");
    auto c_str_expected = lz::c_string("HELLO, WORLD!");
    REQUIRE(lz::equal(map, c_str_expected));

    SUBCASE("Operator=") {
        auto repeater = lz::repeat(20, 5);
        auto end = repeater.begin();
        end = repeater.end(); // calls operator=(sentinel)
        auto begin = repeater.begin();
        auto common = lz::map(lz::make_basic_iterable(begin, end), [](int i) { return i * 2; });

        auto expected = { 40, 40, 40, 40, 40 };
        REQUIRE(lz::equal(common, expected));
    }
}

TEST_CASE("Map changing and creating elements") {
    constexpr std::size_t size = 3;
    std::array<TestStruct, size> array = { TestStruct{ "FieldA", 1 }, TestStruct{ "FieldB", 2 }, TestStruct{ "FieldC", 3 } };

    SUBCASE("Should map out element") {
        auto map = lz::map(array, [](const TestStruct& t) { return t.test_field_str; });
        REQUIRE(map.size() == size);
        static_assert(std::is_same<decltype(map.end()), decltype(map.begin())>::value, "Should not be sentinels");
        static_assert(std::is_same<decltype(*map.begin()), std::string>::value, "Types to not match (decltype(*map.begin()) and std::string)");

        auto it = map.begin();
        REQUIRE(*it == "FieldA");
        REQUIRE(*(++it) == "FieldB");
        REQUIRE(*(++it) == "FieldC");
    }

    SUBCASE("Should be by reference") {
        std::size_t count = 0;
        std::function<std::string&(TestStruct&)> f = [&count, &array](TestStruct& t) -> std::string& {
            REQUIRE(&t == &array[count++]);
            return t.test_field_str;
        };
        lz::map_iterable<decltype(array), decltype(f)> map = array | lz::map(std::move(f));

        for (auto&& _ : map) {
            static_cast<void>(_);
        }
    }
}

TEST_CASE("Map binary operations") {
    constexpr std::size_t size = 3;
    std::array<TestStruct, size> array = { TestStruct{ "FieldA", 1 }, TestStruct{ "FieldB", 2 }, TestStruct{ "FieldC", 3 } };

    std::function<std::string(TestStruct)> f = [](const TestStruct& t) {
        return t.test_field_str;
    };
    auto map = lz::map(array, std::move(f));
    auto it = map.begin();

    SUBCASE("Operator++") {
        auto expected = std::vector<std::string>{ "FieldA", "FieldB", "FieldC" };
        REQUIRE(lz::equal(map, expected));
    }

    SUBCASE("Operator--") {
        auto expected = std::vector<std::string>{ "FieldC", "FieldB", "FieldA" };
        REQUIRE(lz::equal(map | lz::reverse, expected));
    }

    SUBCASE("Operator== & operator!=") {
        REQUIRE(it != map.end());
        it = map.end();
        REQUIRE(it == map.end());
    }

    SUBCASE("Operator+") {
        auto expected = std::vector<std::string>{ "FieldA", "FieldB", "FieldC" };
        test_procs::test_operator_plus(map, expected);
    }

    SUBCASE("Operator-") {
        test_procs::test_operator_minus(map);
    }

    SUBCASE("Operator- sentinel") {
        auto repeater = lz::repeat(20, 5);
        auto map_sentinel = lz::map(repeater, [](int i) { return i; });
        test_procs::test_operator_minus(map_sentinel);
    }

    SUBCASE("Operator+ sentinel") {
        auto repeater = lz::repeat(20, 5);
        auto map_sentinel = lz::map(repeater, [](int i) { return i; });
        std::vector<int> expected = { 20, 20, 20, 20, 20 };
        test_procs::test_operator_plus(map_sentinel, expected);
    }
}

TEST_CASE("Empty or one element map") {
    SUBCASE("Empty map") {
        std::vector<int> vec;
        auto map = lz::map(vec, [](int i) { return i; });
        REQUIRE(lz::empty(map));
    }

    SUBCASE("One element map") {
        std::vector<int> vec = { 1 };
        auto map = lz::map(vec, [](int i) { return i; });
        REQUIRE_FALSE(lz::empty(map));
        REQUIRE(lz::has_one(map));
        REQUIRE_FALSE(lz::has_many(map));
    }
}

TEST_CASE("Map to containers") {
    constexpr std::size_t size = 3;
    std::array<TestStruct, size> array = { TestStruct{ "FieldA", 1 }, TestStruct{ "FieldB", 2 }, TestStruct{ "FieldC", 3 } };
    auto map = lz::map(array, [](const TestStruct& t) { return t.test_field_str; });

    SUBCASE("To array") {
        auto str_array = map | lz::to<std::array<std::string, size>>();

        for (std::size_t i = 0; i < array.size(); i++) {
            REQUIRE(str_array[i] == array[i].test_field_str);
        }
    }

    SUBCASE("To vector") {
        auto str_vec = map | lz::to<std::vector>();

        for (std::size_t i = 0; i < array.size(); i++) {
            REQUIRE(str_vec[i] == array[i].test_field_str);
        }
    }

    SUBCASE("To other container using to()") {
        auto str_list = map | lz::to<std::list<std::string>>();
        auto list_iter = str_list.begin();

        for (std::size_t i = 0; i < array.size(); i++, ++list_iter) {
            REQUIRE(*list_iter == array[i].test_field_str);
        }
    }

    SUBCASE("To map") {
        std::map<std::string, std::string> actual = map | lz::map([](const std::string& s) { return std::make_pair(s, s); }) |
                                                    lz::to<std::map<std::string, std::string>>();

        std::map<std::string, std::string> expected = {
            std::make_pair("FieldA", "FieldA"),
            std::make_pair("FieldB", "FieldB"),
            std::make_pair("FieldC", "FieldC"),
        };

        REQUIRE(actual == expected);
    }

    SUBCASE("To unordered map") {
        std::unordered_map<std::string, std::string> actual = map |
                                                              lz::map([](const std::string& s) { return std::make_pair(s, s); }) |
                                                              lz::to<std::unordered_map<std::string, std::string>>();

        std::unordered_map<std::string, std::string> expected = {
            std::make_pair("FieldA", "FieldA"),
            std::make_pair("FieldB", "FieldB"),
            std::make_pair("FieldC", "FieldC"),
        };

        REQUIRE(actual == expected);
    }
}
