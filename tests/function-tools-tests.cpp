#include "Lz/FunctionTools.hpp"
#include "Lz/Range.hpp"
#include "Lz/View.hpp"

#include <catch2/catch.hpp>
#include <cctype>

TEST_CASE("Function tools") {
    std::vector<int> ints = { 1, 2, 3, 4 };
    std::vector<double> doubles = { 1.2, 2.5, 3.3, 4.5 };

    SECTION("Mean") {
        double avg = lz::mean(ints);
        CHECK(avg == Approx((1. + 2. + 3. + 4.) / 4.));
    }

    SECTION("Median") {
        double median = lz::median(doubles);
        CHECK(median == Approx((2.5 + 3.3) / 2.));

        doubles.push_back(3.3);
        median = lz::median(doubles);
        CHECK(median == Approx(3.3));
    }

    SECTION("For each while") {
        std::array<int, 10> arr = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        lz::forEachWhile(arr, [](int& i) {
            const auto performNextIteration = i < 5;
            if (performNextIteration) {
                i = 0;
            }
            return performNextIteration;
        });
        CHECK(arr[4] == 5);
        CHECK(arr[3] == 0);
    }

    SECTION("Keys and values") {
        const std::map<int, int> map = { { 1, 2 }, { 2, 3 }, { 3, 4 } };
        auto values = lz::values(map);
        auto expectedValues = { 2, 3, 4 };
        CHECK(lz::equal(values, expectedValues));

        auto keys = lz::keys(map);
        auto expectedKeys = { 1, 2, 3 };
        CHECK(lz::equal(keys, expectedKeys));
    }

    SECTION("Unlines") {
        std::vector<std::string> strings = { "hello", "world", "what's", "up" };
        auto unlines = lz::unlines(strings).toString();
        CHECK(unlines == "hello\nworld\nwhat's\nup");
    }

    SECTION("Lines") {
        std::string string = "aa\nbb\nbb";
        auto lines = lz::lines<std::string>(string).toVector();
        CHECK(lines == std::vector<std::string>{ "aa", "bb", "bb" });
    }

    SECTION("Pairwise") {
        auto x = lz::pairwise(ints).toVector();
        CHECK(x == std::vector<std::tuple<int, int>>{ std::make_tuple(1, 2), std::make_tuple(2, 3), std::make_tuple(3, 4) });
    }

    SECTION("As") {
        auto floats = lz::as<float>(ints).toVector();
        CHECK(std::is_same<typename decltype(floats)::value_type, float>::value);
        CHECK(floats == std::vector<float>{ 1., 2., 3., 4. });
    }

    SECTION("First or default") {
        std::vector<std::string> s = { "hello world!", "what's up" };
        std::string toFind = "hel";
        std::string def = "default";

        toFind = lz::findFirstOrDefault(s, toFind, def);
        CHECK(toFind == "default");

        toFind = "hello world!";
        toFind = lz::findFirstOrDefault(s, toFind, def);
        CHECK(toFind == "hello world!");

        def = ' ';
        toFind = lz::findFirstOrDefaultIf(
            s, [](const std::string& a) { return a.find('!') != std::string::npos; }, def);
        CHECK(toFind == "hello world!");

        toFind = lz::findFirstOrDefaultIf(
            s, [](const std::string& a) { return a.find('z') != std::string::npos; }, "default");
        CHECK(toFind == "default");
    }

    SECTION("Last or default") {
        std::vector<std::string> s = { "hello 'world", "what's up" };
        std::string toFind = "hel";
        std::string def = "default";

        toFind = lz::findLastOrDefault(s, toFind, def);
        CHECK(toFind == def);

        toFind = lz::findLastOrDefaultIf(
            s, [](const std::string& a) { return a.find('\'') != std::string::npos; }, def);
        CHECK(toFind == "what's up");

        toFind = lz::findLastOrDefaultIf(
            s, [](const std::string& a) { return lz::contains(a, 'q'); }, def);
        CHECK(toFind == def);
    }

    SECTION("Position") {
        std::vector<char> c = { 'a', 'b', 'c', 'd' };
        std::size_t pos = lz::indexOf(c, 'b');
        CHECK(pos == 1);

        pos = lz::indexOf(c, 'e');
        CHECK(pos == lz::npos);

        std::vector<std::string> strings = { "hello", "world" };
        pos = lz::indexOfIf(strings, [](const std::string& s) { return s.find('o') != std::string::npos; });
        CHECK(pos == 0);

        pos = lz::indexOfIf(strings, [](const std::string& s) { return s.find('q') != std::string::npos; });
        CHECK(pos == lz::npos);
    }

    SECTION("FilterMap") {
        std::string s = "123,d35dd";
        auto f = lz::filterMap(
            s, [](const char c) { return static_cast<bool>(std::isdigit(c)); },
            [](const char c) { return static_cast<int>(c - '0'); });
        auto expected = { 1, 2, 3, 3, 5 };
        CHECK(std::equal(f.begin(), f.end(), expected.begin()));
    }

    SECTION("To string func") {
        std::vector<int> v = { 1, 2, 3, 4, 5 };
        auto dummy = lz::map(v, [](int i) { return i; });

        CHECK(dummy.toString() == "12345");
        CHECK(dummy.toString(" ") == "1 2 3 4 5");
        CHECK(dummy.toString(", ") == "1, 2, 3, 4, 5");
    }

    SECTION("Reverse") {
        std::string s = "hello";
        CHECK(lz::reverse(s).toString() == "olleh");
    }

    SECTION("Last, first, length, isEmpty") {
        std::vector<int> vec = { 1, 3, 5, 7, 9 };
        auto map = lz::map(vec, [](const int i) { return i + 1; });
        auto emptySequence = lz::view(vec.end(), vec.end());

        CHECK(std::distance(emptySequence.begin(), emptySequence.end()) == 0);
        CHECK(lz::empty(emptySequence));
        CHECK(static_cast<std::size_t>(std::distance(map.begin(), map.end())) == vec.size());

        CHECK(lz::front(map) == 2);
        CHECK(lz::back(map) == 10);

        CHECK(lz::frontOr(emptySequence, 10) == 10);
        CHECK(lz::backOr(emptySequence, 10) == 10);

        CHECK(lz::frontOr(map, 10) == 2);
        CHECK(lz::backOr(map, 10) == 10);

        CHECK(lz::hasMany(map));

        std::string splitter = "hello world";
        auto s = lz::split(splitter, " ").toVector();
    }

    SECTION("Contains") {
        std::string s = "hello";
        CHECK(lz::contains(s, 'h'));
        CHECK(!lz::contains(s, 'x'));

        CHECK(lz::containsIf(s, [](char c) { return c == 'h'; }));
    }

    SECTION("select") {
        std::array<int, 10> range = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        std::function<bool(int)> even = [](int i) {
            return i % 2 == 0;
        };
        auto selectors = lz::map(range, std::move(even));
        auto selected = lz::select(range, std::move(selectors));
        CHECK(selected.toVector() == std::vector<int>{ 0, 2, 4, 6, 8 });
    }

    SECTION("Median") {
        std::array<int, 5> arr = { 5, 2, 3, 1, 7 };
        CHECK(lz::median(arr) == 3);
        CHECK(lz::median(arr, std::less<int>())); // NOLINT

        std::array<int, 4> arr2 = { 3, 5, 1, 9 };
        CHECK(lz::median(arr2) == (3 + 5) / 2.);
        CHECK(lz::median(arr2, std::less<int>()) == (3 + 5) / 2.); // NOLINT
    }

    SECTION("Zip with") {
        std::vector<int> v = { 1, 2, 3 };
        std::vector<int> v2 = { 1, 2, 3 };

        auto zipper = lz::zipWith(
                          [](int& a, int b) {
                              int tmp = a;
                              a = 0;
                              return tmp + b;
                          },
                          v, v2)
                          .toVector();

        CHECK(zipper == std::vector<int>{ 2, 4, 6 });
        CHECK(v == std::vector<int>{ 0, 0, 0 }); // ref check
    }

    SECTION("Trimming") {
        std::string toTrim = "\n\n  Hello world    \t\t";
        auto spaceFn = [](char c) {
            return std::isspace(c);
        };
        auto trimming = lz::trim(toTrim, spaceFn, spaceFn);
        CHECK(trimming.toString() == "Hello world");
    }
}
