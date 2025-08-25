#include <Lz/lz.hpp>
#include <array>
#include <benchmark/benchmark.h>

#if LZ_HAS_INCLUDE(<ranges>)

#include <ranges>

#endif

namespace {
constexpr std::size_t size_policy = 32;

void any_iterable_lz(benchmark::State& state) {
    auto arr = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (int i : lz::make_any_iterable(arr)) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void c_string_lz(benchmark::State& state) {
    constexpr const char* str = "this is a 32 char long stringggg";

    for (auto _ : state) {
#ifdef LZ_HAS_CXX_17
        for (char c : lz::c_string(str)) {
            benchmark::DoNotOptimize(c);
        }
#else
        auto c_str = lz::c_string(str);
        lz::for_each(c_str, [](char c) { benchmark::DoNotOptimize(c); });
#endif
    }
}

void cached_reverse_lz(benchmark::State& state) {
    auto arr = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (int c : lz::cached_reverse(arr)) {
            benchmark::DoNotOptimize(c);
        }
    }
}

void cartesian_product_lz(benchmark::State& state) {
    std::array<int, size_policy / 8> a;
    std::array<char, size_policy / 4> b;

    for (auto _ : state) {
        for (auto&& tup : lz::cartesian_product(a, b)) {
            benchmark::DoNotOptimize(tup);
        }
    }
}

void chunk_if_lz(benchmark::State& state) {
    auto a = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();
    constexpr static auto half = static_cast<int>(size_policy / 2);

    for (auto _ : state) {
#ifdef LZ_HAS_CXX_17
        for (auto&& x : lz::chunk_if(a, [](int i) noexcept { return i == half; })) {
            for (int y : x) {
                benchmark::DoNotOptimize(y);
            }
        }
#else
        auto chunk_if = lz::chunk_if(a, [](int i) noexcept { return i == half; });
        using ref = lz::ref_iterable_t<decltype(chunk_if)>;
        lz::for_each(chunk_if, [](ref chunk) {
            for (int x : chunk) {
                benchmark::DoNotOptimize(x);
            }
        });
#endif
    }
}

#if defined(__cpp_lib_ranges_chunk_by) && LZ_HAS_INCLUDE(<ranges>) && CMAKE_CXX_STANDARD >= 23

void chunk_if_std(benchmark::State& state) {
    auto a = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();
    constexpr static auto half = static_cast<int>(size_policy / 2);

    for (auto _ : state) {
        for (auto&& x : std::ranges::chunk_by(a, [](int i) noexcept { return i == half; })) {
            for (int y : x) {
                benchmark::DoNotOptimize(y);
            }
        }
    }
}

#endif

void chunks_lz(benchmark::State& state) {
    auto a = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();
    for (auto _ : state) {
        for (auto&& chunk : lz::chunks(a, 8)) {
            for (int x : chunk) {
                benchmark::DoNotOptimize(x);
            }
        }
    }
}

#if defined(__cpp_lib_ranges_chunk) && LZ_HAS_INCLUDE(<ranges>) && CMAKE_CXX_STANDARD >= 23

void chunks_std(benchmark::State& state) {
    auto a = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();
    for (auto _ : state) {
        for (auto&& chunk : std::views::chunks(a, 8)) {
            for (int x : chunk) {
                benchmark::DoNotOptimize(x);
            }
        }
    }
}

#endif

void common_lz(benchmark::State& state) {
    auto a = lz::generate([]() noexcept { return 0; }, size_policy);

    for (auto _ : state) {
        for (int i : lz::common(a)) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void concatenate_lz(benchmark::State& state) {
    std::string a(size_policy / 2, '0');
    std::string b(size_policy / 2, '1');

    for (auto _ : state) {
        for (char c : lz::concat(a, b)) {
            benchmark::DoNotOptimize(c);
        }
    }
}

void duplicates_lz(benchmark::State& state) {
    auto arr = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (std::pair<char, std::size_t> c : lz::duplicates(arr)) {
            benchmark::DoNotOptimize(c);
        }
    }
}

void enumerate_lz(benchmark::State& state) {
    auto arr = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (auto pair : lz::enumerate(arr)) {
            benchmark::DoNotOptimize(pair);
        }
    }
}

void except_lz(benchmark::State& state) {
    auto large_arr = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();
    auto large_arr_except = lz::range(static_cast<int>(size_policy) / 2) | lz::to<std::array<int, size_policy / 2>>();

    for (auto _ : state) {
#ifdef LZ_HAS_CXX_17
        for (auto excepted : lz::except(large_arr, large_arr_except)) {
            benchmark::DoNotOptimize(excepted);
        }
#else
        auto except = lz::except(large_arr, large_arr_except);
        lz::for_each(except, [](int i) { benchmark::DoNotOptimize(i); });
#endif
    }
}

void exclude_lz(benchmark::State& state) {
    auto a = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
#ifdef LZ_HAS_CXX_17
        for (int i : lz::exclude(a, 5, 10)) {
            benchmark::DoNotOptimize(i);
        }
#else
        auto exclude = lz::exclude(a, 5, 10);
        lz::for_each(exclude, [](int i) { benchmark::DoNotOptimize(i); });
#endif
    }
}

void exclusive_scan_lz(benchmark::State& state) {
    auto array = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
#ifdef LZ_HAS_CXX_17
        for (int i : lz::exclusive_scan(array, 0)) {
            benchmark::DoNotOptimize(i);
        }
#else
        auto scan = lz::exclusive_scan(array, 0);
        lz::for_each(scan, [](int i) { benchmark::DoNotOptimize(i); });
#endif
    }
}

void filter_lz(benchmark::State& state) {
    auto arr = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (int filtered : lz::filter(arr, [](const int i) noexcept { return i == 0; })) {
            benchmark::DoNotOptimize(filtered);
        }
    }
}

void flatten_lz(benchmark::State& state) {
    std::array<std::array<int, size_policy / 4>, size_policy / 8> arr =
        lz::generate([]() { return lz::range(static_cast<int>(size_policy / 4)) | lz::to<std::array<int, size_policy / 4>>(); },
                     size_policy / 8) |
        lz::to<std::array<std::array<int, size_policy / 4>, size_policy / 8>>();

    for (auto _ : state) {
        for (int val : lz::flatten(arr)) {
            benchmark::DoNotOptimize(val);
        }
    }
}

void generate_while_lz(benchmark::State& state) {
    for (auto _ : state) {
        std::size_t cnt = 0;
#ifdef LZ_HAS_CXX_17
        for (auto i : lz::generate_while([&cnt]() -> std::pair<bool, std::size_t> {
                 const auto old_value = cnt++;
                 return { cnt, old_value < size_policy };
             })) {
            benchmark::DoNotOptimize(i);
        }
#else
        auto gen = lz::generate_while([&cnt]() -> std::pair<bool, std::size_t> {
            const auto old_value = cnt++;
            return { cnt, old_value < size_policy };
        });
        lz::for_each(gen, [](std::size_t i) { benchmark::DoNotOptimize(i); });
#endif
    }
}

void generate_lz(benchmark::State& state) {
    size_t cnt = 0;

    for (auto _ : state) {
#ifdef LZ_HAS_CXX_17
        for (auto i : lz::generate([&cnt]() noexcept { return cnt++; }, size_policy)) {
            benchmark::DoNotOptimize(i);
        }
#else
        auto gen = lz::generate([&cnt]() noexcept { return cnt++; }, size_policy);
        lz::for_each(gen, [](int i) { benchmark::DoNotOptimize(i); });
#endif
    }
}

void groupy_by_lz(benchmark::State& state) {
    auto arr = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
#ifdef LZ_HAS_CXX_17
        for (auto&& group : lz::group_by(arr, [](int a, int b) noexcept { return a == b; })) {
            benchmark::DoNotOptimize(group.first);
            for (auto&& pair : group.second) {
                benchmark::DoNotOptimize(pair);
            }
        }
#else
        auto group_by = lz::group_by(arr, [](int a, int b) noexcept { return a == b; });
        using ref = lz::ref_iterable_t<decltype(group_by)>;
        lz::for_each(group_by, [](ref group) {
            benchmark::DoNotOptimize(group.first);
            lz::for_each(group.second, [](int i) { benchmark::DoNotOptimize(i); });
        });
#endif
    }
}

void inclusive_scan_lz(benchmark::State& state) {
    auto array = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
#ifdef LZ_HAS_CXX_17
        for (auto i : lz::exclusive_scan(array)) {
            benchmark::DoNotOptimize(i);
        }
#else
        auto scan = lz::inclusive_scan(array);
        lz::for_each(scan, [](int i) { benchmark::DoNotOptimize(i); });
#endif
    }
}

void interleave_lz(benchmark::State& state) {
    std::array<int, size_policy / 2> arr_a =
        lz::range(static_cast<int>(size_policy) / 2) | lz::to<std::array<int, size_policy / 2>>();
    std::array<int, size_policy / 2> arr_b =
        lz::range(static_cast<int>(size_policy) / 2) | lz::to<std::array<int, size_policy / 2>>();

    for (auto _ : state) {
        for (auto i : lz::interleave(arr_a, arr_b)) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void intersection_lz(benchmark::State& state) {
    auto arr_a = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();
    auto arr_b = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
#ifdef LZ_HAS_CXX_17

        for (auto val : lz::intersection(arr_a, arr_b)) {
            benchmark::DoNotOptimize(val);
        }

#else
        auto intersection = lz::intersection(arr_a, arr_b);
        using ref = lz::ref_iterable_t<decltype(intersection)>;
        lz::for_each(intersection, [](ref val) { benchmark::DoNotOptimize(val); });

#endif
    }
}

void join_where_lz(benchmark::State& state) {
    std::vector<int> arr = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    std::vector<int> to_join = { 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 };

    auto random_index = lz::random(static_cast<std::size_t>(0), to_join.size() - 1, size_policy / 2);
    to_join[*random_index.begin()] = arr[*random_index.begin()]; // Create a value where both values are equal
    std::sort(to_join.begin(), to_join.end());

    for (auto _ : state) {
#ifdef LZ_HAS_CXX_17

        for (std::tuple<int, int> val : lz::join_where(
                 arr, to_join, [](int i) noexcept { return i; }, [](int i) noexcept { return i; },
                 [](int a, int b) noexcept { return std::make_tuple(a, b); })) {
            benchmark::DoNotOptimize(val);
        }

#else
        auto join_where = lz::join_where(
            arr, to_join, [](int i) noexcept { return i; }, [](int i) noexcept { return i; },
            [](int a, int b) noexcept { return std::make_tuple(a, b); });
        lz::for_each(join_where, [](std::tuple<int, int> val) { benchmark::DoNotOptimize(val); });

#endif
    }
}

void loop_lz(benchmark::State& state) {
    std::array<int, size_policy / 2> arr =
        lz::range(static_cast<int>(size_policy) / 2) | lz::to<std::array<int, size_policy / 2>>();

    for (auto _ : state) {
        for (int i : lz::loop(arr, 2)) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void map_lz(benchmark::State& state) {
    auto arr = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (int mapped : lz::map(arr, [](const int i) noexcept { return i == 0 ? 10 : 5; })) {
            benchmark::DoNotOptimize(mapped);
        }
    }
}

void random_lz(benchmark::State& state) {
    for (auto _ : state) {
#ifdef LZ_HAS_CXX_17
        for (int i : lz::random(0u, 32u, size_policy)) {
            benchmark::DoNotOptimize(i);
        }
#else
        auto random = lz::random(0u, 32u, size_policy);
        lz::for_each(random, [](int i) { benchmark::DoNotOptimize(i); });
#endif
    }
}

void common_random_lz(benchmark::State& state) {
    for (auto _ : state) {
#ifdef LZ_HAS_CXX_17
        for (int i : lz::common_random(0u, 32u, size_policy)) {
            benchmark::DoNotOptimize(i);
        }
#else
        auto random = lz::common_random(0u, 32u, size_policy);
        lz::for_each(random, [](int i) { benchmark::DoNotOptimize(i); });
#endif
    }
}

void range_lz(benchmark::State& state) {
    for (auto _ : state) {
        for (int i : lz::range(static_cast<int>(size_policy))) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void std_regex_split_lz(benchmark::State& state) {
    std::string to_split = "hello hello hello hello hello he";
    std::regex r(" ");

    for (auto _ : state) {
#ifdef LZ_HAS_CXX_17
        for (lz::string_view substring : lz::regex_split(to_split, r)) {
            benchmark::DoNotOptimize(substring);
        }
#else
        auto split = lz::regex_split(to_split, r);
        lz::for_each(split, [](lz::string_view substring) { benchmark::DoNotOptimize(substring); });
#endif
    }
}

void repeat_lz(benchmark::State& state) {
    for (auto _ : state) {
#ifdef LZ_HAS_CXX_17
        for (int r : lz::repeat(0, size_policy)) {
            benchmark::DoNotOptimize(r);
        }
#else
        auto repeat = lz::repeat(0, size_policy);
        lz::for_each(repeat, [](int i) { benchmark::DoNotOptimize(i); });
#endif
    }
}

void rotate_lz(benchmark::State& state) {
    auto arr = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (int i : lz::rotate(arr, 5)) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void slice_lz(benchmark::State& state) {
    auto arr = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (int i : lz::slice(arr, 0, size_policy)) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void split_multiple_lz(benchmark::State& state) {
    std::string to_split = "hello hello hello hello hello he";

    for (auto _ : state) {
        using ref = lz::ref_iterable_t<decltype(lz::split(to_split, "o "))>;
#ifdef LZ_HAS_CXX_17
        for (ref substring : lz::split(to_split, "o ")) {
            benchmark::DoNotOptimize(substring);
        }
#else
        auto split = lz::split(to_split, "o ");
        lz::for_each(split, [](ref substring) { benchmark::DoNotOptimize(substring); });
#endif
    }
}

void split_single_lz(benchmark::State& state) {
    std::string to_split = "hello hello hello hello hello he";

    for (auto _ : state) {
        using ref = lz::ref_iterable_t<decltype(lz::split(to_split, ' '))>;
#ifdef LZ_HAS_CXX_17
        for (ref substring : lz::split(to_split, ' ')) {
            benchmark::DoNotOptimize(substring);
        }
#else
        auto split = lz::split(to_split, ' ');
        lz::for_each(split, [](ref substring) { benchmark::DoNotOptimize(substring); });
#endif
    }
}

void take_every_lz(benchmark::State& state) {
    constexpr size_t offset = 2;
    std::array<int, size_policy * offset> array;

    for (auto _ : state) {
        for (int taken : lz::take_every(array, offset)) {
            benchmark::DoNotOptimize(taken);
        }
    }
}

#if defined(__cpp_lib_ranges_stride) && LZ_HAS_INCLUDE(<ranges>) && CMAKE_CXX_STANDARD >= 23

void take_every_std(benchmark::State& state) {
    constexpr size_t offset = 2;
    std::array<int, size_policy * offset> array;

    for (auto _ : state) {
        for (int taken : lz::ranges::stride(array, offset)) {
            benchmark::DoNotOptimize(taken);
        }
    }
}

#endif

void take_while_lz(benchmark::State& state) {
    auto array = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (int taken : lz::take_while(array, [](const int i) noexcept { return i != size_policy - 1; })) {
            benchmark::DoNotOptimize(taken);
        }
    }
}

void take_iterable_lz(benchmark::State& state) {
    auto array = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (int taken : lz::take(array, size_policy)) {
            benchmark::DoNotOptimize(taken);
        }
    }
}

void take_iterator_lz(benchmark::State& state) {
    auto array = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (int taken : lz::take(array.begin(), size_policy)) {
            benchmark::DoNotOptimize(taken);
        }
    }
}

void unique_lz(benchmark::State& state) {
    auto arr = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (char c : lz::unique(arr)) {
            benchmark::DoNotOptimize(c);
        }
    }
}

void zip4_lz(benchmark::State& state) {
    auto arr_a = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();
    auto arr_b = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();
    auto arr_c = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();
    auto arr_d = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (auto tuple : lz::zip(arr_a, arr_b, arr_c, arr_d)) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

void zip3_lz(benchmark::State& state) {
    auto arr_a = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();
    auto arr_b = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();
    auto arr_c = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (auto tuple : lz::zip(arr_a, arr_b, arr_c)) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

void zip2_lz(benchmark::State& state) {
    auto arr_a = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();
    auto arr_b = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (auto tuple : lz::zip(arr_a, arr_b)) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

void zip_longest4_lz(benchmark::State& state) {
    auto arr_a = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();
    auto arr_b = lz::range(static_cast<int>(size_policy) - 1) | lz::to<std::array<int, size_policy - 1>>();
    auto arr_c = lz::range(static_cast<int>(size_policy) - 2) | lz::to<std::array<int, size_policy - 2>>();
    auto arr_d = lz::range(static_cast<int>(size_policy) - 3) | lz::to<std::array<int, size_policy - 3>>();

    for (auto _ : state) {
        for (auto tuple : lz::zip_longest(arr_a, arr_b, arr_c, arr_d)) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

void zip_longest3_lz(benchmark::State& state) {
    auto arr_a = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();
    auto arr_b = lz::range(static_cast<int>(size_policy) - 1) | lz::to<std::array<int, size_policy - 1>>();
    auto arr_c = lz::range(static_cast<int>(size_policy) - 2) | lz::to<std::array<int, size_policy - 2>>();

    for (auto _ : state) {
        for (auto tuple : lz::zip_longest(arr_a, arr_b, arr_c)) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

void zip_longest2_lz(benchmark::State& state) {
    auto arr_a = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();
    auto arr_b = lz::range(static_cast<int>(size_policy) - 1) | lz::to<std::array<int, size_policy - 1>>();

    for (auto _ : state) {
        for (auto tuple : lz::zip_longest(arr_a, arr_b)) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

#if LZ_HAS_INCLUDE(<ranges>) && CMAKE_CXX_STANDARD >= 20

void common_std(benchmark::State& state) {
    auto a = lz::generate([]() noexcept { return 0; }, size_policy);

    for (auto _ : state) {
        for (int i : std::views::common(a)) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void filter_std(benchmark::State& state) {
    auto arr = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (int filtered : std::views::filter(arr, [](const int i) noexcept { return i == 0; })) {
            benchmark::DoNotOptimize(filtered);
        }
    }
}

void flatten_std(benchmark::State& state) {
    std::array<std::array<int, size_policy / 4>, size_policy / 8> arr =
        lz::generate([]() { return lz::range(static_cast<int>(size_policy / 4)) | lz::to<std::array<int, size_policy / 4>>(); },
                     size_policy / 8) |
        lz::to<std::array<std::array<int, size_policy / 4>, size_policy / 8>>();

    for (auto _ : state) {
        for (int val : std::views::join(arr)) {
            benchmark::DoNotOptimize(val);
        }
    }
}

void map_std(benchmark::State& state) {
    auto arr = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (int mapped : std::views::transform(arr, [](const int i) noexcept { return i == 0 ? 10 : 5; })) {
            benchmark::DoNotOptimize(mapped);
        }
    }
}

void split_single_std(benchmark::State& state) {
    std::string to_split = "hello hello hello hello hello he";

    for (auto _ : state) {
        for (auto substring : std::views::lazy_split(to_split, ' ')) {
            benchmark::DoNotOptimize(substring);
        }
    }
}

void split_multiple_std(benchmark::State& state) {
    std::string to_split = "hello hello hello hello hello he";

    for (auto _ : state) {
        for (auto substring : std::views::lazy_split(to_split, "o ")) {
            benchmark::DoNotOptimize(substring);
        }
    }
}

#if defined(__cpp_lib_ranges_cartesian_product) && LZ_HAS_INCLUDE(<ranges>) && CMAKE_CXX_STANDARD >= 20

void cartesian_product_std(benchmark::State& state) {
    std::array<int, size_policy / 8> a;
    std::array<char, size_policy / 4> b;

    for (auto _ : state) {
        for (auto&& tup : std::views::cartesian_product(a, b)) {
            benchmark::DoNotOptimize(tup);
        }
    }
}

#endif

#if defined(__cpp_lib_ranges_repeat) && LZ_HAS_INCLUDE(<ranges>) && CMAKE_CXX_STANDARD >= 20

void repeat_std(benchmark::State& state) {
    for (auto _ : state) {
        for (int r : std::views::repeat(0, size_policy)) {
            benchmark::DoNotOptimize(r);
        }
    }
}

#endif

void take_iterable_std(benchmark::State& state) {
    auto array = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (int taken : std::views::take(array, size_policy)) {
            benchmark::DoNotOptimize(taken);
        }
    }
}

void take_iterator_std(benchmark::State& state) {
    auto array = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (int taken : std::views::counted(array.begin(), size_policy)) {
            benchmark::DoNotOptimize(taken);
        }
    }
}

#if defined(__cpp_lib_ranges_zip) && CMAKE_CXX_STANDARD >= 20

void zip4_std(benchmark::State& state) {
    auto arr_a = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();
    auto arr_b = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();
    auto arr_c = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();
    auto arr_d = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (auto tuple : std::views::zip(arr_a, arr_b, arr_c, arr_d)) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

void zip3_std(benchmark::State& state) {
    auto arr_a = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();
    auto arr_b = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();
    auto arr_c = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (auto tuple : std::views::zip(arr_a, arr_b, arr_c)) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

void zip2_std(benchmark::State& state) {
    auto arr_a = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();
    auto arr_b = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (auto tuple : std::views::zip(arr_a, arr_b)) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

#endif

#endif

} // namespace

BENCHMARK(any_iterable_lz);
BENCHMARK(c_string_lz);
BENCHMARK(cached_reverse_lz);
BENCHMARK(cartesian_product_lz);
BENCHMARK(chunk_if_lz);
BENCHMARK(chunks_lz);
BENCHMARK(common_lz);
BENCHMARK(concatenate_lz);
BENCHMARK(duplicates_lz);
BENCHMARK(enumerate_lz);
BENCHMARK(except_lz);
BENCHMARK(exclude_lz);
BENCHMARK(exclusive_scan_lz);
BENCHMARK(filter_lz);
BENCHMARK(flatten_lz);
BENCHMARK(generate_while_lz);
BENCHMARK(generate_lz);
BENCHMARK(groupy_by_lz);
BENCHMARK(inclusive_scan_lz);
BENCHMARK(interleave_lz);
BENCHMARK(intersection_lz);
BENCHMARK(join_where_lz);
BENCHMARK(loop_lz);
BENCHMARK(map_lz);
BENCHMARK(random_lz);
BENCHMARK(common_random_lz);
BENCHMARK(range_lz);
BENCHMARK(std_regex_split_lz);
BENCHMARK(repeat_lz);
BENCHMARK(rotate_lz);
BENCHMARK(slice_lz);
BENCHMARK(split_multiple_lz);
BENCHMARK(split_single_lz);
BENCHMARK(take_every_lz);
BENCHMARK(take_while_lz);
BENCHMARK(take_iterable_lz);
BENCHMARK(take_iterator_lz);
BENCHMARK(unique_lz);
BENCHMARK(zip_longest4_lz);
BENCHMARK(zip_longest3_lz);
BENCHMARK(zip_longest2_lz);
BENCHMARK(zip4_lz);
BENCHMARK(zip3_lz);
BENCHMARK(zip2_lz);

#if defined(__cpp_lib_ranges_cartesian_product) && LZ_HAS_INCLUDE(<ranges>) && CMAKE_CXX_STANDARD >= 23

BENCHMARK(cartesian_product_std);

#endif

#if defined(__cpp_lib_ranges_chunk) && LZ_HAS_INCLUDE(<ranges>) && CMAKE_CXX_STANDARD >= 23

BENCHMARK(chunks_std);

#endif

#if defined(__cpp_lib_ranges_chunk_by) && LZ_HAS_INCLUDE(<ranges>) && CMAKE_CXX_STANDARD >= 23

BENCHMARK(chunk_if_std);

#endif

#if LZ_HAS_INCLUDE(<ranges>) && CMAKE_CXX_STANDARD >= 20

BENCHMARK(common_std);
BENCHMARK(filter_std);
BENCHMARK(flatten_std);
BENCHMARK(take_iterable_std);
BENCHMARK(take_iterator_std);
BENCHMARK(map_std);
BENCHMARK(split_single_std);
BENCHMARK(split_multiple_std);

#endif

#if defined(__cpp_lib_ranges_stride) && LZ_HAS_INCLUDE(<ranges>) && CMAKE_CXX_STANDARD >= 23

BENCHMARK(take_every_std);

#endif

#if defined(__cpp_lib_ranges_repeat) && LZ_HAS_INCLUDE(<ranges>) && CMAKE_CXX_STANDARD >= 23

BENCHMARK(repeat_std);

#endif

#if defined(__cpp_lib_ranges_zip) && LZ_HAS_INCLUDE(<ranges>) && CMAKE_CXX_STANDARD >= 23

BENCHMARK(zip4_std);
BENCHMARK(zip3_std);
BENCHMARK(zip2_std);

#endif

BENCHMARK_MAIN();
