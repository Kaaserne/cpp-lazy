#include <Lz/algorithm.hpp>
#include <Lz/c_string.hpp>
#include <Lz/iterable.hpp>
#include <catch2/catch.hpp>

TEST_CASE("Empty") {
    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        CHECK(lz::empty(iterable));
    }

    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(!lz::empty(iterable));
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        CHECK(!lz::empty(iterable));
    }
}

TEST_CASE("Has one") {
    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        CHECK(!lz::has_one(iterable));
    }

    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(!lz::has_one(iterable));
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        CHECK(lz::has_one(iterable));
    }
}

TEST_CASE("Has many") {
    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        CHECK(!lz::has_many(iterable));
    }

    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(lz::has_many(iterable));
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        CHECK(!lz::has_many(iterable));
    }
}

TEST_CASE("Front") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(lz::front(iterable) == 'H');
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        CHECK(lz::front(iterable) == 'H');
    }
}

TEST_CASE("Back") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str, str + std::strlen(str));
        CHECK(lz::back(iterable) == 'o');
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str, str + std::strlen(str));
        CHECK(lz::back(iterable) == 'H');
    }
}

TEST_CASE("Front or") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(lz::front_or(iterable, 'a') == 'H');
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        CHECK(lz::front_or(iterable, 'a') == 'H');
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        CHECK(lz::front_or(iterable, 'a') == 'a');
    }
}

TEST_CASE("Back or") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str, str + std::strlen(str));
        CHECK(lz::back_or(iterable, 'a') == 'o');
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str, str + std::strlen(str));
        CHECK(lz::back_or(iterable, 'a') == 'H');
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str, str + std::strlen(str));
        CHECK(lz::back_or(iterable, 'a') == 'a');
    }
}

TEST_CASE("Distance") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(lz::distance(iterable.begin(), iterable.end()) == 5);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        CHECK(lz::distance(iterable.begin(), iterable.end()) == 1);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        CHECK(lz::distance(iterable.begin(), iterable.end()) == 0);
    }
}

TEST_CASE("Accumulate") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(lz::accumulate(iterable, 0, std::plus<int>()) == 500);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        CHECK(lz::accumulate(iterable, 0, std::plus<int>()) == 72);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        CHECK(lz::accumulate(iterable, 0, std::plus<int>()) == 0);
    }
}

TEST_CASE("Max element") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(*lz::max_element(iterable) == 'o');
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        CHECK(*lz::max_element(iterable) == 'H');
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        CHECK(lz::max_element(iterable) == iterable.end());
    }
}

TEST_CASE("Min element") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(*lz::min_element(iterable) == 'H');
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        CHECK(*lz::min_element(iterable) == 'H');
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        CHECK(lz::min_element(iterable) == iterable.end());
    }
}

TEST_CASE("Find if") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_if(iterable, [](char c) { return c == 'l'; });
        CHECK(lz::distance(iterable.begin(), pos) == 2);
        CHECK(*pos == 'l');
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_if(iterable, [](char c) { return c == 'H'; });
        CHECK(lz::distance(iterable.begin(), pos) == 0);
        CHECK(*pos == 'H');
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_if(iterable, [](char c) { return c == 'H'; });
        CHECK(pos == iterable.end());
        CHECK(lz::distance(iterable.begin(), pos) == 0);
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_if(iterable, [](char c) { return c == 'a'; });
        CHECK(pos == iterable.end());
        CHECK(lz::distance(iterable.begin(), pos) == 5);
    }
}

TEST_CASE("Find") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        auto pos = lz::find(iterable, 'l');
        CHECK(lz::distance(iterable.begin(), pos) == 2);
        CHECK(*pos == 'l');
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        auto pos = lz::find(iterable, 'H');
        CHECK(lz::distance(iterable.begin(), pos) == 0);
        CHECK(*pos == 'H');
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        auto pos = lz::find(iterable, 'H');
        CHECK(pos == iterable.end());
        CHECK(lz::distance(iterable.begin(), pos) == 0);
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        auto pos = lz::find(iterable, 'a');
        CHECK(pos == iterable.end());
        CHECK(lz::distance(iterable.begin(), pos) == 5);
    }
}

TEST_CASE("Find last") {
    SECTION("With non-empty c-string") {
        const char* str = "Helloo";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_last(iterable, 'o');
        CHECK(*pos == 'o');
        CHECK(lz::distance(iterable.begin(), pos) == 5);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_last(iterable, 'H');
        CHECK(*pos == 'H');
        CHECK(lz::distance(iterable.begin(), pos) == 0);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_last(iterable, 'H');
        CHECK(pos == iterable.end());
        CHECK(lz::distance(iterable.begin(), pos) == 0);
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_last(iterable, 'a');
        CHECK(pos == iterable.end());
        CHECK(lz::distance(iterable.begin(), pos) == 5);
    }
}

TEST_CASE("Find last random access") {
    SECTION("With non-empty c-string") {
        const char* str = "Helloo";
        auto iterable = lz::c_string(str, str + std::strlen(str));
        auto pos = lz::find_last(iterable, 'o');
        CHECK(*pos == 'o');
        CHECK(lz::distance(iterable.begin(), pos) == 5);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str, str + std::strlen(str));
        auto pos = lz::find_last(iterable, 'H');
        CHECK(*pos == 'H');
        CHECK(lz::distance(iterable.begin(), pos) == 0);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str, str + std::strlen(str));
        auto pos = lz::find_last(iterable, 'H');
        CHECK(pos == iterable.end());
        CHECK(lz::distance(iterable.begin(), pos) == 0);
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str, str + std::strlen(str));
        auto pos = lz::find_last(iterable, 'a');
        CHECK(pos == iterable.end());
        CHECK(lz::distance(iterable.begin(), pos) == 5);
    }
}

TEST_CASE("Find last if") {
    SECTION("With non-empty c-string") {
        const char* str = "Helloo";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_last_if(iterable, [](char c) { return c == 'o'; });
        CHECK(*pos == 'o');
        CHECK(lz::distance(iterable.begin(), pos) == 5);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_last_if(iterable, [](char c) { return c == 'H'; });
        CHECK(*pos == 'H');
        CHECK(lz::distance(iterable.begin(), pos) == 0);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_last_if(iterable, [](char c) { return c == 'H'; });
        CHECK(pos == iterable.end());
        CHECK(lz::distance(iterable.begin(), pos) == 0);
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_last_if(iterable, [](char c) { return c == 'a'; });
        CHECK(pos == iterable.end());
        CHECK(lz::distance(iterable.begin(), pos) == 5);
    }
}

TEST_CASE("Find last if random access") {
    SECTION("With non-empty c-string") {
        const char* str = "Helloo";
        auto iterable = lz::c_string(str, str + std::strlen(str));
        auto pos = lz::find_last_if(iterable, [](char c) { return c == 'o'; });
        CHECK(*pos == 'o');
        CHECK(lz::distance(iterable.begin(), pos) == 5);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str, str + std::strlen(str));
        auto pos = lz::find_last_if(iterable, [](char c) { return c == 'H'; });
        CHECK(*pos == 'H');
        CHECK(lz::distance(iterable.begin(), pos) == 0);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str, str + std::strlen(str));
        auto pos = lz::find_last_if(iterable, [](char c) { return c == 'H'; });
        CHECK(pos == iterable.end());
        CHECK(lz::distance(iterable.begin(), pos) == 0);
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str, str + std::strlen(str));
        auto pos = lz::find_last_if(iterable, [](char c) { return c == 'a'; });
        CHECK(pos == iterable.end());
        CHECK(lz::distance(iterable.begin(), pos) == 5);
    }
}

TEST_CASE("Search") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        const char* search = "lo";
        auto searchIterable = lz::c_string(search);
        auto it = lz::search(iterable, searchIterable);
        CHECK(*it.first == 'l');
        CHECK(it.second == iterable.end());
        CHECK(lz::distance(iterable.begin(), it.first) == 3);
        CHECK(lz::distance(iterable.begin(), it.second) == 5);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        const char* search = "H";
        auto searchIterable = lz::c_string(search);
        auto it = lz::search(iterable, searchIterable);
        CHECK(*it.first == 'H');
        CHECK(it.second == iterable.end());
        CHECK(lz::distance(iterable.begin(), it.first) == 0);
        CHECK(lz::distance(iterable.begin(), it.second) == 1);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        const char* search = "H";
        auto searchIterable = lz::c_string(search);
        auto it = lz::search(iterable, searchIterable);
        CHECK(it.first == iterable.end());
        CHECK(it.second == iterable.end());
        CHECK(lz::distance(iterable.begin(), it.first) == 0);
        CHECK(lz::distance(iterable.begin(), it.second) == 0);
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        const char* search = "a";
        auto searchIterable = lz::c_string(search);
        auto it = lz::search(iterable, searchIterable);
        CHECK(it.first == iterable.end());
        CHECK(it.second == iterable.end());
        CHECK(lz::distance(iterable.begin(), it.first) == 5);
        CHECK(lz::distance(iterable.begin(), it.second) == 5);
    }
}

TEST_CASE("Find if not") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_if_not(iterable, [](char c) { return c == 'H'; });
        CHECK(*pos == 'e');
        CHECK(lz::distance(iterable.begin(), pos) == 1);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_if_not(iterable, [](char c) { return c == 'H'; });
        CHECK(pos == iterable.end());
        CHECK(lz::distance(iterable.begin(), pos) == 1);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_if_not(iterable, [](char c) { return c == 'H'; });
        CHECK(pos == iterable.end());
        CHECK(lz::distance(iterable.begin(), pos) == 0);
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_if_not(iterable, [](char c) { return lz::contains(lz::c_string("Hello"), c); });
        CHECK(pos == iterable.end());
        CHECK(lz::distance(iterable.begin(), pos) == 5);
    }
}

TEST_CASE("Find last if not") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_last_if_not(iterable, [](char c) { return c == 'o'; });
        CHECK(*pos == 'l');
        CHECK(lz::distance(iterable.begin(), pos) == 3);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_last_if_not(iterable, [](char c) { return c == 'H'; });
        CHECK(pos == iterable.end());
        CHECK(lz::distance(iterable.begin(), pos) == 1);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_last_if_not(iterable, [](char c) { return c == 'H'; });
        CHECK(pos == iterable.end());
        CHECK(lz::distance(iterable.begin(), pos) == 0);
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        auto pos = lz::find_last_if_not(iterable, [](char c) { return lz::contains(lz::c_string("Hello"), c); });
        CHECK(pos == iterable.end());
        CHECK(lz::distance(iterable.begin(), pos) == 5);
    }
}

TEST_CASE("Find last if not random access") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str, str + std::strlen(str));
        auto pos = lz::find_last_if_not(iterable, [](char c) { return c == 'o'; });
        CHECK(*pos == 'l');
        CHECK(lz::distance(iterable.begin(), pos) == 3);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str, str + std::strlen(str));
        auto pos = lz::find_last_if_not(iterable, [](char c) { return c == 'H'; });
        CHECK(pos == iterable.end());
        CHECK(lz::distance(iterable.begin(), pos) == 1);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str, str + std::strlen(str));
        auto pos = lz::find_last_if_not(iterable, [](char c) { return c == 'H'; });
        CHECK(pos == iterable.end());
        CHECK(lz::distance(iterable.begin(), pos) == 0);
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str, str + std::strlen(str));
        auto pos = lz::find_last_if_not(iterable, [](char c) { return lz::contains(lz::c_string("Hello"), c); });
        CHECK(pos == iterable.end());
        CHECK(lz::distance(iterable.begin(), pos) == 5);
    }
}

TEST_CASE("Find or default") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(lz::find_or_default(iterable, 'o', 'a') == 'o');
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        CHECK(lz::find_or_default(iterable, 'H', 'a') == 'H');
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        CHECK(lz::find_or_default(iterable, 'H', 'a') == 'a');
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(lz::find_or_default(iterable, 'a', 'b') == 'b');
    }
}

TEST_CASE("Find or default if") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(lz::find_or_default_if(iterable, [](char c) { return c == 'o'; }, 'a') == 'o');
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        CHECK(lz::find_or_default_if(iterable, [](char c) { return c == 'H'; }, 'a') == 'H');
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        CHECK(lz::find_or_default_if(iterable, [](char c) { return c == 'H'; }, 'a') == 'a');
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(lz::find_or_default_if(iterable, [](char c) { return c == 'a'; }, 'b') == 'b');
    }
}

TEST_CASE("Find last or default") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(lz::find_last_or_default(iterable, 'o', 'a') == 'o');
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        CHECK(lz::find_last_or_default(iterable, 'H', 'a') == 'H');
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        CHECK(lz::find_last_or_default(iterable, 'H', 'a') == 'a');
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(lz::find_last_or_default(iterable, 'a', 'b') == 'b');
    }
}

TEST_CASE("Find last or default random access") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str, str + std::strlen(str));
        CHECK(lz::find_last_or_default(iterable, 'o', 'a') == 'o');
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str, str + std::strlen(str));
        CHECK(lz::find_last_or_default(iterable, 'H', 'a') == 'H');
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str, str + std::strlen(str));
        CHECK(lz::find_last_or_default(iterable, 'H', 'a') == 'a');
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str, str + std::strlen(str));
        CHECK(lz::find_last_or_default(iterable, 'a', 'b') == 'b');
    }
}

TEST_CASE("Find last or default if") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(lz::find_last_or_default_if(iterable, [](char c) { return c == 'o'; }, 'a') == 'o');
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        CHECK(lz::find_last_or_default_if(iterable, [](char c) { return c == 'H'; }, 'a') == 'H');
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        CHECK(lz::find_last_or_default_if(iterable, [](char c) { return c == 'H'; }, 'a') == 'a');
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(lz::find_last_or_default_if(iterable, [](char c) { return c == 'a'; }, 'b') == 'b');
    }
}

TEST_CASE("Find last or default if random access") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str, str + std::strlen(str));
        CHECK(lz::find_last_or_default_if(iterable, [](char c) { return c == 'o'; }, 'a') == 'o');
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str, str + std::strlen(str));
        CHECK(lz::find_last_or_default_if(iterable, [](char c) { return c == 'H'; }, 'a') == 'H');
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str, str + std::strlen(str));
        CHECK(lz::find_last_or_default_if(iterable, [](char c) { return c == 'H'; }, 'a') == 'a');
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str, str + std::strlen(str));
        CHECK(lz::find_last_or_default_if(iterable, [](char c) { return c == 'a'; }, 'b') == 'b');
    }
}

TEST_CASE("Find last or default not") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(lz::find_last_or_default_not(iterable, 'o', 'a') == 'l');
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        CHECK(lz::find_last_or_default_not(iterable, 'H', 'a') == 'a');
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        CHECK(lz::find_last_or_default_not(iterable, 'H', 'a') == 'a');
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(lz::find_last_or_default_not(iterable, 'a', 'b') == 'o');
    }
}

TEST_CASE("Find last or default not random access") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str, str + std::strlen(str));
        CHECK(lz::find_last_or_default_not(iterable, 'o', 'a') == 'l');
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str, str + std::strlen(str));
        CHECK(lz::find_last_or_default_not(iterable, 'H', 'a') == 'a');
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str, str + std::strlen(str));
        CHECK(lz::find_last_or_default_not(iterable, 'H', 'a') == 'a');
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str, str + std::strlen(str));
        CHECK(lz::find_last_or_default_not(iterable, 'a', 'b') == 'o');
    }
}

TEST_CASE("Index of") {
    SECTION("With non-empty c-string") {
        const char* str = "Helloo";
        auto iterable = lz::c_string(str);
        CHECK(lz::index_of(iterable, 'o') == 4);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        CHECK(lz::index_of(iterable, 'H') == 0);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        CHECK(lz::index_of(iterable, 'H') == lz::npos);
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(lz::index_of(iterable, 'a') == lz::npos);
    }
}

TEST_CASE("Index of if") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(lz::index_of_if(iterable, [](char c) { return c == 'o'; }) == 4);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        CHECK(lz::index_of_if(iterable, [](char c) { return c == 'H'; }) == 0);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        CHECK(lz::index_of_if(iterable, [](char c) { return c == 'H'; }) == lz::npos);
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(lz::index_of_if(iterable, [](char c) { return c == 'a'; }) == lz::npos);
    }
}

TEST_CASE("Contains") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(lz::contains(iterable, 'o'));
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        CHECK(lz::contains(iterable, 'H'));
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        CHECK(!lz::contains(iterable, 'H'));
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(!lz::contains(iterable, 'a'));
    }
}

TEST_CASE("Starts with") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        const char* str2 = "He";
        auto iterable2 = lz::c_string(str2);
        CHECK(lz::starts_with(iterable, iterable2));
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        const char* str2 = "H";
        auto iterable2 = lz::c_string(str2);
        CHECK(lz::starts_with(iterable, iterable2));
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        const char* str2 = "H";
        auto iterable2 = lz::c_string(str2);
        CHECK(!lz::starts_with(iterable, iterable2));
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        const char* str2 = "a";
        auto iterable2 = lz::c_string(str2);
        CHECK(!lz::starts_with(iterable, iterable2));
    }
}

TEST_CASE("Partition") {
    // Use lz::detail explicitly here so to prevent ADL
    SECTION("With non-empty c-string") {
        char str[] = "6789012345";
        auto iterable = lz::c_string(str);
        auto partitioned = lz::detail::partition(iterable.begin(), iterable.end(), [](char c) { return c % 2 == 0; });

        auto part1 = lz::to_iterable(iterable.begin(), partitioned);
        auto part2 = lz::to_iterable(partitioned, iterable.end());

        CHECK(lz::all_of(part1, [](char c) { return c % 2 == 0; }));
        CHECK(lz::none_of(part2, [](char c) { return c % 2 == 0; }));
    }

    SECTION("With one element") {
        char str[] = "6";
        auto iterable = lz::c_string(str);
        auto partitioned = lz::detail::partition(iterable.begin(), iterable.end(), [](char c) { return c % 2 == 0; });

        auto part1 = lz::to_iterable(iterable.begin(), partitioned);
        auto part2 = lz::to_iterable(partitioned, iterable.end());

        CHECK(lz::all_of(part1, [](char c) { return c % 2 == 0; }));
        CHECK(lz::none_of(part2, [](char c) { return c % 2 == 0; }));
    }

    SECTION("With empty c-string") {
        char str[] = "";
        auto iterable = lz::c_string(str);
        auto partitioned = lz::detail::partition(iterable.begin(), iterable.end(), [](char c) { return c % 2 == 0; });

        auto part1 = lz::to_iterable(iterable.begin(), partitioned);
        auto part2 = lz::to_iterable(partitioned, iterable.end());

        CHECK(lz::all_of(part1, [](char c) { return c % 2 == 0; }));
        CHECK(lz::none_of(part2, [](char c) { return c % 2 == 0; }));
    }

    SECTION("With all even c-string") {
        char str[] = "2468";
        auto iterable = lz::c_string(str);
        auto partitioned = lz::detail::partition(iterable.begin(), iterable.end(), [](char c) { return c % 2 == 0; });

        auto part1 = lz::to_iterable(iterable.begin(), partitioned);
        auto part2 = lz::to_iterable(partitioned, iterable.end());

        CHECK(lz::all_of(part1, [](char c) { return c % 2 == 0; }));
        CHECK(lz::none_of(part2, [](char c) { return c % 2 == 0; }));
    }

    SECTION("With all odd c-string") {
        char str[] = "13579";
        auto iterable = lz::c_string(str);
        auto partitioned = lz::detail::partition(iterable.begin(), iterable.end(), [](char c) { return c % 2 == 0; });

        auto part1 = lz::to_iterable(iterable.begin(), partitioned);
        auto part2 = lz::to_iterable(partitioned, iterable.end());

        CHECK(lz::all_of(part2, [](char c) { return c % 2 != 0; }));
        CHECK(lz::none_of(part1, [](char c) { return c % 2 != 0; }));
    }
}

TEST_CASE("Mean") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(lz::mean(iterable, std::plus<int>()) == Approx(('H' + 'e' + 'l' + 'l' + 'o') / 5.0));
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        CHECK(lz::mean(iterable, std::plus<int>()) == Approx(static_cast<double>('H')));
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        CHECK(lz::mean(iterable, std::plus<int>()) == Approx(0.0));
    }
}

TEST_CASE("For each") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        std::string result;
        lz::for_each(iterable, [&result](char c) { result += c; });
        CHECK(result == "Hello");
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        std::string result;
        lz::for_each(iterable, [&result](char c) { result += c; });
        CHECK(result == "H");
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        std::string result;
        lz::for_each(iterable, [&result](char c) { result += c; });
        CHECK(result.empty());
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
        CHECK(result == "Hel");
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        std::string result;
        lz::for_each_while(iterable, [&result](char c) {
            result += c;
            return c != 'H';
        });
        CHECK(result == "H");
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
        CHECK(result.empty());
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        std::string result;
        lz::for_each_while(iterable, [&result](char c) {
            result += c;
            return c != 'H';
        });
        CHECK(result.empty());
    }
}

TEST_CASE("Copy") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        std::string result;
        lz::copy(iterable, std::back_inserter(result));
        CHECK(result == "Hello");
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        std::string result;
        lz::copy(iterable, std::back_inserter(result));
        CHECK(result == "H");
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        std::string result;
        lz::copy(iterable, std::back_inserter(result));
        CHECK(result.empty());
    }
}

TEST_CASE("Transform") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        std::string result;
        lz::transform(iterable, std::back_inserter(result), [](char c) { return static_cast<char>(c + 1); });
        CHECK(result == "Ifmmp");
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        std::string result;
        lz::transform(iterable, std::back_inserter(result), [](char c) { return static_cast<char>(c + 1); });
        CHECK(result == "I");
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        std::string result;
        lz::transform(iterable, std::back_inserter(result), [](char c) { return static_cast<char>(c + 1); });
        CHECK(result.empty());
    }
}

TEST_CASE("Equal") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        const char* str2 = "Hello";
        auto iterable2 = lz::c_string(str2);
        CHECK(lz::equal(iterable, iterable2));
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        const char* str2 = "H";
        auto iterable2 = lz::c_string(str2);
        CHECK(lz::equal(iterable, iterable2));
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        const char* str2 = "";
        auto iterable2 = lz::c_string(str2);
        CHECK(lz::equal(iterable, iterable2));
    }

    SECTION("Not equal c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        const char* str2 = "Helloo";
        auto iterable2 = lz::c_string(str2);
        CHECK(!lz::equal(iterable, iterable2));
    }
}

TEST_CASE("Lower bound") {
    SECTION("With non-empty c-string") {
        const char* str = "aaabcccdeee";
        auto iterable = lz::c_string(str);
        auto it = lz::lower_bound(iterable, 'c');
        CHECK(*it == 'c');
        CHECK(lz::distance(iterable.begin(), it) == 4);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        auto it = lz::lower_bound(iterable, 'H');
        CHECK(*it == 'H');
        CHECK(lz::distance(iterable.begin(), it) == 0);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        auto it = lz::lower_bound(iterable, 'H');
        CHECK(it == iterable.end());
        CHECK(lz::distance(iterable.begin(), it) == 0);
    }

    SECTION("Not found c-string") {
        const char* str = "aaabcccdeee";
        auto iterable = lz::c_string(str);
        auto it = lz::lower_bound(iterable, 'f');
        CHECK(it == iterable.end());
        CHECK(lz::distance(iterable.begin(), it) == 11);
    }
}

TEST_CASE("Upper bound") {
    SECTION("With non-empty c-string") {
        const char* str = "aaabcccdeee";
        auto iterable = lz::c_string(str);
        auto it = lz::upper_bound(iterable, 'c');
        CHECK(*it == 'd');
        CHECK(lz::distance(iterable.begin(), it) == 7);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        auto it = lz::upper_bound(iterable, 'H');
        CHECK(it == iterable.end());
        CHECK(lz::distance(iterable.begin(), it) == 1);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        auto it = lz::upper_bound(iterable, 'H');
        CHECK(it == iterable.end());
        CHECK(lz::distance(iterable.begin(), it) == 0);
    }

    SECTION("Not found c-string") {
        const char* str = "aaabcccdeee";
        auto iterable = lz::c_string(str);
        auto it = lz::upper_bound(iterable, 'f');
        CHECK(it == iterable.end());
        CHECK(lz::distance(iterable.begin(), it) == 11);
    }
}

TEST_CASE("Binary search") {
    SECTION("With non-empty c-string") {
        const char* str = "aaabcccdeee";
        auto iterable = lz::c_string(str);
        CHECK(lz::binary_search(iterable, 'c'));
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        CHECK(lz::binary_search(iterable, 'H'));
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        CHECK(!lz::binary_search(iterable, 'H'));
    }

    SECTION("Not found c-string") {
        const char* str = "aaabcccdeee";
        auto iterable = lz::c_string(str);
        CHECK(!lz::binary_search(iterable, 'f'));
    }
}

TEST_CASE("All of") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(lz::all_of(iterable, [](char c) { return c != 'a'; }));
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        CHECK(lz::all_of(iterable, [](char c) { return c == 'H'; }));
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        CHECK(lz::all_of(iterable, [](char c) { return c == 'H'; }));
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(!lz::all_of(iterable, [](char c) { return c == 'a'; }));
    }
}

TEST_CASE("Any of") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(lz::any_of(iterable, [](char c) { return c == 'o'; }));
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        CHECK(lz::any_of(iterable, [](char c) { return c == 'H'; }));
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        CHECK(!lz::any_of(iterable, [](char c) { return c == 'H'; }));
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(!lz::any_of(iterable, [](char c) { return c == 'a'; }));
    }
}

TEST_CASE("None of") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(lz::none_of(iterable, [](char c) { return c == 'a'; }));
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        CHECK(lz::none_of(iterable, [](char c) { return c == 'a'; }));
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        CHECK(lz::none_of(iterable, [](char c) { return c == 'a'; }));
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(!lz::none_of(iterable, [](char c) { return c == 'o'; }));
    }
}

TEST_CASE("Adjacent find") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        auto it = lz::adjacent_find(iterable);
        CHECK(*it == 'l');
        CHECK(lz::distance(iterable.begin(), it) == 2);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        auto it = lz::adjacent_find(iterable);
        CHECK(it == iterable.end());
        CHECK(lz::distance(iterable.begin(), it) == 1);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        auto it = lz::adjacent_find(iterable);
        CHECK(it == iterable.end());
        CHECK(lz::distance(iterable.begin(), it) == 0);
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        auto it = lz::adjacent_find(iterable, [](char a, char b) { return a == 'a' && b == 'a'; });
        CHECK(it == iterable.end());
        CHECK(lz::distance(iterable.begin(), it) == 5);
    }
}

TEST_CASE("Count") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(lz::count(iterable, 'l') == 2);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        CHECK(lz::count(iterable, 'H') == 1);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        CHECK(lz::count(iterable, 'H') == 0);
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(lz::count(iterable, 'a') == 0);
    }
}

TEST_CASE("Count if") {
    SECTION("With non-empty c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(lz::count_if(iterable, [](char c) { return c == 'l'; }) == 2);
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        CHECK(lz::count_if(iterable, [](char c) { return c == 'H'; }) == 1);
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        CHECK(lz::count_if(iterable, [](char c) { return c == 'H'; }) == 0);
    }

    SECTION("Not found c-string") {
        const char* str = "Hello";
        auto iterable = lz::c_string(str);
        CHECK(lz::count_if(iterable, [](char c) { return c == 'a'; }) == 0);
    }
}

TEST_CASE("Is sorted") {
    SECTION("With non-empty c-string") {
        const char* str = "abcde";
        auto iterable = lz::c_string(str);
        CHECK(lz::is_sorted(iterable));
    }

    SECTION("With one element") {
        const char* str = "H";
        auto iterable = lz::c_string(str);
        CHECK(lz::is_sorted(iterable));
    }

    SECTION("With empty c-string") {
        const char* str = "";
        auto iterable = lz::c_string(str);
        CHECK(lz::is_sorted(iterable));
    }

    SECTION("Not sorted c-string") {
        const char* str = "abcdea";
        auto iterable = lz::c_string(str);
        CHECK(!lz::is_sorted(iterable));
    }
}