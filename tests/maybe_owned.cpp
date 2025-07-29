#include <Lz/detail/maybe_owned.hpp>
#include <Lz/filter.hpp>
#include <catch2/catch.hpp>
#include <vector>

TEST_CASE("maybe_owned basic tests") {
    SECTION("STD container") {
        std::vector<int> vec{ 1, 2, 3 };
        lz::maybe_owned<std::vector<int>> maybe_owned{ vec };
        static_assert(std::is_same<decltype(maybe_owned), lz::detail::maybe_owned_impl<std::vector<int>, false>>::value,
                      "Should be by ref");
        REQUIRE(&(*vec.begin()) == &(*maybe_owned.begin()));
    }

    SECTION("Const STD container") {
        const std::vector<int> vec{ 1, 2, 3 };
        lz::maybe_owned<const std::vector<int>> maybe_owned{ vec };
        static_assert(std::is_same<decltype(maybe_owned), lz::detail::maybe_owned_impl<const std::vector<int>, false>>::value,
                      "Should be by ref");
        REQUIRE(&(*vec.begin()) == &(*maybe_owned.begin()));
    }

    SECTION("C array") {
        int arr[] = { 1, 2, 3 };
        lz::maybe_owned<int[3]> maybe_owned{ arr };
        static_assert(std::is_same<decltype(maybe_owned), lz::detail::maybe_owned_impl<int[3], false>>::value,
                      "Should be by ref");
        REQUIRE(&(*arr) == &(*maybe_owned.begin()));
    }

    SECTION("Volatile C array") {
        volatile int arr[] = { 1, 2, 3 };
        lz::maybe_owned<volatile int[3]> maybe_owned{ arr };
        static_assert(std::is_same<decltype(maybe_owned), lz::detail::maybe_owned_impl<volatile int[3], false>>::value,
                      "Should be by ref");
        REQUIRE(&(*arr) == &(*maybe_owned.begin()));
    }

    SECTION("Const C array") {
        const int arr[] = { 1, 2, 3 };
        lz::maybe_owned<const int[3]> maybe_owned{ arr };
        REQUIRE(&(*arr) == &(*maybe_owned.begin()));
        static_assert(std::is_same<decltype(maybe_owned), lz::detail::maybe_owned_impl<const int[3], false>>::value,
                      "Should be by ref");
    }

    SECTION("Const volatile C array") {
        const volatile int arr[] = { 1, 2, 3 };
        lz::maybe_owned<const volatile int[3]> maybe_owned{ arr };
        REQUIRE(&(*arr) == &(*maybe_owned.begin()));
        static_assert(std::is_same<decltype(maybe_owned), lz::detail::maybe_owned_impl<const volatile int[3], false>>::value,
                      "Should be by ref");
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

TEST_CASE("lz::copied") {
    SECTION("Iterable that is inherited by lz::lazy_view") {
        std::vector<int> vec{ 1, 2, 3 };
        auto filter = lz::filter(vec, [](int i) { return i > 1; });
        auto copied = lz::as_copied(filter);
        REQUIRE(&(*filter.begin()) == &(*copied.begin()));
    }

    SECTION("Iterable that is not inherited by lz::lazy_view") {
        std::vector<int> vec{ 1, 2, 3 };
        auto copied = lz::as_copied(vec);
        REQUIRE(&(*vec.begin()) != &(*copied.begin()));
    }

    SECTION("Custom iterable") {
        int arr[] = { 1, 2, 3 };
        iterable it{ arr, arr + lz::size(arr) };
        lz::copied<iterable> copied = lz::as_copied(it);
        REQUIRE(&(*it.begin()) == &(*copied.begin()));
    }
}
