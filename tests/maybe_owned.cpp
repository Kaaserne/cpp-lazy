#include <Lz/detail/maybe_owned.hpp>
#include <Lz/filter.hpp>
#include <doctest/doctest.h>
#include <pch.hpp>

namespace doctest {
namespace detail {
// workaround for volatile pointers not being handled correctly by the default filldata

// clang-format off

template<typename T>
struct filldata<volatile T*> {
DOCTEST_MSVC_SUPPRESS_WARNING_WITH_PUSH(4180)
    static void fill(std::ostream* stream, const volatile T* in) {
        DOCTEST_MSVC_SUPPRESS_WARNING_POP
        DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wmicrosoft-cast")
        filldata<const volatile void*>::fill(stream,
#if DOCTEST_GCC == 0 || DOCTEST_GCC >= DOCTEST_COMPILER(4, 9, 0)
                                             reinterpret_cast<const volatile void*>(in)
#else
                                             *reinterpret_cast<const volatile void* const*>(&in)
#endif
        );
DOCTEST_CLANG_SUPPRESS_WARNING_POP
    }
};

// clang-format on
template<>
void filldata<const volatile void*>::fill(std::ostream* stream, const volatile void* in) {
    if (in) {
        *stream << in;
    }
    else {
        *stream << "nullptr";
    }
}
} // namespace detail
} // namespace doctest

TEST_CASE("maybe_owned basic tests") {
    SUBCASE("STD container") {
        std::vector<int> vec{ 1, 2, 3 };
        lz::maybe_owned<std::vector<int>> maybe_owned{ vec };
        static_assert(std::is_same<decltype(maybe_owned), lz::detail::maybe_owned_impl<std::vector<int>, false>>::value,
                      "Should be by ref");
        REQUIRE(&(*vec.begin()) == &(*maybe_owned.begin()));
    }

    SUBCASE("Const STD container") {
        const std::vector<int> vec{ 1, 2, 3 };
        lz::maybe_owned<const std::vector<int>> maybe_owned{ vec };
        static_assert(std::is_same<decltype(maybe_owned), lz::detail::maybe_owned_impl<const std::vector<int>, false>>::value,
                      "Should be by ref");
        REQUIRE(&(*vec.begin()) == &(*maybe_owned.begin()));
    }

    SUBCASE("C array") {
        int arr[] = { 1, 2, 3 };
        lz::maybe_owned<int[3]> maybe_owned{ arr };
        static_assert(std::is_same<decltype(maybe_owned), lz::detail::maybe_owned_impl<int[3], false>>::value,
                      "Should be by ref");
        REQUIRE(&(*arr) == &(*maybe_owned.begin()));
    }

    SUBCASE("Volatile C array") {
        volatile int arr[] = { 1, 2, 3 };
        lz::maybe_owned<volatile int[3]> maybe_owned{ arr };
        static_assert(std::is_same<decltype(maybe_owned), lz::detail::maybe_owned_impl<volatile int[3], false>>::value,
                      "Should be by ref");
        REQUIRE(&(*arr) == &(*maybe_owned.begin()));
    }

    SUBCASE("Const C array") {
        const int arr[] = { 1, 2, 3 };
        lz::maybe_owned<const int[3]> maybe_owned{ arr };
        REQUIRE(&(*arr) == &(*maybe_owned.begin()));
        static_assert(std::is_same<decltype(maybe_owned), lz::detail::maybe_owned_impl<const int[3], false>>::value,
                      "Should be by ref");
    }

    SUBCASE("Const volatile C array") {
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
    SUBCASE("Iterable that is inherited by lz::lazy_view") {
        std::vector<int> vec{ 1, 2, 3 };
        auto filter = lz::filter(vec, [](int i) { return i > 1; });
        auto copied = lz::as_copied(filter);
        REQUIRE(&(*filter.begin()) == &(*copied.begin()));
    }

    SUBCASE("Iterable that is not inherited by lz::lazy_view") {
        std::vector<int> vec{ 1, 2, 3 };
        auto copied = lz::as_copied(vec);
        REQUIRE(&(*vec.begin()) != &(*copied.begin()));
    }

    SUBCASE("Custom iterable") {
        int arr[] = { 1, 2, 3 };
        iterable it{ arr, arr + lz::size(arr) };
        lz::copied<iterable> copied = lz::as_copied(it);
        REQUIRE(&(*it.begin()) == &(*copied.begin()));
    }
}
