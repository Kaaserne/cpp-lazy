#include <Lz/detail/ref_or_view.hpp>
#include <Lz/filter.hpp>
#include <Lz/repeat.hpp>
#include <catch2/catch.hpp>
#include <vector>

TEST_CASE("ref_or_view basic tests") {
    SECTION("STD container") {
        std::vector<int> vec{ 1, 2, 3 };
        lz::ref_or_view<std::vector<int>> ref_or_view{ vec };
        static_assert(std::is_same<decltype(ref_or_view), lz::detail::ref_or_view_helper<std::vector<int>, false>>::value, "Should be by ref");
        REQUIRE(&(*vec.begin()) == &(*ref_or_view.begin()));
    }
    
    SECTION("Const STD container") {
        const std::vector<int> vec{ 1, 2, 3 };
        lz::ref_or_view<const std::vector<int>> ref_or_view{ vec };
        static_assert(std::is_same<decltype(ref_or_view), lz::detail::ref_or_view_helper<const std::vector<int>, false>>::value, "Should be by ref");
        REQUIRE(&(*vec.begin()) == &(*ref_or_view.begin()));
    }

    SECTION("C array") {
        int arr[] = { 1, 2, 3 };
        lz::ref_or_view<int[3]> ref_or_view{ arr };
        static_assert(std::is_same<decltype(ref_or_view), lz::detail::ref_or_view_helper<int[3], false>>::value, "Should be by ref");
        REQUIRE(&(*arr) == &(*ref_or_view.begin()));
    }

    SECTION("Volatile C array") {
        volatile int arr[] = { 1, 2, 3 };
        lz::ref_or_view<volatile int[3]> ref_or_view{ arr };
        static_assert(std::is_same<decltype(ref_or_view), lz::detail::ref_or_view_helper<volatile int[3], false>>::value, "Should be by ref");
        REQUIRE(&(*arr) == &(*ref_or_view.begin()));
    }

    SECTION("Const C array") {
        const int arr[] = { 1, 2, 3 };
        lz::ref_or_view<const int[3]> ref_or_view{ arr };
        REQUIRE(&(*arr) == &(*ref_or_view.begin()));
        static_assert(std::is_same<decltype(ref_or_view), lz::detail::ref_or_view_helper<const int[3], false>>::value, "Should be by ref");
    }

    SECTION("Const volatile C array") {
        const volatile int arr[] = { 1, 2, 3 };
        lz::ref_or_view<const volatile int[3]> ref_or_view{ arr };
        REQUIRE(&(*arr) == &(*ref_or_view.begin()));
        static_assert(std::is_same<decltype(ref_or_view), lz::detail::ref_or_view_helper<const volatile int[3], false>>::value, "Should be by ref");
    }

    SECTION("lz iterable") {
        auto iterable = lz::repeat(20, 5);
        lz::ref_or_view<decltype(iterable)> ref_or_view{ iterable };
        REQUIRE(&(*iterable.begin()) != &(*ref_or_view.begin()));
        REQUIRE(ref_or_view.size() == iterable.size());
    }
}

struct iterable {
    int* _begin;
    int* _end;

    int* begin() {
        return _begin;
    }

    int* end() {
        return _end;
    }
};

TEST_CASE("lz::copied_iterable") {
    SECTION("Iterable that is inherited by lz::lazy_view") {
        std::vector<int> vec{ 1, 2, 3 };
        auto filter = lz::filter(vec, [](int i) { return i > 1; });
        auto copied = lz::as_copied_iterable(filter);
        REQUIRE(&(*filter.begin()) == &(*copied.begin()));
    }

    SECTION("Iterable that is not inherited by lz::lazy_view") {
        std::vector<int> vec{ 1, 2, 3 };
        auto copied = lz::as_copied_iterable(vec);
        REQUIRE(&(*vec.begin()) != &(*copied.begin()));
    }

    SECTION("Custom iterable") {
        int arr[] = { 1, 2, 3 };
        iterable it{ arr, arr + lz::size(arr) };
        auto copied = lz::as_copied_iterable(it);
        REQUIRE(&(*it.begin()) == &(*copied.begin()));
    }
}
