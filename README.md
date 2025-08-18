[![Build status](https://github.com/Kaaserne/cpp-lazy/workflows/Continuous%20Integration/badge.svg)](https://github.com/Kaaserne/cpp-lazy/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![CodeQL](https://github.com/Kaaserne/cpp-lazy/actions/workflows/codeql.yml/badge.svg)](https://github.com/Kaaserne/cpp-lazy/actions/workflows/codeql.yml)

![](https://i.ibb.co/ccn2V8N/Screenshot-2021-05-05-Make-A-High-Quality-Logo-In-Just-5-Minutes-For-Under-30-v1-cropped.png)

Examples can be found [here](https://github.com/Kaaserne/cpp-lazy/tree/master/examples). Installation can be found [here](https://github.com/Kaaserne/cpp-lazy#installation).

# cpp-lazy
`cpp-lazy` is an easy and fast lazy evaluation library for C++11/14/17/20. The library tries to reduce redundant data usage for begin/end iterator pairs. For instance: `lz::random_iterable::end()` will return a `lz::default_sentinel_t` to prevent duplicate data that is also present in `lz::random_iterable::begin()`. If a 'symmetrical' end-begin iterator pair is needed, one can use `lz::common` or `lz::common_random`. Generally, `lz` *forward* iterators will return a `lz::default_sentinel_t` (or if the input iterable is sentinelled) because forward iterators can only go forward, so there is no need to store the end iterator, is the philosophy. Lz random access iterators can also return a `default_sentinel` if the internal data of `begin` can already decide whether end is reached, such as `lz::repeat`.

The library uses one optional dependency: the library `{fmt}`, more of which can be found out in the [installation section](https://github.com/Kaaserne/cpp-lazy#Installation). This dependency is only used for printing and formatting.

# Features
- C++11/14/17/20 compatible
- C++20's module compatible
- Easy printing/formatting using `lz::format`, `fmt::print` or `std::cout`
- One optional dependency ([`{fmt}`](https://github.com/fmtlib/fmt)), can be turned off by using option `CPP-LAZY_USE_STANDALONE=TRUE`/`set(CPP-LAZY_USE_STANDALONE TRUE)` in CMake
- STL compatible (if the input iterable is not sentinelled, otherwise use `lz::*` equivalents)
- Little overhead, as little data usage as possible
- Any compiler with at least C++11 support should be suitable
- [Easy installation](https://github.com/Kaaserne/cpp-lazy#installation)
- [Clear Examples](https://github.com/Kaaserne/cpp-lazy/tree/master/examples)
- Piping/chaining using `|` operator
<details>
<summary>Tested with GCC's warning flags</summary>

```
-Wpedantic -Wextra -Wall -Wshadow -Werror -Wconversion -Wcast-qual -Wcomma-subscript -Wctor-dtor-privacy -Wdeprecated-copy-dtor -Wdouble-promotion -Wduplicated-branches -Wduplicated-cond -Wenum-conversion -Wextra-semi -Wfloat-equal -Wformat-overflow=2 -Wformat-signedness -Wformat=2 -Wlogical-op -Wmismatched-tags -Wmultichar -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wrange-loop-construct -Wrestrict -Wstrict-null-sentinel -Wsuggest-attribute=format -Wsuggest-attribute=malloc -Wuninitialized -Wvla -Wvolatile -Wwrite-strings
```

</details>

# What is lazy?
Lazy evaluation is an evaluation strategy which holds the evaluation of an expression until its value is needed. In this library, all the iterators are lazy evaluated. Suppose you want to have a sequence of `n` random numbers. You could write a for loop:

```cpp
std::random_device rd;
std::std::mt19937 gen(rd());
std::uniform_int_distribution dist(0, 32);

for (int i = 0; i < n; i++) {
 std::cout << dist(gen); // prints a random number n times, between [0, 32]
}
```

This is actually exactly almost identical as:
```cpp
// If standalone:
std::cout << lz::random(0, 32, n);

// If with fmt:
fmt::print("{}", lz::random(0, 32, n));
```

Both methods do not allocate any memory but the second example is a much more convenient way of writing the same thing. Now what if you wanted to do eager evaluation? Well then you could do this:

```cpp
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution dist(0, 32);
std::vector<int> random_numbers;
std::generate(random_numbers.begin(), random_numbers.end(), [&dist, &gen]{ return dist(gen); });
```

or, using `cpp-lazy`:

```cpp
std::vector<int> random_numbers = lz::random(0, 32, n) | lz::to<std::vector>();
```

`cpp-lazy` is also (semi) compatible with the STL `<algorithm>` library. This means that you can use `std::find`, `std::find_if`, etc. on `lz` iterables, as long as the input iterable is not sentinelled. If the input iterable is sentinelled, you can use `lz::find`, `lz::find_if`, etc instead. `lz` algorithm equivalents will try to use `std::*` equivalents if possible.

```cpp
auto random = lz::random(0, 32, n);
// Calls lz::find
auto pos = lz::find(random, 6) != random.end();

auto common = lz::common_random(0, 32, n);
// Calls std::find
auto pos = lz::find(common, 6) != common.end();
// or (exactly the same as above)
auto pos = std::find(common.begin(), common.end(), 6) != common.end();
```

## Basic usage
```cpp
#include <Lz/map.hpp>
#include <vector>

int main() {
  std::array<int, 4> arr = {1, 2, 3, 4};
  auto result = lz::map(arr, [](int i) { return i + 1; }) 
                       | lz::to<std::vector>(); // == {2, 3, 4, 5}
  // or
  auto result = arr | lz::map([](int i) { return i + 1; })
                    | lz::to<std::vector>(); // == {2, 3, 4, 5}

  // Some iterables will return sentinels, for instance (specific rules about when sentinels are returned can be found in the documentation):
  std::vector<int> vec = {1, 2, 3, 4};
  auto forward = lz::c_string("Hello World"); // .end() returns default_sentinel_t
  auto inf_loop = lz::loop(vec); // .end() returns default_sentinel_t
  auto random = lz::random(0, 32, 4); // .end() returns default_sentinel_t

  // Some iterables are sized, if the input iterable is also sized:
  auto sized = lz::map(vec, [](int i) { return i + 1; });
  auto size = sized.size(); // == 4
  // forward.size(); // error: forward is not sized, instead use (O(n) time): lz::eager_size(forward)

  // Some iterables require the size of iterables. If the input iterable is not sized,
  // the sequence will be traversed to get the size using lz::eager_size. The iterable will be documented
  // appropriately if this requires a sized iterable. Example:
  auto zipper1 = lz::zip(lz::c_string("ab"), lz::c_string("cd")); // Calling .end() will take O(n) time
  auto zipper2 = lz::zip(sized, sized); // Calling .end() will take O(1) time

  // You can cache the size if you need to.
  // More about when to do that in the appropriate iterable documentation
  // lz:c_string isn't sized, so in order to make it sized, we can use lz::cache_size:
  auto cached_sizes = lz::zip(lz::c_string("ab") | lz::cache_size, lz::c_string("cd") | lz::cache_size);
  // cached_sizes.end() will now take O(1) time, instead of zipper1's O(n) time.
}
```

## Ownership
`lz` iterables will hold a reference to the input iterable if the input iterable is *not* inherited from `lz::lazy_view`. This means that the `lz` iterables will hold a reference to (but not excluded to) containers such as `std::vector`, `std::array` and `std::string`, as they do not inherit from `lz::lazy_view`. This is done by the class `lz::maybe_owned`. This can be altered using `lz::copied` or `lz::as_copied`. This will copy the input iterable instead of holding a reference to it. This is useful for cheap to copy iterables that are not inherited from `lz::lazy_view` (for example `boost::iterator_range`).

```cpp
#include <Lz/lz.hpp>

struct non_lz_iterable {
  int* _begin{};
  int* _end{};

  non_lz_iterable(int* begin, int* end) : _begin{ begin }, _end{ end } {
  }

  int* begin() {
    return _begin;
  }
  int* end() {
    return _end;
  }
};

int main() {
  std::vector<int> vec = {1, 2, 3, 4};
  // mapped will hold a reference to vec
  auto mapped = lz::map(vec, [](int i) { return i + 1; });
  // filtered does NOT hold a reference to mapped, but mapped still holds a reference to vec
  auto filtered = lz::filter(mapped, [](int i) { return i % 2 == 0; });

  auto random = lz::random(0, 32, 4);
  // str will *not* hold a reference to random, because random is a lazy iterable and is trivial to copy
  auto str = lz::map(random, [](int i) { return std::to_string(i); });

  lz::maybe_owned<std::vector<int>> ref(vec); // Holds a reference to vec

  using random_iterable = decltype(random);
  lz::maybe_owned<random_iterable> ref2(random); // Does NOT hold a reference to random

  non_lz_iterable non_lz(vec.data(), vec.data() + vec.size());
  lz::maybe_owned<non_lz_iterable> ref(non_lz); // Holds a reference of non_lz! Watch out for this!

  // Instead, if you don't want this behaviour, you can use `lz::copied`:
  lz::copied<non_lz_iterable> copied(non_lz); // Holds a copy of non_lz = cheap to copy
  // Or use the helper function:
  copied = lz::as_copied(non_lz); // Holds a copy of non_lz = cheap to copy
}
```

## Iterating
Iterating over iterables with sentinels using range-based for loops is possible. However, a workaround for C++ versions < 17 is needed.

```cpp
#include <Lz/c_string.hpp>
#include <Lz/algorithm.hpp>

int main() {
  auto iterable_with_sentinel = lz::c_string("Hello World");
  // Possible in C++17 and higher
  for (auto i : iterable_with_sentinel) {
    std::cout << i; // prints "Hello World"
  }

  // Possible in C++11 - 14
  lz::for_each(iterable_with_sentinel, [](char i) { std::cout << i; }); // prints "Hello World"
}
```

## Formatting
Formatting is done using `{fmt}` or `<format>`. If neither is available, it will use `std::cout`/`std::ostringstream`:

```cpp
#include <Lz/stream.hpp>
#include <Lz/filter.hpp>
#include <vector>

int main() {
  std::vector<int> vec = {1, 2, 3, 4};
  auto filtered = vec | lz::filter([](int i) { return i % 2 == 0; }); // == {2, 4}

  // To a stream
  std::cout << filtered; // prints "2 4" (only works for lz iterables)
  lz::format(filtered, std::cout, ", ", "{:02d}"); // prints "02, 04" (only with {fmt} installed or C++20's <format>)
  lz::format(filtered, std::cout, ", "); // prints "2, 4"
  fmt::print("{}", fmt::join(filtered, ", ")); // prints "2, 4" (only with {fmt} installed)

  filtered | lz::format(std::cout, ", "); // prints "2, 4"
  filtered | lz::format; // prints "2, 4"
  filtered | lz::format(std::cout, ", ", "{:02d}"); // prints "02, 04" (only with {fmt} installed or C++20's <format>)
}
```

# Installation

## With CMake
### Options
The following CMake options are available:
- `CPP-LAZY_USE_STANDALONE`: Use the standalone version of cpp-lazy. This will not use the library `{fmt}`. Default is `FALSE`
- `CPP-LAZY_LZ_USE_MODULES` (experimental): Use C++20 modules. Default is `FALSE`
- `CPP-LAZY_USE_INSTALLED_FMT`: Use the installed version of `{fmt}`. This will not use the bundled version. Will use `find_package(fmt)` if enabled. Default is `FALSE`.
- `CPP-LAZY_DEBUG_ASSERTIONS`: Enable debug assertions. Default is `TRUE` for debug mode, `FALSE` for release.

### Using `FetchContent`
The following way is recommended (cpp-lazy version >= 5.0.1). Note that you choose the cpp-lazy-src.zip, and not the source-code.zip/source-code.tar.gz. This prevents you from downloading stuff that you don't need, and thus preventing pollution of the cmake build directory:
```cmake

# Uncomment this line to use the cpp-lazy standalone version or use -D CPP-LAZY_USE_STANDALONE=TRUE
# set(CPP-LAZY_USE_STANDALONE TRUE)

include(FetchContent)
FetchContent_Declare(cpp-lazy
        URL https://github.com/Kaaserne/cpp-lazy/releases/download/<TAG_HERE E.G. v9.0.0>/cpp-lazy-src.zip
        # Below is optional
        # URL_MD5 <MD5 HASH OF cpp-lazy-src.zip>
        # If using CMake >= 3.24, preferably set <bool> to TRUE
        # DOWNLOAD_EXTRACT_TIMESTAMP <bool>
)
FetchContent_MakeAvailable(cpp-lazy)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} cpp-lazy::cpp-lazy)
```

An alternative ('less' recommended), add to your `CMakeLists.txt` the following:
```cmake
# Uncomment this line to use the cpp-lazy standalone version
# set(CPP-LAZY_USE_STANDALONE TRUE)

include(FetchContent)
FetchContent_Declare(cpp-lazy
        GIT_REPOSITORY https://github.com/Kaaserne/cpp-lazy
        GIT_TAG ... # Commit hash
        # If using CMake >= 3.24, preferably set <bool> to TRUE
        # DOWNLOAD_EXTRACT_TIMESTAMP <bool>
)
FetchContent_MakeAvailable(cpp-lazy)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} cpp-lazy::cpp-lazy)
```

## With xmake
Everything higher than version 7.0.2 is supported. Please note that version 9.0.0 has drastic changes in the API (PascalCase/camelCase -> snake_case).
```xmake
add_requires("cpp-lazy >=9.0.0")

target("test")
    add_packages("cpp-lazy")
```

## Without CMake
### Without `{fmt}`
- Clone the repository
- Specify the include directory to `cpp-lazy/include`.
- Include files as follows:

```cpp
// Important, preprocessor macro 'LZ_STANDALONE' has to be defined already
// or
// #define LZ_STANDALONE
#include <Lz/map.hpp>
#include <vector>

int main() {
  std::array<int, 4> arr = {1, 2, 3, 4};
  auto result = lz::map(arr, [](int i) { return i + 1; }) | lz::to<std::vector>(); // == {2, 3, 4, 5}
  // or
  auto result = lz::to<std::vector>(arr | lz::map([](int i) { return i + 1; })); // == {2, 3, 4, 5}
}
```

### With `{fmt}`
- Clone the repository
- Specify the include directory to `cpp-lazy/include` and `fmt/include`.
- Define `FMT_HEADER_ONLY` before including any `lz` files.
- Include files as follows:

```cpp
#define FMT_HEADER_ONLY

#include <Lz/map.hpp>
#include <vector>

int main() {
  std::array<int, 4> arr = {1, 2, 3, 4};
  auto result = lz::map(arr, [](int i) { return i + 1; }) | lz::to<std::vector>(); // == {2, 3, 4, 5}
  // or
  auto result = lz::to<std::vector>(arr | lz::map([](int i) { return i + 1; })); // == {2, 3, 4, 5}
}
```

### Using `git clone`
Clone the repository using `git clone https://github.com/Kaaserne/cpp-lazy/` and add to `CMakeLists.txt` the following:
```cmake
add_subdirectory(cpp-lazy)
add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} cpp-lazy::cpp-lazy)
```

# Benchmarks
The time is equal to one iteration. One iteration includes the creation of the iterable and one iteration of that iterable. Compiled with: gcc version 13.3.0.

<div style="text-align:center"><img src="https://raw.githubusercontent.com/Kaaserne/cpp-lazy/master/bench/benchmarks-iterators-C%2B%2B23.png" /></div>
