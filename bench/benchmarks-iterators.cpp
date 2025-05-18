#include <Lz/lz.hpp>
#include <array>
#include <benchmark/benchmark.h>

namespace {
constexpr std::size_t size_policy = 32;

void any_iterable(benchmark::State& state) {
    std::array<int, size_policy> arr;

    for (auto _ : state) {
        for (int i : lz::make_any_iterable(arr)) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void c_string(benchmark::State& state) {
    constexpr const char* str = "this is a 32 char long stringggg";
    if (std::strlen(str) != size_policy) {
        throw std::runtime_error(fmt::format("String is not {} characters long", size_policy));
    }

    for (auto _ : state) {
#ifdef LZ_HAS_CXX_17
        for (char c : lz::c_string(str)) {
            benchmark::DoNotOptimize(c);
        }
#else
        auto c_str = lz::c_string(str);
        c_str.for_each([](char c) { benchmark::DoNotOptimize(c); });
#endif
    }
}

void cartesian_product(benchmark::State& state) {
    std::array<int, size_policy / 8> a;
    std::array<char, size_policy / 4> b;

    for (auto _ : state) {
        for (auto&& tup : lz::cartesian_product(a, b)) {
            benchmark::DoNotOptimize(tup);
        }
    }
}

void chunk_if(benchmark::State& state) {
    std::array<int, size_policy> a = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();
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
        chunk_if.for_each([](ref chunk) {
            for (int x : chunk) {
                benchmark::DoNotOptimize(x);
            }
        });
#endif
    }
}

void chunks(benchmark::State& state) {
    std::array<int, size_policy> a = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();
    for (auto _ : state) {
        for (auto&& chunk : lz::chunks(a, 8)) {
            for (int x : chunk) {
                benchmark::DoNotOptimize(x);
            }
        }
    }
}

void common(benchmark::State& state) {
    auto a = lz::generate([]() noexcept { return 0; }, size_policy);

    for (auto _ : state) {
        for (int i : lz::common(a)) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void concatenate(benchmark::State& state) {
    std::string a(size_policy / 2, '0');
    std::string b(size_policy / 2, '1');

    for (auto _ : state) {
        for (char c : lz::concat(a, b)) {
            benchmark::DoNotOptimize(c);
        }
    }
}

void enumerate(benchmark::State& state) {
    std::array<int, size_policy> arr;

    for (auto _ : state) {
        for (auto pair : lz::enumerate(arr)) {
            benchmark::DoNotOptimize(pair);
        }
    }
}

void except(benchmark::State& state) {
    std::array<int, size_policy> large_arr = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();
    std::array<int, size_policy / 2> large_arr_except =
        lz::range(static_cast<int>(size_policy) / 2) | lz::to<std::array<int, size_policy / 2>>();

    for (auto _ : state) {
#ifdef LZ_HAS_CXX_17
        for (auto excepted : lz::except(large_arr, large_arr_except)) {
            benchmark::DoNotOptimize(excepted);
        }
#else
        auto except = lz::except(large_arr, large_arr_except);
        except.for_each([](int i) { benchmark::DoNotOptimize(i); });
#endif
    }
}

void exclude(benchmark::State& state) {
    std::array<int, size_policy> a = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
#ifdef LZ_HAS_CXX_17
        for (int i : lz::exclude(a, 5, 10)) {
            benchmark::DoNotOptimize(i);
        }
#else
        auto exclude = lz::exclude(a, 5, 10);
        exclude.for_each([](int i) { benchmark::DoNotOptimize(i); });
#endif
    }
}

void exclusive_scan(benchmark::State& state) {
    auto array = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
#ifdef LZ_HAS_CXX_17
        for (int i : lz::exclusive_scan(array, 0)) {
            benchmark::DoNotOptimize(i);
        }
#else
        auto scan = lz::exclusive_scan(array, 0);
        scan.for_each([](int i) { benchmark::DoNotOptimize(i); });
#endif
    }
}

void filter(benchmark::State& state) {
    std::array<int, size_policy> arr;

    for (auto _ : state) {
        for (int filtered : lz::filter(arr, [](const int i) noexcept { return i == 0; })) {
            benchmark::DoNotOptimize(filtered);
        }
    }
}

void flatten(benchmark::State& state) {
    std::array<std::array<int, size_policy / 4>, size_policy / 8> arr =
        lz::generate([]() { return lz::range(static_cast<int>(size_policy / 4)) | lz::to<std::array<int, size_policy / 4>>(); },
                     size_policy / 8) | lz::to<std::array<std::array<int, size_policy / 4>, size_policy / 8>>();

    for (auto _ : state) {
        for (int val : lz::flatten(arr)) {
            benchmark::DoNotOptimize(val);
        }
    }
}

void generate_while(benchmark::State& state) {
    for (auto _ : state) {
        std::size_t cnt = 0;
#ifdef LZ_HAS_CXX_17
        for (auto i : lz::generate_while([&cnt]() -> std::pair<bool, std::size_t> {
                 const auto old_value = cnt++;
                 return { old_value < size_policy, cnt };
             })) {
            benchmark::DoNotOptimize(i);
        }
#else
        auto gen = lz::generate_while([&cnt]() -> std::pair<bool, std::size_t> {
            const auto old_value = cnt++;
            return { old_value < size_policy, cnt };
        });
        gen.for_each([](std::size_t i) { benchmark::DoNotOptimize(i); });
#endif
    }
}

void generate(benchmark::State& state) {
    size_t cnt = 0;

    for (auto _ : state) {
#ifdef LZ_HAS_CXX_17
        for (auto i : lz::generate([&cnt]() noexcept { return cnt++; }, size_policy)) {
            benchmark::DoNotOptimize(i);
        }
#else
        auto gen = lz::generate([&cnt]() noexcept { return cnt++; }, size_policy);
        gen.for_each([](int i) { benchmark::DoNotOptimize(i); });
#endif
    }
}

void groupy_by(benchmark::State& state) {
    std::array<int, size_policy> arr = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

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
        group_by.for_each([](ref group) {
            benchmark::DoNotOptimize(group.first);
            group.second.for_each([](int i) { benchmark::DoNotOptimize(i); });
        });
#endif
    }
}

void inclusive_scan(benchmark::State& state) {
    auto array = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
#ifdef LZ_HAS_CXX_17
        for (auto i : lz::exclusive_scan(array)) {
            benchmark::DoNotOptimize(i);
        }
#else
        auto scan = lz::inclusive_scan(array);
        scan.for_each([](int i) { benchmark::DoNotOptimize(i); });
#endif
    }
}

void interleave(benchmark::State& state) {
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

void intersection(benchmark::State& state) {
    std::array<int, size_policy> arr_a = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();
    std::array<int, size_policy> arr_b = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
#ifdef LZ_HAS_CXX_17

        for (auto val : lz::intersection(arr_a, arr_b)) {
            benchmark::DoNotOptimize(val);
        }

#else
        auto intersection = lz::intersection(arr_a, arr_b);
        using ref = lz::ref_iterable_t<decltype(intersection)>;
        intersection.for_each([](ref val) { benchmark::DoNotOptimize(val); });

#endif
    }
}

void join_where(benchmark::State& state) {
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
        join_where.for_each([](std::tuple<int, int> val) { benchmark::DoNotOptimize(val); });

#endif
    }
}

void loop(benchmark::State& state) {
    std::array<int, size_policy / 2> arr =
        lz::range(static_cast<int>(size_policy) / 2) | lz::to<std::array<int, size_policy / 2>>();

    for (auto _ : state) {
        for (int i : lz::loop(arr, 2)) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void map(benchmark::State& state) {
    std::array<int, size_policy> arr = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (int mapped : lz::map(arr, [](const int i) noexcept { return i == 0 ? 10 : 5; })) {
            benchmark::DoNotOptimize(mapped);
        }
    }
}

void random(benchmark::State& state) {
    for (auto _ : state) {
#ifdef LZ_HAS_CXX_17
        for (int i : lz::random(0, 32, size_policy)) {
            benchmark::DoNotOptimize(i);
        }
#else
        auto random = lz::random(0, 32, size_policy);
        random.for_each([](int i) { benchmark::DoNotOptimize(i); });
#endif
    }
}

void common_random(benchmark::State& state) {
    for (auto _ : state) {
#ifdef LZ_HAS_CXX_17
        for (int i : lz::common_random(0, 32, size_policy)) {
            benchmark::DoNotOptimize(i);
        }
#else
        auto random = lz::common_random(0, 32, size_policy);
        random.for_each([](int i) { benchmark::DoNotOptimize(i); });
#endif
    }
}

void range(benchmark::State& state) {
    for (auto _ : state) {
        for (int i : lz::range(static_cast<int>(size_policy))) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void std_regex_split(benchmark::State& state) {
    std::string to_split = "hello hello hello hello hello he";
    std::regex r(" ");

    for (auto _ : state) {
#ifdef LZ_HAS_CXX_17
        for (lz::string_view substring : lz::regex_split(to_split, r)) {
            benchmark::DoNotOptimize(substring);
        }
#else
        auto split = lz::regex_split(to_split, r);
        split.for_each([](lz::string_view substring) { benchmark::DoNotOptimize(substring); });
#endif
    }
}

void repeat(benchmark::State& state) {
    for (auto _ : state) {
#ifdef LZ_HAS_CXX_17
        for (int r : lz::repeat(0, size_policy)) {
            benchmark::DoNotOptimize(r);
        }
#else
        auto repeat = lz::repeat(0, size_policy);
        repeat.for_each([](int i) { benchmark::DoNotOptimize(i); });
#endif
    }
}

void rotate(benchmark::State& state) {
    std::array<int, size_policy> arr = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (int i : lz::rotate(arr, 5)) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void slice(benchmark::State& state) {
    std::array<int, size_policy> arr = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (int i : lz::slice(arr, 0, size_policy)) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void split_multiple(benchmark::State& state) {
    std::string to_split = "hello hello hello hello hello he";

    for (auto _ : state) {
        using ref = lz::ref_iterable_t<decltype(lz::split(to_split, "o "))>;
#ifdef LZ_HAS_CXX_17
        for (ref substring : lz::split(to_split, "o ")) {
            benchmark::DoNotOptimize(substring);
        }
#else
        auto split = lz::split(to_split, "o ");
        split.for_each([](ref substring) { benchmark::DoNotOptimize(substring); });
#endif
    }
}

void split_single(benchmark::State& state) {
    std::string to_split = "hello hello hello hello hello he";

    for (auto _ : state) {
        using ref = lz::ref_iterable_t<decltype(lz::split(to_split, ' '))>;
#ifdef LZ_HAS_CXX_17
        for (ref substring : lz::split(to_split, ' ')) {
            benchmark::DoNotOptimize(substring);
        }
#else
        auto split = lz::split(to_split, ' ');
        split.for_each([](ref substring) { benchmark::DoNotOptimize(substring); });
#endif
    }
}

void take_every(benchmark::State& state) {
    constexpr size_t offset = 2;
    std::array<int, size_policy * offset> array;

    for (auto _ : state) {
        for (int taken : lz::take_every(array, offset)) {
            benchmark::DoNotOptimize(taken);
        }
    }
}

void take_while(benchmark::State& state) {
    std::array<int, size_policy> array = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (int taken : lz::take_while(array, [](const int i) noexcept { return i != size_policy - 1; })) {
            benchmark::DoNotOptimize(taken);
        }
    }
}

void take(benchmark::State& state) {
    std::array<int, size_policy> array = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (int taken : lz::take(array, size_policy)) {
            benchmark::DoNotOptimize(taken);
        }
    }
}

void unique(benchmark::State& state) {
    std::array<int, size_policy> arr = lz::range(static_cast<int>(size_policy)) | lz::to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (char c : lz::unique(arr)) {
            benchmark::DoNotOptimize(c);
        }
    }
}

void zip4(benchmark::State& state) {
    std::array<int, size_policy> arr_a;
    std::array<int, size_policy> arr_b;
    std::array<int, size_policy> arr_c;
    std::array<int, size_policy> arr_d;

    for (auto _ : state) {
        for (auto tuple : lz::zip(arr_a, arr_b, arr_c, arr_d)) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

void zip3(benchmark::State& state) {
    std::array<int, size_policy> arr_a;
    std::array<int, size_policy> arr_b;
    std::array<int, size_policy> arr_c;

    for (auto _ : state) {
        for (auto tuple : lz::zip(arr_a, arr_b, arr_c)) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

void zip2(benchmark::State& state) {
    std::array<int, size_policy> arr_a;
    std::array<int, size_policy> arr_b;

    for (auto _ : state) {
        for (auto tuple : lz::zip(arr_a, arr_b)) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

void zip_longest4(benchmark::State& state) {
    std::array<int, size_policy> arr_a;
    std::array<int, size_policy - 1> arr_b;
    std::array<int, size_policy - 2> arr_c;
    std::array<int, size_policy - 3> arr_d;

    for (auto _ : state) {
        for (auto tuple : lz::zip_longest(arr_a, arr_b, arr_c, arr_d)) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

void zip_longest3(benchmark::State& state) {
    std::array<int, size_policy> arr_a;
    std::array<int, size_policy - 1> arr_b;
    std::array<int, size_policy - 2> arr_c;

    for (auto _ : state) {
        for (auto tuple : lz::zip_longest(arr_a, arr_b, arr_c)) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

void zip_longest2(benchmark::State& state) {
    std::array<int, size_policy> arr_a;
    std::array<int, size_policy - 1> arr_b;

    for (auto _ : state) {
        for (auto tuple : lz::zip_longest(arr_a, arr_b)) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}
} // namespace

// TODO add lz::any_iterable, and other iterators

BENCHMARK(any_iterable);
BENCHMARK(c_string);
BENCHMARK(cartesian_product);
BENCHMARK(chunk_if);
BENCHMARK(chunks);
BENCHMARK(common);
BENCHMARK(concatenate);
BENCHMARK(enumerate);
BENCHMARK(except);
BENCHMARK(exclude);
BENCHMARK(exclusive_scan);
BENCHMARK(filter);
BENCHMARK(flatten);
BENCHMARK(generate_while);
BENCHMARK(generate);
BENCHMARK(groupy_by);
BENCHMARK(inclusive_scan);
BENCHMARK(interleave);
BENCHMARK(intersection);
BENCHMARK(join_where);
BENCHMARK(loop);
BENCHMARK(map);
BENCHMARK(random);
BENCHMARK(common_random);
BENCHMARK(range);
BENCHMARK(std_regex_split);
BENCHMARK(repeat);
BENCHMARK(rotate);
BENCHMARK(slice);
BENCHMARK(split_multiple);
BENCHMARK(split_single);
BENCHMARK(take_every);
BENCHMARK(take_while);
BENCHMARK(take);
BENCHMARK(unique);
BENCHMARK(zip_longest4);
BENCHMARK(zip_longest3);
BENCHMARK(zip_longest2);
BENCHMARK(zip4);
BENCHMARK(zip3);
BENCHMARK(zip2);

BENCHMARK_MAIN();
