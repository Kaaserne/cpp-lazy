#include <Lz/algorithm.hpp>
#include <Lz/stream.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cpp-lazy-ut-helper/c_string.hpp>
#include <cpp-lazy-ut-helper/repeat.hpp>
#include <deque>
#include <forward_list>
#include <iostream>
#include <list>
#include <queue>
#include <set>
#include <sstream>
#include <unordered_set>

template<class T>
class custom_container {
    std::vector<T> _vec;
    std::size_t _expected_capacity{};

public:
    std::size_t expected_capacity() const {
        return _expected_capacity;
    }

    custom_container(std::size_t expected_capacity) : _expected_capacity(expected_capacity) {
    }

    void reserve(std::size_t size) {
        _vec.reserve(size);
    }

    std::vector<T>& vec() {
        return _vec;
    }
};

template<class T>
struct lz::custom_copier_for<custom_container<T>> {
    template<class Iterable>
    void copy(Iterable&& iterable, custom_container<T>& container) const {
        container.reserve(lz::eager_size(iterable));
        REQUIRE(container.vec().empty());
        lz::copy(std::forward<Iterable>(iterable), std::back_inserter(container.vec()));
    }
};

TEST_CASE("Formatting") {
    SECTION("Non-empty fmt") {
        std::array<int, 5> arr = { 1, 2, 3, 4, 5 };

        auto str = arr | lz::format;
        REQUIRE(str == "1, 2, 3, 4, 5");
        REQUIRE(lz::format(arr) == "1, 2, 3, 4, 5");

        str = arr | lz::format(", ", "{}");
        REQUIRE(str == "1, 2, 3, 4, 5");
        REQUIRE(lz::format(arr, ", ", "{}") == "1, 2, 3, 4, 5");

        str = arr | lz::format(", ", "{:02}");
        REQUIRE(str == "01, 02, 03, 04, 05");
        REQUIRE(lz::format(arr, ", ", "{:02}") == "01, 02, 03, 04, 05");

        str = arr | lz::format(",", "{:02}");
        REQUIRE(str == "01,02,03,04,05");
        REQUIRE(lz::format(arr, ",", "{:02}") == "01,02,03,04,05");

        std::streambuf* old_cout = std::cout.rdbuf();
        std::ostringstream oss;
        std::cout.rdbuf(oss.rdbuf());

        lz::format(arr, std::cout);
        REQUIRE(oss.str() == "1, 2, 3, 4, 5");
        oss.str("");

        lz::format(arr, std::cout, ",");
        REQUIRE(oss.str() == "1,2,3,4,5");
        oss.str("");

        lz::format(arr, std::cout, ", ", "{:02}");
        REQUIRE(oss.str() == "01, 02, 03, 04, 05");
        oss.str("");

        std::cout.rdbuf(old_cout);
    }

    SECTION("Empty fmt") {
        std::array<int, 0> arr = {};

        auto str = arr | lz::format;
        REQUIRE(str == "");
        REQUIRE(lz::format(arr) == "");

        str = arr | lz::format(",", "{}");
        REQUIRE(str == "");
        REQUIRE(lz::format(arr, ",", "{}") == "");

        str = arr | lz::format(",", "{:02}");
        REQUIRE(str == "");
        REQUIRE(lz::format(arr, ",", "{:02}") == "");

        str = arr | lz::format(",", "{:02}");
        REQUIRE(str == "");
        REQUIRE(lz::format(arr, ",", "{:02}") == "");
    }

    SECTION("One element fmt") {
        std::array<int, 1> arr = { 1 };

        auto str = arr | lz::format(", ", "{}");
        REQUIRE(str == "1");
        REQUIRE(lz::format(arr, ", ", "{}") == "1");

        str = arr | lz::format(", ", "{:02}");
        REQUIRE(str == "01");
        REQUIRE(lz::format(arr, ", ", "{:02}") == "01");
    }

    SECTION("Non empty ostream") {
        std::array<int, 5> arr = { 1, 2, 3, 4, 5 };
        lz::basic_iterable<std::array<int, 5>::iterator> iterable(arr);
        std::ostringstream oss;
        oss << iterable;
        REQUIRE(oss.str() == "1, 2, 3, 4, 5");

        oss.str("");

        oss << iterable;
        REQUIRE(oss.str() == "1, 2, 3, 4, 5");
    }

    SECTION("Empty ostream") {
        std::array<int, 0> arr = {};
        lz::basic_iterable<std::array<int, 0>::iterator> iterable(arr);
        std::ostringstream oss;
        oss << iterable;
        REQUIRE(oss.str() == "");
    }

    SECTION("One element ostream") {
        std::array<int, 1> arr = { 1 };
        lz::basic_iterable<std::array<int, 1>::iterator> iterable(arr.begin(), arr.end());
        std::ostringstream oss;
        oss << iterable;
        REQUIRE(oss.str() == "1");
    }
}

TEST_CASE("Size") {
    SECTION("lz::size") {
        std::vector<int> vec = { 1, 2, 3, 4, 5 };
        static_assert(std::is_unsigned<decltype(lz::size(vec))>::value, "lz::size should return an unsigned type");
        REQUIRE(lz::size(vec) == 5);

        int arr[] = { 1, 2, 3, 4, 5 };
        static_assert(std::is_unsigned<decltype(lz::size(arr))>::value, "lz::size should return an unsigned type");
        REQUIRE(lz::size(arr) == 5);
    }

    SECTION("lz::ssize") {
        std::vector<int> vec = { 1, 2, 3, 4, 5 };
        static_assert(std::is_signed<decltype(lz::ssize(vec))>::value, "lz::ssize should return a signed type");
        REQUIRE(lz::ssize(vec) == 5);

        int arr[] = { 1, 2, 3, 4, 5 };
        static_assert(std::is_signed<decltype(lz::ssize(arr))>::value, "lz::ssize should return a signed type");
        REQUIRE(lz::ssize(arr) == 5);
    }

    SECTION("Eager size random access") {
        std::vector<int> vec = { 1, 2, 3, 4, 5 };
        REQUIRE(lz::eager_size(vec) == 5);
    }

    SECTION("Eager size bidirectional") {
        std::list<int> lst = { 1, 2, 3, 4, 5 };
        REQUIRE(lz::eager_size(lst) == 5);
    }

    SECTION("Eager size forward") {
        std::forward_list<int> flst = { 1, 2, 3, 4, 5 };
        REQUIRE(lz::eager_size(flst) == 5);
    }

    SECTION("Eager size sentinel") {
        auto c_str = lz::c_string("Hello");
        REQUIRE(lz::eager_size(c_str) == 5);
    }
}

TEST_CASE("To iterable") {
    SECTION("To iterable random access") {
        std::vector<int> vec = { 1, 2, 3, 4, 5 };
        lz::basic_iterable<std::vector<int>::iterator> iterable(vec.begin(), vec.end());
        REQUIRE(lz::size(iterable) == 5);
    }

    SECTION("To iterable forward") {
        std::list<int> lst = { 1, 2, 3, 4, 5 };
        lz::sized_iterable<std::list<int>::iterator> iterable(lst);
        REQUIRE(lz::size(iterable) == 5);
    }

    SECTION("To iterable begin end random access") {
        std::vector<int> vec = { 1, 2, 3, 4, 5 };
        lz::basic_iterable<std::vector<int>::iterator> iterable(vec.begin(), vec.end());
        REQUIRE(lz::size(iterable) == 5);
    }

    SECTION("To iterable begin end bidirectional") {
        std::list<int> lst = { 1, 2, 3, 4, 5 };
        lz::basic_iterable<std::list<int>::iterator> iterable(lst.begin(), lst.end());
        static_assert(!lz::sized<decltype(iterable)::iterator>::value, "Should not be a sized iterator");
    }
}

TEST_CASE("To container") {
    SECTION("To cpp array same iterator pair") {
        const std::vector<int> vec = { 1, 2, 3, 4, 5 };
        auto arr = lz::to<std::array<int, 5>>(vec);
        REQUIRE(lz::equal(arr, vec));
        arr = vec | lz::to<std::array<int, 5>>();
        REQUIRE(lz::equal(arr, vec));
    }

    SECTION("To std list same iterator pair") {
        const std::vector<int> vec = { 1, 2, 3, 4, 5 };
        auto lst = lz::to<std::list<int>>(vec);
        REQUIRE(lz::equal(lst, vec));
        lst = vec | lz::to<std::list<int>>();
        REQUIRE(lz::equal(lst, vec));
    }

    SECTION("To vector same iterator pair") {
        const std::vector<int> v1 = { 1, 2, 3, 4, 5 };
        auto v2 = lz::to<std::vector<int>>(v1);
        REQUIRE(lz::equal(v1, v2));
        v2 = v1 | lz::to<std::vector<int>>();
        REQUIRE(lz::equal(v1, v2));
    }

    SECTION("To forward list same iterator pair") {
        const std::vector<int> vec = { 1, 2, 3, 4, 5 };
        auto flst = lz::to<std::forward_list<int>>(vec);
        REQUIRE(lz::equal(flst, vec));
        flst = vec | lz::to<std::forward_list<int>>();
        REQUIRE(lz::equal(flst, vec));
    }

    SECTION("To queue same iterator pair") {
        const std::vector<int> vec = { 1, 2, 3, 4, 5 };
        auto queue = lz::to<std::queue<int>>(vec);
        std::size_t i = 0;
        for (; !queue.empty(); queue.pop(), ++i) {
            REQUIRE(queue.front() == vec[i]);
        }
        queue = vec | lz::to<std::queue<int>>();
        for (i = 0; !queue.empty(); queue.pop(), ++i) {
            REQUIRE(queue.front() == vec[i]);
        }
    }

    SECTION("To deque same iterator pair") {
        const std::vector<int> vec = { 1, 2, 3, 4, 5 };
        auto deque = lz::to<std::deque<int>>(vec);
        REQUIRE(lz::equal(deque, vec));
        deque = vec | lz::to<std::deque<int>>();
        REQUIRE(lz::equal(deque, vec));
    }

    SECTION("To set same iterator pair") {
        const std::vector<int> vec = { 1, 2, 3, 4, 5 };
        auto set = lz::to<std::set<int>>(vec);
        REQUIRE(set.size() == vec.size());
        for (std::size_t i = 0; i < vec.size(); ++i) {
            REQUIRE(set.find(vec[i]) != set.end());
        }
        set = vec | lz::to<std::set<int>>();
        REQUIRE(set.size() == vec.size());
        for (std::size_t i = 0; i < vec.size(); ++i) {
            REQUIRE(set.find(vec[i]) != set.end());
        }
    }

    SECTION("To unordered set same iterator pair") {
        const std::vector<int> vec = { 1, 2, 3, 4, 5 };
        auto set = lz::to<std::unordered_set<int>>(vec);
        REQUIRE(set.size() == vec.size());
        for (std::size_t i = 0; i < vec.size(); ++i) {
            REQUIRE(set.find(vec[i]) != set.end());
        }
        set = vec | lz::to<std::unordered_set<int>>();
        REQUIRE(set.size() == vec.size());
        for (std::size_t i = 0; i < vec.size(); ++i) {
            REQUIRE(set.find(vec[i]) != set.end());
        }
    }

    SECTION("To custom container same iterator pair") {
        const std::vector<int> vec = { 1, 2, 3, 4, 5 };
        auto container = lz::to<custom_container<int>>(vec, vec.size());
        REQUIRE(lz::equal(container.vec(), vec));
        container = vec | lz::to<custom_container<int>>(vec.size());
        REQUIRE(lz::equal(container.vec(), vec));
    }

    SECTION("To cpp array sentinel iterator pair") {
        auto c_str = lz::c_string("Hello");
        auto arr = lz::to<std::array<char, 5>>(c_str);
        REQUIRE(lz::equal(arr, c_str));
        arr = c_str | lz::to<std::array<char, 5>>();
        REQUIRE(lz::equal(arr, c_str));
    }

    SECTION("To std list sentinel iterator pair") {
        auto c_str = lz::c_string("Hello");
        auto lst = lz::to<std::list<char>>(c_str);
        REQUIRE(lz::equal(lst, c_str));
        lst = c_str | lz::to<std::list<char>>();
        REQUIRE(lz::equal(lst, c_str));
    }

    SECTION("To vector sentinel iterator pair") {
        auto c_str = lz::c_string("Hello");
        auto vec = lz::to<std::vector<char>>(c_str);
        REQUIRE(lz::equal(vec, c_str));
        vec = c_str | lz::to<std::vector<char>>();
        REQUIRE(lz::equal(vec, c_str));
    }

    SECTION("To forward list sentinel iterator pair") {
        auto c_str = lz::c_string("Hello");
        auto flst = lz::to<std::forward_list<char>>(c_str);
        REQUIRE(lz::equal(flst, c_str));
        flst = c_str | lz::to<std::forward_list<char>>();
        REQUIRE(lz::equal(flst, c_str));
    }

    SECTION("To queue sentinel iterator pair") {
        auto c_str = lz::c_string("Hello");
        auto queue = lz::to<std::queue<char>>(c_str);
        std::size_t i = 0;
        for (; !queue.empty(); queue.pop(), ++i) {
            REQUIRE(queue.front() == *std::next(c_str.begin(), static_cast<std::ptrdiff_t>(i)));
        }
        queue = c_str | lz::to<std::queue<char>>();
        i = 0;
        for (; !queue.empty(); queue.pop(), ++i) {
            REQUIRE(queue.front() == *std::next(c_str.begin(), static_cast<std::ptrdiff_t>(i)));
        }
    }

    SECTION("To deque sentinel iterator pair") {
        auto c_str = lz::c_string("Hello");
        auto deque = lz::to<std::deque<char>>(c_str);
        REQUIRE(lz::equal(deque, c_str));
        deque = c_str | lz::to<std::deque<char>>();
        REQUIRE(lz::equal(deque, c_str));
    }

    SECTION("To set sentinel iterator pair") {
        auto c_str = lz::c_string("Helo");
        auto set = lz::to<std::set<char>>(c_str);
        REQUIRE(set.size() == lz::eager_size(c_str));
        lz::for_each(c_str, [&set](char c) { REQUIRE(set.find(c) != set.end()); });
        set = c_str | lz::to<std::set<char>>();
        REQUIRE(set.size() == lz::eager_size(c_str));
        lz::for_each(c_str, [&set](char c) { REQUIRE(set.find(c) != set.end()); });
    }

    SECTION("To unordered set sentinel iterator pair") {
        auto c_str = lz::c_string("Helo");
        auto set = lz::to<std::unordered_set<char>>(c_str);
        REQUIRE(set.size() == lz::eager_size(c_str));
        lz::for_each(c_str, [&set](char c) { REQUIRE(set.find(c) != set.end()); });
        set = c_str | lz::to<std::unordered_set<char>>();
        REQUIRE(set.size() == lz::eager_size(c_str));
        lz::for_each(c_str, [&set](char c) { REQUIRE(set.find(c) != set.end()); });
    }

    SECTION("To custom container same iterator pair") {
        const auto c_str = lz::c_string("Hello");
        auto container = lz::to<custom_container<int>>(c_str, std::size_t{ 0 });
        REQUIRE(lz::equal(container.vec(), c_str));
        container = c_str | lz::to<custom_container<int>>(std::size_t{ 0 });
        REQUIRE(lz::equal(container.vec(), c_str));
    }
}

TEST_CASE("Empty") {
    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::empty(iterable));
    }

    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE_FALSE(lz::empty(iterable));
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        REQUIRE_FALSE(lz::empty(iterable));
    }
}

TEST_CASE("Has one") {
    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        REQUIRE_FALSE(lz::has_one(iterable));
    }

    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE_FALSE(lz::has_one(iterable));
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::has_one(iterable));
    }
}

TEST_CASE("Has many") {
    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        REQUIRE_FALSE(lz::has_many(iterable));
    }

    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::has_many(iterable));
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        REQUIRE_FALSE(lz::has_many(iterable));
    }
}

TEST_CASE("Front") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::front(iterable) == 'H');
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::front(iterable) == 'H');
    }
}

TEST_CASE("Back") {
    SECTION("With non-empty") {
        std::vector<int> vec = { 1, 2, 3 };
        REQUIRE(lz::back(vec) == 3);
    }

    SECTION("With one element") {
        std::vector<int> vec = { 1 };
        REQUIRE(lz::back(vec) == 1);
    }

    SECTION("With sentinel") {
        auto repeater = lz::repeat(20, 5);
        REQUIRE(lz::back(repeater) == 20);
    }
}

TEST_CASE("Front or") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::front_or(iterable, 'a') == 'H');
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::front_or(iterable, 'a') == 'H');
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::front_or(iterable, 'a') == 'a');
    }
}

TEST_CASE("Back or") {
    SECTION("With non-empty") {
        std::vector<int> vec = { 1, 2, 3 };
        REQUIRE(lz::back_or(vec, 0) == 3);
    }

    SECTION("With one element") {
        std::vector<int> vec = { 1 };
        REQUIRE(lz::back_or(vec, 0) == 1);
    }

    SECTION("With empty") {
        std::vector<int> vec = {};
        REQUIRE(lz::back_or(vec, 0) == 0);
    }
}

TEST_CASE("Distance") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::distance(iterable.begin(), iterable.end()) == 5);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::distance(iterable.begin(), iterable.end()) == 1);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::distance(iterable.begin(), iterable.end()) == 0);
    }
}

TEST_CASE("Accumulate") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::accumulate(iterable, 0, std::plus<int>()) == 500);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::accumulate(iterable, 0, std::plus<int>()) == 72);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::accumulate(iterable, 0, std::plus<int>()) == 0);
    }
}

TEST_CASE("Max element") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(*lz::max_element(iterable) == 'o');
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        REQUIRE(*lz::max_element(iterable) == 'H');
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::max_element(iterable) == iterable.end());
    }
}

TEST_CASE("Min element") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(*lz::min_element(iterable) == 'H');
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        REQUIRE(*lz::min_element(iterable) == 'H');
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::min_element(iterable) == iterable.end());
    }
}

TEST_CASE("Find if") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_if(iterable, [](char c) { return c == 'l'; });
        REQUIRE(lz::distance(iterable.begin(), pos) == 2);
        REQUIRE(*pos == 'l');
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_if(iterable, [](char c) { return c == 'H'; });
        REQUIRE(lz::distance(iterable.begin(), pos) == 0);
        REQUIRE(*pos == 'H');
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_if(iterable, [](char c) { return c == 'H'; });
        REQUIRE(pos == iterable.end());
        REQUIRE(lz::distance(iterable.begin(), pos) == 0);
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_if(iterable, [](char c) { return c == 'a'; });
        REQUIRE(pos == iterable.end());
        REQUIRE(lz::distance(iterable.begin(), pos) == 5);
    }
}

TEST_CASE("Find") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        auto pos = lz::find(iterable, 'l');
        REQUIRE(lz::distance(iterable.begin(), pos) == 2);
        REQUIRE(*pos == 'l');
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        auto pos = lz::find(iterable, 'H');
        REQUIRE(lz::distance(iterable.begin(), pos) == 0);
        REQUIRE(*pos == 'H');
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        auto pos = lz::find(iterable, 'H');
        REQUIRE(pos == iterable.end());
        REQUIRE(lz::distance(iterable.begin(), pos) == 0);
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        auto pos = lz::find(iterable, 'a');
        REQUIRE(pos == iterable.end());
        REQUIRE(lz::distance(iterable.begin(), pos) == 5);
    }
}

TEST_CASE("Find last") {
    SECTION("With non-empty c-string") {
        const char* str = "Helloo";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_last(iterable, 'o');
        REQUIRE(*pos == 'o');
        REQUIRE(lz::distance(iterable.begin(), pos) == 5);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_last(iterable, 'H');
        REQUIRE(*pos == 'H');
        REQUIRE(lz::distance(iterable.begin(), pos) == 0);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_last(iterable, 'H');
        REQUIRE(pos == iterable.end());
        REQUIRE(lz::distance(iterable.begin(), pos) == 0);
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_last(iterable, 'a');
        REQUIRE(pos == iterable.end());
        REQUIRE(lz::distance(iterable.begin(), pos) == 5);
    }
}

TEST_CASE("Find last random access") {
    SECTION("With non-empty") {
        std::vector<int> vec = { 1, 2, 3, 4, 5, 5 };
        auto pos = lz::find_last_if(vec, [](int i) { return i == 5; });
        REQUIRE(*pos == 5);
        REQUIRE(lz::distance(vec.begin(), pos) == 5);
    }

    SECTION("With one element") {
        std::vector<int> vec = { 1 };
        auto pos = lz::find_last_if(vec, [](int i) { return i == 1; });
        REQUIRE(*pos == 1);
        REQUIRE(lz::distance(vec.begin(), pos) == 0);
    }

    SECTION("With empty ") {
        std::vector<int> vec;
        auto pos = lz::find_last_if(vec, [](int i) { return i == 1; });
        REQUIRE(pos == vec.end());
        REQUIRE(lz::distance(vec.begin(), pos) == 0);
    }

    SECTION("Not found") {
        std::vector<int> vec = { 1, 2, 3, 4, 5 };
        auto pos = lz::find_last_if(vec, [](int i) { return i == 6; });
        REQUIRE(pos == vec.end());
        REQUIRE(lz::distance(vec.begin(), pos) == 5);
    }
}

TEST_CASE("Find last if") {
    SECTION("With non-empty c-string") {
        const char* str = "Helloo";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_last_if(iterable, [](char c) { return c == 'o'; });
        REQUIRE(*pos == 'o');
        REQUIRE(lz::distance(iterable.begin(), pos) == 5);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_last_if(iterable, [](char c) { return c == 'H'; });
        REQUIRE(*pos == 'H');
        REQUIRE(lz::distance(iterable.begin(), pos) == 0);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_last_if(iterable, [](char c) { return c == 'H'; });
        REQUIRE(pos == iterable.end());
        REQUIRE(lz::distance(iterable.begin(), pos) == 0);
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_last_if(iterable, [](char c) { return c == 'a'; });
        REQUIRE(pos == iterable.end());
        REQUIRE(lz::distance(iterable.begin(), pos) == 5);
    }
}

TEST_CASE("Find last if random access") {
    SECTION("With non-empty ") {
        std::vector<int> vec = { 1, 2, 3, 4, 5, 5 };
        auto pos = lz::find_last_if(vec, [](int i) { return i == 5; });
        REQUIRE(*pos == 5);
        REQUIRE(lz::distance(vec.begin(), pos) == 5);
    }

    SECTION("With one element") {
        std::vector<int> vec = { 1 };
        auto pos = lz::find_last_if(vec, [](int i) { return i == 1; });
        REQUIRE(*pos == 1);
        REQUIRE(lz::distance(vec.begin(), pos) == 0);
    }

    SECTION("With empty") {
        std::vector<int> vec;
        auto pos = lz::find_last_if(vec, [](int i) { return i == 1; });
        REQUIRE(pos == vec.end());
        REQUIRE(lz::distance(vec.begin(), pos) == 0);
    }

    SECTION("Not found") {
        std::vector<int> vec = { 1, 2, 3, 4, 5 };
        auto pos = lz::find_last_if(vec, [](int i) { return i == 6; });
        REQUIRE(pos == vec.end());
        REQUIRE(lz::distance(vec.begin(), pos) == 5);
    }
}

TEST_CASE("Search") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        const char* search = "lo";
        auto searchIterable = lz::c_string(search);
        auto it = lz::search(iterable, searchIterable);
        REQUIRE(*it.first == 'l');
        REQUIRE(it.second == iterable.end());
        REQUIRE(lz::distance(iterable.begin(), it.first) == 3);
        REQUIRE(lz::distance(iterable.begin(), it.second) == 5);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        const char* search = "H";
        auto searchIterable = lz::c_string(search);
        auto it = lz::search(iterable, searchIterable);
        REQUIRE(*it.first == 'H');
        REQUIRE(it.second == iterable.end());
        REQUIRE(lz::distance(iterable.begin(), it.first) == 0);
        REQUIRE(lz::distance(iterable.begin(), it.second) == 1);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        const char* search = "H";
        auto searchIterable = lz::c_string(search);
        auto it = lz::search(iterable, searchIterable);
        REQUIRE(it.first == iterable.end());
        REQUIRE(it.second == iterable.end());
        REQUIRE(lz::distance(iterable.begin(), it.first) == 0);
        REQUIRE(lz::distance(iterable.begin(), it.second) == 0);
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        const char* search = "a";
        auto searchIterable = lz::c_string(search);
        auto it = lz::search(iterable, searchIterable);
        REQUIRE(it.first == iterable.end());
        REQUIRE(it.second == iterable.end());
        REQUIRE(lz::distance(iterable.begin(), it.first) == 5);
        REQUIRE(lz::distance(iterable.begin(), it.second) == 5);
    }
}

TEST_CASE("Search random access") {
    SECTION("With non-empty") {
        std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8 };
        std::vector<int> search = { 5, 6 };
        auto it = lz::search(vec, search);
        REQUIRE(*it.first == 5);
        REQUIRE(*it.second == 7);
        REQUIRE(lz::distance(vec.begin(), it.first) == 4);
        REQUIRE(lz::distance(vec.begin(), it.second) == 6);
        REQUIRE(std::distance(it.first, it.second) == 2);
    }

    SECTION("With one element") {
        std::vector<int> vec = { 1 };
        std::vector<int> search = { 1 };
        auto it = lz::search(vec, search);
        REQUIRE(*it.first == 1);
        REQUIRE(it.second == vec.end());
        REQUIRE(lz::distance(vec.begin(), it.first) == 0);
        REQUIRE(lz::distance(vec.begin(), it.second) == 1);
    }

    SECTION("With empty 1") {
        std::vector<int> vec = {};
        std::vector<int> search = { 1, 2, 3, 4, 5 };
        auto it = lz::search(vec, search);
        REQUIRE(it.first == vec.end());
        REQUIRE(it.second == vec.end());
        REQUIRE(lz::distance(vec.begin(), it.first) == 0);
        REQUIRE(lz::distance(vec.begin(), it.second) == 0);
    }

    SECTION("With empty 2") {
        std::vector<int> vec = { 1, 2, 3, 4, 5 };
        std::vector<int> search = {};
        auto it = lz::search(vec, search);
        REQUIRE(it.first == vec.begin());
        REQUIRE(it.second == vec.begin());
        REQUIRE(lz::distance(vec.begin(), it.first) == 0);
        REQUIRE(lz::distance(vec.begin(), it.second) == 0);
    }

    SECTION("With empty 3") {
        std::vector<int> vec = {};
        std::vector<int> search = {};
        auto it = lz::search(vec, search);
        REQUIRE(it.first == vec.end());
        REQUIRE(it.second == vec.end());
        REQUIRE(lz::distance(vec.begin(), it.first) == 0);
        REQUIRE(lz::distance(vec.begin(), it.second) == 0);
    }

    SECTION("Not found") {
        std::vector<int> vec = { 1, 2, 3, 4, 5 };
        std::vector<int> search = { 5, 6 };
        auto it = lz::search(vec, search);
        REQUIRE(it.first == vec.end());
        REQUIRE(it.second == vec.end());
        REQUIRE(lz::distance(vec.begin(), it.first) == 5);
        REQUIRE(lz::distance(vec.begin(), it.second) == 5);
    }
}

TEST_CASE("Find if not") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_if_not(iterable, [](char c) { return c == 'H'; });
        REQUIRE(*pos == 'e');
        REQUIRE(lz::distance(iterable.begin(), pos) == 1);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_if_not(iterable, [](char c) { return c == 'H'; });
        REQUIRE(pos == iterable.end());
        REQUIRE(lz::distance(iterable.begin(), pos) == 1);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_if_not(iterable, [](char c) { return c == 'H'; });
        REQUIRE(pos == iterable.end());
        REQUIRE(lz::distance(iterable.begin(), pos) == 0);
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_if_not(iterable, [](char c) { return lz::contains(lz::c_string("Hello"), c); });
        REQUIRE(pos == iterable.end());
        REQUIRE(lz::distance(iterable.begin(), pos) == 5);
    }
}

TEST_CASE("Find last if not") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_last_if_not(iterable, [](char c) { return c == 'o'; });
        REQUIRE(*pos == 'l');
        REQUIRE(lz::distance(iterable.begin(), pos) == 3);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_last_if_not(iterable, [](char c) { return c == 'H'; });
        REQUIRE(pos == iterable.end());
        REQUIRE(lz::distance(iterable.begin(), pos) == 1);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_last_if_not(iterable, [](char c) { return c == 'H'; });
        REQUIRE(pos == iterable.end());
        REQUIRE(lz::distance(iterable.begin(), pos) == 0);
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_last_if_not(iterable, [](char c) { return lz::contains(lz::c_string("Hello"), c); });
        REQUIRE(pos == iterable.end());
        REQUIRE(lz::distance(iterable.begin(), pos) == 5);
    }
}

TEST_CASE("Find last if not random access") {
    SECTION("With non-empty") {
        std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8 };
        auto pos = lz::find_last_if_not(vec, [](int i) { return i == 5; });
        REQUIRE(*pos == 8);
        REQUIRE(lz::distance(vec.begin(), pos) == 7);
    }

    SECTION("With one element") {
        std::vector<int> vec = { 1 };
        auto pos = lz::find_last_if_not(vec, [](int i) { return i != 1; });
        REQUIRE(*pos == *vec.begin());
        REQUIRE(lz::distance(vec.begin(), pos) == 0);
    }

    SECTION("With empty") {
        std::vector<int> vec = {};
        auto pos = lz::find_last_if_not(vec, [](int i) { return i == 1; });
        REQUIRE(pos == vec.end());
        REQUIRE(lz::distance(vec.begin(), pos) == 0);
    }

    SECTION("Not found") {
        std::vector<int> vec = { 5, 5, 5 };
        auto pos = lz::find_last_if_not(vec, [](int i) { return i == 5; });
        REQUIRE(pos == vec.end());
        REQUIRE(lz::distance(vec.begin(), pos) == 3);
    }
}

TEST_CASE("Find or default") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::find_or_default(iterable, 'o', 'a') == 'o');
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::find_or_default(iterable, 'H', 'a') == 'H');
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::find_or_default(iterable, 'H') == char{});
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::find_or_default(iterable, 'a', 'b') == 'b');
    }
}

TEST_CASE("Find or default if") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::find_or_default_if(iterable, [](char c) { return c == 'o'; }, 'a') == 'o');
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::find_or_default_if(iterable, [](char c) { return c == 'H'; }, 'a') == 'H');
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::find_or_default_if(iterable, [](char c) { return c == 'H'; }) == char{});
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::find_or_default_if(iterable, [](char c) { return c == 'a'; }, 'b') == 'b');
    }
}

TEST_CASE("Find last or default") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::find_last_or_default(iterable, 'o', 'a') == 'o');
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::find_last_or_default(iterable, 'H', 'a') == 'H');
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::find_last_or_default(iterable, 'H') == char{});
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::find_last_or_default(iterable, 'a', 'b') == 'b');
    }
}

TEST_CASE("Find last or default random access") {
    SECTION("With non-empty") {
        std::vector<int> vec = { 1, 2, 3, 4, 5, 6 };
        REQUIRE(lz::find_last_or_default(vec, 5, 0) == 5);
    }

    SECTION("With one element") {
        std::vector<int> vec = { 1 };
        REQUIRE(lz::find_last_or_default(vec, 1, 0) == 1);
    }

    SECTION("With empty") {
        std::vector<int> vec = {};
        REQUIRE(lz::find_last_or_default(vec, 1) == 0);
    }

    SECTION("Not found") {
        std::vector<int> vec = { 1, 2, 3, 4 };
        REQUIRE(lz::find_last_or_default(vec, 5, 0) == 0);
    }
}

TEST_CASE("Find last or default if") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::find_last_or_default_if(iterable, [](char c) { return c == 'o'; }, 'a') == 'o');
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::find_last_or_default_if(iterable, [](char c) { return c == 'H'; }, 'a') == 'H');
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::find_last_or_default_if(iterable, [](char c) { return c == 'H'; }) == char{});
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::find_last_or_default_if(iterable, [](char c) { return c == 'a'; }, 'b') == 'b');
    }
}

TEST_CASE("Find last or default if random access") {
    SECTION("With non-empty") {
        std::vector<int> vec = { 1, 2, 3, 4, 5, 6 };
        REQUIRE(lz::find_last_or_default_if(vec, [](int i) { return i == 5; }, 0) == 5);
    }

    SECTION("With one element") {
        std::vector<int> vec = { 1 };
        REQUIRE(lz::find_last_or_default_if(vec, [](int i) { return i == 1; }, 0) == 1);
    }

    SECTION("With empty") {
        std::vector<int> vec = {};
        REQUIRE(lz::find_last_or_default_if(vec, [](int i) { return i == 1; }) == 0);
    }

    SECTION("Not found") {
        std::vector<int> vec = { 5, 5, 5 };
        REQUIRE(lz::find_last_or_default_if(vec, [](int i) { return i == 6; }, 0) == 0);
    }
}

TEST_CASE("Find last or default if not random access") {
    SECTION("With non-empty") {
        std::vector<int> vec = { 1, 2, 3, 4, 5, 6 };
        REQUIRE(lz::find_last_or_default_if_not(vec, [](int i) { return i == 6; }, 0) == 5);
    }

    SECTION("With one element") {
        std::vector<int> vec = { 1 };
        REQUIRE(lz::find_last_or_default_if_not(vec, [](int i) { return i == 1; }, 0) == 0);
    }

    SECTION("With empty") {
        std::vector<int> vec = {};
        REQUIRE(lz::find_last_or_default_if_not(vec, [](int i) { return i == 1; }) == 0);
    }

    SECTION("Not found") {
        std::vector<int> vec = { 5, 5, 5 };
        REQUIRE(lz::find_last_or_default_if_not(vec, [](int i) { return i == 5; }, 0) == 0);
    }
}

TEST_CASE("Find last or default if not") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::find_last_or_default_if_not(iterable, [](char c) { return c == 'o'; }, 'a') == 'l');
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::find_last_or_default_if_not(iterable, [](char c) { return c == 'H'; }, 'a') == 'a');
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::find_last_or_default_if_not(iterable, [](char c) { return c == 'H'; }) == char{});
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::find_last_or_default_if_not(iterable, [](char c) { return c == 'a'; }, 'b') == 'o');
    }
}

TEST_CASE("Find last or default not") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::find_last_or_default_not(iterable, 'o', 'a') == 'l');
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::find_last_or_default_not(iterable, 'H', 'a') == 'a');
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::find_last_or_default_not(iterable, 'H') == char{});
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::find_last_or_default_not(iterable, 'a', 'b') == 'o');
    }
}

TEST_CASE("Find last or default not random access") {
    SECTION("With non-empty") {
        std::vector<int> vec = { 1, 2, 3, 4, 5, 6 };
        REQUIRE(lz::find_last_or_default_not(vec, 5, 0) == 6);
    }

    SECTION("With one element") {
        std::vector<int> vec = { 1 };
        REQUIRE(lz::find_last_or_default_not(vec, 1, 0) == 0);
    }

    SECTION("With empty") {
        std::vector<int> vec = {};
        REQUIRE(lz::find_last_or_default_not(vec, 1) == 0);
    }

    SECTION("Not found") {
        std::vector<int> vec = { 5, 5, 5 };
        REQUIRE(lz::find_last_or_default_not(vec, 5, 0) == 0);
    }
}

TEST_CASE("Index of") {
    SECTION("With non-empty c-string") {
        const char* str = "Helloo";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::index_of(iterable, 'o') == 4);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::index_of(iterable, 'H') == 0);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::index_of(iterable, 'H') == lz::npos);
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::index_of(iterable, 'a') == lz::npos);
    }
}

TEST_CASE("Index of if") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::index_of_if(iterable, [](char c) { return c == 'o'; }) == 4);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::index_of_if(iterable, [](char c) { return c == 'H'; }) == 0);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::index_of_if(iterable, [](char c) { return c == 'H'; }) == lz::npos);
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::index_of_if(iterable, [](char c) { return c == 'a'; }) == lz::npos);
    }
}

TEST_CASE("Contains") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::contains(iterable, 'o'));
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::contains(iterable, 'H'));
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        REQUIRE_FALSE(lz::contains(iterable, 'H'));
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE_FALSE(lz::contains(iterable, 'a'));
    }
}

TEST_CASE("Starts with") {
    SECTION("With non-empty c-string") {
        auto iterable = lz::c_string("Hello");
        auto iterable2 = lz::c_string("He");
        REQUIRE(lz::starts_with(iterable, iterable2));
        iterable2 = lz::c_string("H");
        REQUIRE(lz::starts_with(iterable, iterable2));
        iterable2 = lz::c_string("Hello");
        REQUIRE(lz::starts_with(iterable, iterable2));
    }

    SECTION("With one element") {
        auto iterable = lz::c_string("H");
        auto iterable2 = lz::c_string("H");
        REQUIRE(lz::starts_with(iterable, iterable2));
    }

    SECTION("With empty c-string") {
        auto iterable = lz::c_string("");
        auto iterable2 = lz::c_string("H");
        REQUIRE_FALSE(lz::starts_with(iterable, iterable2));

        iterable = lz::c_string("H");
        iterable2 = lz::c_string("");
        REQUIRE_FALSE(lz::starts_with(iterable, iterable2));
    }

    SECTION("Not found c-string") {
        auto iterable = lz::c_string("Hello");
        auto iterable2 = lz::c_string("a");
        REQUIRE_FALSE(lz::starts_with(iterable, iterable2));
    }

    SECTION("With non-empty string") {
        auto iterable = std::string("Hello");
        auto iterable2 = std::string("He");
        REQUIRE(lz::starts_with(iterable, iterable2));
    }

    SECTION("With one element string") {
        auto iterable = std::string("H");
        auto iterable2 = std::string("H");
        REQUIRE(lz::starts_with(iterable, iterable2));
    }

    SECTION("With empty string") {
        auto iterable = std::string("");
        auto iterable2 = std::string("H");
        REQUIRE_FALSE(lz::starts_with(iterable, iterable2));

        iterable = std::string("H");
        iterable2 = std::string("");
        REQUIRE_FALSE(lz::starts_with(iterable, iterable2));
    }

    SECTION("Not found string") {
        auto iterable = std::string("Hello");
        auto iterable2 = std::string("a");
        REQUIRE_FALSE(lz::starts_with(iterable, iterable2));
    }
}

TEST_CASE("Ends with") {
    SECTION("Forward non sized") {
        auto iterable = lz::c_string("");
        auto iterable2 = lz::c_string("H");
        REQUIRE_FALSE(lz::ends_with(iterable, iterable2));

        iterable2 = lz::c_string("");
        REQUIRE(lz::ends_with(iterable, iterable2));

        iterable = lz::c_string("Hello");
        iterable2 = lz::c_string("o");
        REQUIRE(lz::ends_with(iterable, iterable2));
        iterable2 = lz::c_string("lo");
        REQUIRE(lz::ends_with(iterable, iterable2));

        iterable2 = lz::c_string("H");
        REQUIRE_FALSE(lz::ends_with(iterable, iterable2));
    }

    SECTION("Bidirectional sized") {
        std::list<char> lst;
        std::list<char> lst2;
        REQUIRE(lz::ends_with(lst, lst2));
        lst = { 'H' };
        REQUIRE_FALSE(lz::ends_with(lst, lst2));
        lst = {};
        lst2 = { 'H' };
        REQUIRE_FALSE(lz::ends_with(lst, lst2));
        lst = { 'A', 'B' };
        lst2 = { 'B' };
        REQUIRE(lz::ends_with(lst, lst2));
    }
}

TEST_CASE("Partition") {
    SECTION("With non-empty c-string") {
        char str[] = "6789012345";
        auto iterable = lz::c_string(str);
        auto partitioned = lz::partition(iterable, [](char c) { return c % 2 == 0; });

        lz::basic_iterable<decltype(iterable.begin())> part1(iterable.begin(), partitioned);
        lz::basic_iterable<decltype(iterable.begin()), decltype(iterable.end())> part2(partitioned, iterable.end());

        REQUIRE(lz::all_of(part1, [](char c) { return c % 2 == 0; }));
        REQUIRE(lz::none_of(part2, [](char c) { return c % 2 == 0; }));
    }

    SECTION("With one element") {
        char str[] = "6";
        auto iterable = str | lz::c_string;
        auto partitioned = lz::partition(iterable, [](char c) { return c % 2 == 0; });

        lz::basic_iterable<decltype(iterable.begin())> part1(iterable.begin(), partitioned);
        lz::basic_iterable<decltype(iterable.begin()), decltype(iterable.end())> part2(partitioned, iterable.end());

        REQUIRE(lz::all_of(part1, [](char c) { return c % 2 == 0; }));
        REQUIRE(lz::none_of(part2, [](char c) { return c % 2 == 0; }));
    }

    SECTION("With empty c-string") {
        char str[] = "";
        auto iterable = lz::c_string(str);
        auto partitioned = lz::partition(iterable, [](char c) { return c % 2 == 0; });

        lz::basic_iterable<decltype(iterable.begin())> part1(iterable.begin(), partitioned);
        lz::basic_iterable<decltype(iterable.begin()), decltype(iterable.end())> part2(partitioned, iterable.end());

        REQUIRE(lz::all_of(part1, [](char c) { return c % 2 == 0; }));
        REQUIRE(lz::none_of(part2, [](char c) { return c % 2 == 0; }));
    }

    SECTION("With all even c-string") {
        char str[] = "2468";
        auto iterable = lz::c_string(str);
        auto partitioned = lz::partition(iterable, [](char c) { return c % 2 == 0; });

        lz::basic_iterable<decltype(iterable.begin())> part1(iterable.begin(), partitioned);
        lz::basic_iterable<decltype(iterable.begin()), decltype(iterable.end())> part2(partitioned, iterable.end());

        REQUIRE(lz::all_of(part1, [](char c) { return c % 2 == 0; }));
        REQUIRE(lz::none_of(part2, [](char c) { return c % 2 == 0; }));
    }

    SECTION("With all odd c-string") {
        char str[] = "13579";
        auto iterable = lz::c_string(str);
        auto partitioned = lz::partition(iterable, [](char c) { return c % 2 == 0; });

        lz::basic_iterable<decltype(iterable.begin())> part1(iterable.begin(), partitioned);
        lz::basic_iterable<decltype(iterable.begin()), decltype(iterable.end())> part2(partitioned, iterable.end());

        REQUIRE(lz::all_of(part2, [](char c) { return c % 2 != 0; }));
        REQUIRE(lz::none_of(part1, [](char c) { return c % 2 != 0; }));
    }
}

TEST_CASE("Mean") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::mean(iterable, std::plus<int>()) == Catch::Approx(('H' + 'e' + 'l' + 'l' + 'o') / 5.0));
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::mean(iterable, std::plus<int>()) == Catch::Approx(static_cast<double>('H')));
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::mean(iterable, std::plus<int>()) == Catch::Approx(0.0));
    }
}

TEST_CASE("For each") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        std::string result;
        lz::for_each(iterable, [&result](char c) { result += c; });
        REQUIRE(result == "Hello");
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        std::string result;
        lz::for_each(iterable, [&result](char c) { result += c; });
        REQUIRE(result == "H");
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        std::string result;
        lz::for_each(iterable, [&result](char c) { result += c; });
        REQUIRE(result.empty());
    }
}

TEST_CASE("For each while") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        std::string result;
        lz::for_each_while(iterable, [&result](char c) {
            result += c;
            return c != 'l';
        });
        REQUIRE(result == "Hel");
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        std::string result;
        lz::for_each_while(iterable, [&result](char c) {
            result += c;
            return c != 'H';
        });
        REQUIRE(result == "H");
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        std::string result;
        lz::for_each_while(iterable, [&result](char c) {
            if (c == 'H') {
                return false;
            }
            result += c;
            return true;
        });
        REQUIRE(result.empty());
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        std::string result;
        lz::for_each_while(iterable, [&result](char c) {
            result += c;
            return c != 'H';
        });
        REQUIRE(result.empty());
    }
}

TEST_CASE("For each while n") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        std::string result;
        lz::for_each_while_n(iterable, 5, [&result](char c) {
            result += c;
            return c != 'l';
        });
        REQUIRE(result == "Hel");

        result = "";
        lz::for_each_while_n(iterable, 3, [&result](char c) {
            result += c;
            return c != 'o';
        });
        REQUIRE(result == "Hel");

        result = "";
        lz::for_each_while_n(iterable, 0, [&result](char c) {
            result += c;
            return c != 'o';
        });
        REQUIRE(result.empty());
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        std::string result;
        lz::for_each_while_n(iterable, 1, [&result](char c) {
            result += c;
            return c != 'H';
        });
        REQUIRE(result == "H");

        result = "";
        lz::for_each_while_n(iterable, 0, [&result](char c) {
            result += c;
            return c != 'H';
        });
        REQUIRE(result.empty());

        result = "";
        lz::for_each_while_n(iterable, 10, [&result](char c) {
            result += c;
            return c != 'H';
        });
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        std::string result;
        lz::for_each_while_n(iterable, 0, [&result](char c) {
            result += c;
            return c != 'H';
        });
        REQUIRE(result.empty());

        result = "";
        lz::for_each_while_n(iterable, 10, [&result](char c) {
            result += c;
            return c != 'H';
        });
        REQUIRE(result.empty());
    }
}

TEST_CASE("Copy") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        std::string result;
        lz::copy(iterable, std::back_inserter(result));
        REQUIRE(result == "Hello");
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        std::string result;
        lz::copy(iterable, std::back_inserter(result));
        REQUIRE(result == "H");
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        std::string result;
        lz::copy(iterable, std::back_inserter(result));
        REQUIRE(result.empty());
    }
}

TEST_CASE("Transform") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        std::string result;
        lz::transform(iterable, std::back_inserter(result), [](char c) { return static_cast<char>(c + 1); });
        REQUIRE(result == "Ifmmp");
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        std::string result;
        lz::transform(iterable, std::back_inserter(result), [](char c) { return static_cast<char>(c + 1); });
        REQUIRE(result == "I");
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        std::string result;
        lz::transform(iterable, std::back_inserter(result), [](char c) { return static_cast<char>(c + 1); });
        REQUIRE(result.empty());
    }
}

TEST_CASE("Equal") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        const char* str2 = "Hello";
        auto iterable2 = lz::c_string(str2);
        REQUIRE(lz::equal(iterable, iterable2));
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        const char* str2 = "H";
        auto iterable2 = lz::c_string(str2);
        REQUIRE(lz::equal(iterable, iterable2));
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        const char* str2 = "";
        auto iterable2 = lz::c_string(str2);
        REQUIRE(lz::equal(iterable, iterable2));
    }

    SECTION("Not equal c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        const char* str2 = "Helloo";
        auto iterable2 = lz::c_string(str2);
        REQUIRE_FALSE(lz::equal(iterable, iterable2));
    }
}

TEST_CASE("Lower bound") {
    SECTION("With non-empty c-string") {
        const char* str = "aaabcccdeee";
        auto iterable = lz::c_string(str);
        auto it = lz::lower_bound(iterable, 'c');
        REQUIRE(*it == 'c');
        REQUIRE(lz::distance(iterable.begin(), it) == 4);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        auto it = lz::lower_bound(iterable, 'H');
        REQUIRE(*it == 'H');
        REQUIRE(lz::distance(iterable.begin(), it) == 0);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        auto it = lz::lower_bound(iterable, 'H');
        REQUIRE(it == iterable.end());
        REQUIRE(lz::distance(iterable.begin(), it) == 0);
    }

    SECTION("Not found c-string") {
        const char* str = "aaabcccdeee";
        auto iterable = lz::c_string(str);
        auto it = lz::lower_bound(iterable, 'f');
        REQUIRE(it == iterable.end());
        REQUIRE(lz::distance(iterable.begin(), it) == 11);
    }
}

TEST_CASE("Upper bound") {
    SECTION("With non-empty c-string") {
        const char* str = "aaabcccdeee";
        auto iterable = lz::c_string(str);
        auto it = lz::upper_bound(iterable, 'c');
        REQUIRE(*it == 'd');
        REQUIRE(lz::distance(iterable.begin(), it) == 7);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        auto it = lz::upper_bound(iterable, 'H');
        REQUIRE(it == iterable.end());
        REQUIRE(lz::distance(iterable.begin(), it) == 1);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        auto it = lz::upper_bound(iterable, 'H');
        REQUIRE(it == iterable.end());
        REQUIRE(lz::distance(iterable.begin(), it) == 0);
    }

    SECTION("Not found c-string") {
        const char* str = "aaabcccdeee";
        auto iterable = lz::c_string(str);
        auto it = lz::upper_bound(iterable, 'f');
        REQUIRE(it == iterable.end());
        REQUIRE(lz::distance(iterable.begin(), it) == 11);
    }
}

TEST_CASE("Binary search") {
    SECTION("With non-empty c-string") {
        const char* str = "aaabcccdeee";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::binary_search(iterable, 'c'));
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::binary_search(iterable, 'H'));
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        REQUIRE_FALSE(lz::binary_search(iterable, 'H'));
    }

    SECTION("Not found c-string") {
        const char* str = "aaabcccdeee";
        auto iterable = lz::c_string(str);
        REQUIRE_FALSE(lz::binary_search(iterable, 'f'));
    }
}

TEST_CASE("All of") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::all_of(iterable, [](char c) { return c != 'a'; }));
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::all_of(iterable, [](char c) { return c == 'H'; }));
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::all_of(iterable, [](char c) { return c == 'H'; }));
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE_FALSE(lz::all_of(iterable, [](char c) { return c == 'a'; }));
    }
}

TEST_CASE("Any of") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::any_of(iterable, [](char c) { return c == 'o'; }));
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::any_of(iterable, [](char c) { return c == 'H'; }));
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        REQUIRE_FALSE(lz::any_of(iterable, [](char c) { return c == 'H'; }));
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE_FALSE(lz::any_of(iterable, [](char c) { return c == 'a'; }));
    }
}

TEST_CASE("None of") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::none_of(iterable, [](char c) { return c == 'a'; }));
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::none_of(iterable, [](char c) { return c == 'a'; }));
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::none_of(iterable, [](char c) { return c == 'a'; }));
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE_FALSE(lz::none_of(iterable, [](char c) { return c == 'o'; }));
    }
}

TEST_CASE("Adjacent find") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        auto it = lz::adjacent_find(iterable);
        REQUIRE(*it == 'l');
        REQUIRE(lz::distance(iterable.begin(), it) == 2);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        auto it = lz::adjacent_find(iterable);
        REQUIRE(it == iterable.end());
        REQUIRE(lz::distance(iterable.begin(), it) == 1);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        auto it = lz::adjacent_find(iterable);
        REQUIRE(it == iterable.end());
        REQUIRE(lz::distance(iterable.begin(), it) == 0);
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        auto it = lz::adjacent_find(iterable, [](char a, char b) { return a == 'a' && b == 'a'; });
        REQUIRE(it == iterable.end());
        REQUIRE(lz::distance(iterable.begin(), it) == 5);
    }
}

TEST_CASE("Count") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::count(iterable, 'l') == 2);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::count(iterable, 'H') == 1);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::count(iterable, 'H') == 0);
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::count(iterable, 'a') == 0);
    }
}

TEST_CASE("Count if") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::count_if(iterable, [](char c) { return c == 'l'; }) == 2);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::count_if(iterable, [](char c) { return c == 'H'; }) == 1);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::count_if(iterable, [](char c) { return c == 'H'; }) == 0);
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::count_if(iterable, [](char c) { return c == 'a'; }) == 0);
    }
}

TEST_CASE("Is sorted") {
    SECTION("With non-empty c-string") {
        const char* str = "abcde";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::is_sorted(iterable));
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::is_sorted(iterable));
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        REQUIRE(lz::is_sorted(iterable));
    }

    SECTION("Not sorted c-string") {
        const char* str = "abcdea";
        auto iterable = lz::c_string(str);
        REQUIRE_FALSE(lz::is_sorted(iterable));
    }
}
