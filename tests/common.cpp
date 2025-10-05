#include <Lz/algorithm/equal.hpp>
#include <Lz/cached_size.hpp>
#include <Lz/chunk_if.hpp>
#include <Lz/chunks.hpp>
#include <Lz/common.hpp>
#include <Lz/generate.hpp>
#include <Lz/repeat.hpp>
#include <Lz/reverse.hpp>
#include <cpp-lazy-ut-helper/pch.hpp>
#include <cpp-lazy-ut-helper/ut_helper.hpp>
#include <doctest/doctest.h>

// TODO recheck all include files, make smaller if possible

class random_access_non_sized : lz::lazy_view {
    lz::repeat_iterable<int> _repeater;

public:
    random_access_non_sized() = default;

    explicit random_access_non_sized(lz::repeat_iterable<int> rep) : _repeater{ rep } {
    }

    lz::iter_t<lz::repeat_iterable<int>> begin() const {
        return _repeater.begin();
    }

    lz::sentinel_t<lz::repeat_iterable<int>> end() const {
        return _repeater.end();
    }
};
// TODO common iterable
TEST_CASE("Common") {
    SUBCASE("No sentinel") {
        std::vector<int> vec = { 1, 2, 3, 4, 5 };
        auto&& common = lz::common(vec);
        static_assert(std::is_same<decltype(common), std::vector<int>&>::value, "");
        REQUIRE(lz::equal(common, vec));
    }

    SUBCASE("forward non sized") {
        std::string str = "hello, world!";
        auto chunk = lz::sv_chunk_if(str, [](char c) { return c == ','; });
        auto common = lz::common(chunk);
        static_assert(std::is_same<decltype(common), lz::basic_iterable<lz::iter_t<decltype(common)>>>::value, "");
        std::vector<lz::string_view> expected = { "hello", " world!" };
        static_assert(!lz::is_sized<decltype(common)>::value, "");
        REQUIRE(lz::equal(common, expected));
    }

    SUBCASE("forward sized") {
        auto gen = lz::generate([]() { return 1; }, 3);
        auto expected = { 1, 1, 1 };
        auto common = lz::common(gen);
        static_assert(std::is_same<decltype(common), lz::sized_iterable<lz::iter_t<decltype(gen)>>>::value, "");
        REQUIRE(lz::size(common) == 3);
        REQUIRE(lz::equal(common, expected));
    }

    SUBCASE("bidirectional non sized") {
        std::vector<int> vec = { 1, 2, 3, 4, 5 };
        auto filter = make_non_sized_bidi_sentinelled(lz::filter(vec, [](int) { return true; }));
        auto common = lz::common(filter);

        std::vector<int> expected = { 1, 2, 3, 4, 5 };
        REQUIRE(lz::equal(common, expected));
        REQUIRE(lz::equal(common | lz::reverse, expected | lz::reverse));
    }

    SUBCASE("bidirectional sized") {
        std::list<int> lst = { 1, 2, 3, 4, 5 };
        auto chunks = lz::chunks(lst, 2);
        auto sent = make_sized_bidi_sentinelled(chunks) | lz::cache_size;
        auto common = lz::common(sent);

        using common_t = decltype(common);
        static_assert(std::is_same<common_t, lz::sized_iterable<lz::iter_t<common_t>, lz::sentinel_t<common_t>>>::value, "");
        static_assert(lz::is_sized<decltype(common)>::value, "");
        using value_type = decltype(*common.begin());

        std::vector<std::vector<int>> expected = { { 1, 2 }, { 3, 4 }, { 5 } };
        REQUIRE(lz::size(common) == 3);
        REQUIRE(lz::equal(common, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));
        REQUIRE(lz::equal(common | lz::reverse, expected | lz::reverse,
                          [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));
    }

    SUBCASE("random access non sized") {
        random_access_non_sized non_sized_vec{ lz::repeat(1, 5) };
        auto common = lz::common(non_sized_vec);
        static_assert(std::is_same<decltype(common),
                                   lz::basic_iterable<lz::iter_t<decltype(common)>, lz::sentinel_t<decltype(common)>>>::value,
                      "");
        static_assert(lz::is_sized<decltype(common)>::value, ""); // because random access
        REQUIRE(lz::size(common) == 5);
        auto expected = { 1, 1, 1, 1, 1 };
        REQUIRE(lz::equal(common, expected));
    }

    SUBCASE("random access sized") {
        auto rep = lz::repeat(1, 5);
        auto common = lz::common(rep);
        using common_t = decltype(common);
        static_assert(std::is_same<common_t, lz::basic_iterable<lz::iter_t<common_t>, lz::sentinel_t<common_t>>>::value, "");
        static_assert(lz::is_sized<decltype(common)>::value, "");
        REQUIRE(lz::size(common) == 5);
        auto expected = { 1, 1, 1, 1, 1 };
        REQUIRE(lz::equal(common, expected));
    }
}
