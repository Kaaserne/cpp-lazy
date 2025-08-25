#include <Lz/repeat.hpp>
#include <cpp-lazy-ut-helper/c_string.hpp>
#include <doctest/doctest.h>
#include <pch.hpp>

TEST_CASE("Sentinel operators") {
    SUBCASE("Operator==") {
        static_assert(lz::default_sentinel == lz::default_sentinel, "");
    }

    SUBCASE("Operator!=") {
        static_assert(!(lz::default_sentinel != lz::default_sentinel), "");
    }

    SUBCASE("Operator<") {
        static_assert(!(lz::default_sentinel < lz::default_sentinel), "");
    }

    SUBCASE("Operator>") {
        static_assert(!(lz::default_sentinel > lz::default_sentinel), "");
    }

    SUBCASE("Operator<=") {
        static_assert(lz::default_sentinel <= lz::default_sentinel, "");
    }

    SUBCASE("Operator>=") {
        static_assert(lz::default_sentinel >= lz::default_sentinel, "");
    }
}

TEST_CASE("default_sentinel with iterator operators") {
    auto repeater = lz::repeat(20, 5);

    SUBCASE("Operator== with iterator") {
        auto last = repeater.begin() + lz::ssize(repeater);
        REQUIRE(last == lz::default_sentinel);
        REQUIRE(last.operator==(lz::default_sentinel));
        REQUIRE(lz::default_sentinel == last);

        --last;
        REQUIRE_FALSE(last == lz::default_sentinel);
        REQUIRE_FALSE(last.operator==(lz::default_sentinel));
        REQUIRE_FALSE(lz::default_sentinel == last);
    }

    SUBCASE("Operator!= with iterator") {
        auto last = repeater.begin() + lz::ssize(repeater);
        REQUIRE_FALSE(last != lz::default_sentinel);
        REQUIRE_FALSE(last.operator!=(lz::default_sentinel));
        REQUIRE_FALSE(lz::default_sentinel != last);

        --last;
        REQUIRE(last != lz::default_sentinel);
        REQUIRE(last.operator!=(lz::default_sentinel));
        REQUIRE(lz::default_sentinel != last);
    }

    SUBCASE("Operator> with iterator") {
        auto last = repeater.begin() + lz::ssize(repeater);
        REQUIRE_FALSE(last > lz::default_sentinel);
        REQUIRE_FALSE(last.operator>(lz::default_sentinel));
        REQUIRE_FALSE(lz::default_sentinel > last);

        --last;
        REQUIRE_FALSE(last > lz::default_sentinel);
        REQUIRE_FALSE(last.operator>(lz::default_sentinel));
        REQUIRE(lz::default_sentinel > last);
    }

    SUBCASE("Operator< with iterator") {
        auto last = repeater.begin() + lz::ssize(repeater);
        REQUIRE_FALSE(last < lz::default_sentinel);
        REQUIRE_FALSE(last.operator<(lz::default_sentinel));
        REQUIRE_FALSE(lz::default_sentinel < last);

        --last;
        REQUIRE(last < lz::default_sentinel);
        REQUIRE(last.operator<(lz::default_sentinel));
        REQUIRE_FALSE(lz::default_sentinel < last);
    }

    SUBCASE("Operator>= with iterator") {
        auto last = repeater.begin() + lz::ssize(repeater);
        REQUIRE(last >= lz::default_sentinel);
        REQUIRE(last.operator>=(lz::default_sentinel));
        REQUIRE(lz::default_sentinel >= last);

        --last;
        REQUIRE_FALSE(last >= lz::default_sentinel);
        REQUIRE_FALSE(last.operator>=(lz::default_sentinel));
        REQUIRE(lz::default_sentinel >= last);
    }

    SUBCASE("Operator<= with iterator") {
        auto last = repeater.begin() + lz::ssize(repeater);
        REQUIRE(last <= lz::default_sentinel);
        REQUIRE(last.operator<=(lz::default_sentinel));
        REQUIRE(lz::default_sentinel <= last);

        --last;
        REQUIRE(last <= lz::default_sentinel);
        REQUIRE(last.operator<=(lz::default_sentinel));
        REQUIRE_FALSE(lz::default_sentinel <= last);
    }

    SUBCASE("Operator- with iterator") {
        auto last = repeater.begin() + lz::ssize(repeater);
        REQUIRE(last - lz::default_sentinel == 0);
        REQUIRE(last.operator-(lz::default_sentinel) == 0);
        REQUIRE(lz::default_sentinel - last == 0);

        --last;
        REQUIRE(last - lz::default_sentinel == -1);
        REQUIRE(last.operator-(lz::default_sentinel) == -1);
        REQUIRE(lz::default_sentinel - last == 1);
    }
}

TEST_CASE("Helper functions") {
    SUBCASE("next_fast ra") {
        std::vector<int> vec = { 1, 2, 3, 4, 5 };
        auto next = lz::detail::next_fast(vec, 2);
        REQUIRE(next == std::next(vec.begin(), 2));
        REQUIRE(*next == 3);

        next = lz::detail::next_fast(vec, 4);
        REQUIRE(next == std::next(vec.begin(), 4));
        REQUIRE(*next == 5);
    }

    SUBCASE("next_fast ra sentinel") {
        auto repeat = lz::repeat(1, 5);
        auto next = lz::detail::next_fast(repeat, 2);
        REQUIRE(next == std::next(repeat.begin(), 2));
        REQUIRE(*next == 1);

        next = lz::detail::next_fast(repeat, 5);
        REQUIRE(next == std::end(repeat));
    }

    SUBCASE("next_fast bidi") {
        std::list<int> lst = { 1, 2, 3, 4, 5 };
        auto next = lz::detail::next_fast(lst, 2);
        REQUIRE(next == std::next(lst.begin(), 2));
        REQUIRE(*next == 3);

        next = lz::detail::next_fast(lst, 4);
        REQUIRE(next == std::next(lst.begin(), 4));
        REQUIRE(*next == 5);
    }

    SUBCASE("next_fast fwd") {
        auto cstr = lz::c_string("Hello");
        auto next = lz::detail::next_fast(cstr, 2);
        REQUIRE(next == std::next(cstr.begin(), 2));
        REQUIRE(*next == 'l');

        next = lz::detail::next_fast(cstr, 4);
        REQUIRE(next == std::next(cstr.begin(), 4));
        REQUIRE(*next == 'o');
    }

    SUBCASE("next_fast_safe ra") {
        std::vector<int> vec = { 1, 2, 3, 4, 5 };
        auto next = lz::detail::next_fast_safe(vec, 2);
        REQUIRE(next == std::next(vec.begin(), 2));
        REQUIRE(*next == 3);

        next = lz::detail::next_fast_safe(vec, lz::ssize(vec) + 1);
        REQUIRE(next == std::end(vec));
    }

    SUBCASE("next_fast_safe ra sentinel") {
        auto repeat = lz::repeat(1, 5);
        auto next = lz::detail::next_fast_safe(repeat, 2);
        REQUIRE(next == std::next(repeat.begin(), 2));
        REQUIRE(*next == 1);

        next = lz::detail::next_fast_safe(repeat, lz::ssize(repeat) + 1);
        REQUIRE(next == std::end(repeat));
    }

    SUBCASE("next_fast_safe fwd") {
        auto cstr = lz::c_string("Hel1o");
        auto next = lz::detail::next_fast_safe(cstr, 2);
        REQUIRE(next == std::next(cstr.begin(), 2));
        REQUIRE(*next == 'l');

        next = lz::detail::next_fast_safe(cstr, lz::distance(cstr) + 1);
        REQUIRE(next == std::end(cstr));
    }

    SUBCASE("next_fast_safe bidi") {
        std::list<int> lst = { 1, 2, 3, 4, 5 };
        auto next = lz::detail::next_fast_safe(lst, 2);
        REQUIRE(next == std::next(lst.begin(), 2));
        REQUIRE(*next == 3);

        next = lz::detail::next_fast_safe(lst, lz::ssize(lst) + 1);
        REQUIRE(next == std::end(lst));
    }
}
