#include <Lz/c_string.hpp>
#include <Lz/map.hpp>
#include <catch2/catch.hpp>
#include <forward_list>
#include <list>
#include <map>
#include <unordered_map>

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

    SECTION("Operator=") {
        auto begin = map.begin();
        REQUIRE(begin == map.begin());
        begin = map.end();
        REQUIRE(begin == map.end());
    }
}

TEST_CASE("Map changing and creating elements") {
    constexpr std::size_t size = 3;
    std::array<TestStruct, size> array = { TestStruct{ "FieldA", 1 }, TestStruct{ "FieldB", 2 }, TestStruct{ "FieldC", 3 } };

    SECTION("Should map out element") {
        auto map = lz::map(array, [](const TestStruct& t) { return t.test_field_str; });
        REQUIRE(map.size() == size);
        static_assert(std::is_same<decltype(map.end()), decltype(map.begin())>::value, "Should not be sentinels");
        static_assert(std::is_same<decltype(*map.begin()), std::string>::value, "Types to not match (decltype(*map.begin()) and std::string)");

        auto it = map.begin();
        REQUIRE(*it == "FieldA");
        REQUIRE(*(++it) == "FieldB");
        REQUIRE(*(++it) == "FieldC");
    }

    SECTION("Should be by reference") {
        std::size_t count = 0;
        std::function<std::string&(TestStruct&)> f = [&count, &array](TestStruct& t) -> std::string& {
            REQUIRE(&t == &array[count++]);
            return t.test_field_str;
        };
        auto map = array | lz::map(std::move(f));

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

    SECTION("Operator++") {
        ++it;
        REQUIRE(*it == array[1].test_field_str);
    }

    SECTION("Operator--") {
        ++it;
        --it;
        REQUIRE(*it == array[0].test_field_str);
    }

    SECTION("Operator== & operator!=") {
        REQUIRE(it != map.end());
        it = map.end();
        REQUIRE(it == map.end());
    }

    SECTION("Operator+") {
        auto begin = map.begin();
        auto end = map.end();

        std::vector<std::string> expected = { "FieldA", "FieldB", "FieldC" };
        for (std::size_t i = 0; i < lz::size(map) - 1; ++i) {
            INFO("With i = " << i);
            REQUIRE(*(begin + static_cast<std::ptrdiff_t>(i)) == *(expected.begin() + static_cast<std::ptrdiff_t>(i)));
        }
        REQUIRE(begin + static_cast<std::ptrdiff_t>(lz::size(map)) == map.end());
        for (std::size_t i = 1; i <= lz::size(map); ++i) {
            INFO("With i = " << i);
            REQUIRE(*(end - static_cast<std::ptrdiff_t>(i)) == *(expected.end() - static_cast<std::ptrdiff_t>(i)));
        }
        REQUIRE(end - static_cast<std::ptrdiff_t>(lz::size(map)) == map.begin());

        std::advance(begin, static_cast<std::ptrdiff_t>(lz::size(map)));
        std::advance(end, -static_cast<std::ptrdiff_t>(lz::size(map)));

        for (std::size_t i = 0; i < lz::size(map) - 1; ++i) {
            INFO("With i = " << i);
            REQUIRE(*(end + static_cast<std::ptrdiff_t>(i)) == *(expected.begin() + static_cast<std::ptrdiff_t>(i)));
        }
        REQUIRE(end + static_cast<std::ptrdiff_t>(lz::size(map)) == map.end());
        for (std::size_t i = 1; i <= lz::size(map); ++i) {
            INFO("With i = " << i);
            REQUIRE(*(begin - static_cast<std::ptrdiff_t>(i)) == *(expected.end() - static_cast<std::ptrdiff_t>(i)));
        }
        REQUIRE(begin - static_cast<std::ptrdiff_t>(lz::size(map)) == map.begin());
    }

    SECTION("Operator-") {
        auto begin = map.begin();
        auto end = map.end();
        for (std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>(lz::size(map)); ++i) {
            INFO("With i = " << i);
            REQUIRE((end - i) - begin == static_cast<std::ptrdiff_t>(lz::size(map)) - i);
            REQUIRE(end - (begin + i) == static_cast<std::ptrdiff_t>(lz::size(map)) - i);
            REQUIRE((begin + i) - end == -(static_cast<std::ptrdiff_t>(lz::size(map)) - i));
            REQUIRE(begin - (end - i) == -(static_cast<std::ptrdiff_t>(lz::size(map)) - i));
        }

        for (std::size_t i = 0; i < lz::size(map) ; ++i) {
            INFO("With i = " << i);
            REQUIRE((end - static_cast<std::ptrdiff_t>(i)) - (begin + static_cast<std::ptrdiff_t>(i)) ==
                    static_cast<std::ptrdiff_t>(lz::size(map)) - 2 * static_cast<std::ptrdiff_t>(i));
            REQUIRE((begin + static_cast<std::ptrdiff_t>(i)) - (end - static_cast<std::ptrdiff_t>(i)) ==
                    -(static_cast<std::ptrdiff_t>(lz::size(map)) - 2 * static_cast<std::ptrdiff_t>(i)));
        }
    }
}

TEST_CASE("Empty or one element map") {
    SECTION("Empty map") {
        std::vector<int> vec;
        auto map = lz::map(vec, [](int i) { return i; });
        REQUIRE(lz::empty(map));
    }

    SECTION("One element map") {
        std::vector<int> vec = { 1 };
        auto map = lz::map(vec, [](int i) { return i; });
        REQUIRE(!lz::empty(map));
        REQUIRE(lz::has_one(map));
        REQUIRE(!lz::has_many(map));
    }
}

TEST_CASE("Map to containers") {
    constexpr std::size_t size = 3;
    std::array<TestStruct, size> array = { TestStruct{ "FieldA", 1 }, TestStruct{ "FieldB", 2 }, TestStruct{ "FieldC", 3 } };
    auto map = lz::map(array, [](const TestStruct& t) { return t.test_field_str; });

    SECTION("To array") {
        auto str_array = map | lz::to<std::array<std::string, size>>();

        for (std::size_t i = 0; i < array.size(); i++) {
            REQUIRE(str_array[i] == array[i].test_field_str);
        }
    }

    SECTION("To vector") {
        auto str_vec = map | lz::to<std::vector>();

        for (std::size_t i = 0; i < array.size(); i++) {
            REQUIRE(str_vec[i] == array[i].test_field_str);
        }
    }

    SECTION("To other container using to()") {
        auto str_list = map | lz::to<std::list<std::string>>();
        auto list_iter = str_list.begin();

        for (std::size_t i = 0; i < array.size(); i++, ++list_iter) {
            REQUIRE(*list_iter == array[i].test_field_str);
        }
    }

    SECTION("To map") {
        std::map<std::string, std::string> actual = map | lz::map([](const std::string& s) { return std::make_pair(s, s); }) |
                                                    lz::to<std::map<std::string, std::string>>();

        std::map<std::string, std::string> expected = {
            std::make_pair("FieldA", "FieldA"),
            std::make_pair("FieldB", "FieldB"),
            std::make_pair("FieldC", "FieldC"),
        };

        REQUIRE(actual == expected);
    }

    SECTION("To unordered map") {
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
