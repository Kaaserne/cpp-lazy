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
- `pkgconfig` compatible
- Easy printing/formatting using `lz::format`, `fmt::print` or `std::cout`
- One optional dependency ([`{fmt}`](https://github.com/fmtlib/fmt)), can be turned off by using option `CPP_LAZY_USE_STANDALONE=TRUE`/`set(CPP_LAZY_USE_STANDALONE TRUE)` in CMake
- STL compatible (if the input iterable is not sentinelled, otherwise use `lz::*` equivalents)
- Little overhead, as little data usage as possible
- Any compiler with at least C++11 support should be suitable
- [Easy installation](https://github.com/Kaaserne/cpp-lazy#installation)
- [Clear Examples](https://github.com/Kaaserne/cpp-lazy/tree/master/examples)
- Piping/chaining using `|` operator
- [Tested with very strict GCC/Clang/MSVC flags](https://github.com/Kaaserne/cpp-lazy/blob/11a1fd957cba49c57b9bbca7e69f53bd7210e323/tests/CMakeLists.txt#L87)
- Bidirectional sentinelled iterables can be reversed using `lz::common`

# What is lazy?
Lazy evaluation is an evaluation strategy which holds the evaluation of an expression until its value is needed. In this library, this is the case for all iterables/iteartors. It holds all the elements that are needed for the operation:
```cpp
std::vector<int> vec = {1, 2, 3, 4, 5};

// No evaluation is done here, function is stored and a reference to vec
auto mapped = lz::map(vec, [](int i) { return i * 2; });
for (auto i : mapped) { // Evaluation is done here
  std::cout << i << " "; // prints "2 4 6 8 10 "
}
```


# Basic usage
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

  // Some iterables will return sentinels, for instance:
  // (specific rules about when sentinels are returned can be found in the documentation):
  std::vector<int> vec = {1, 2, 3, 4};
  auto forward = lz::c_string("Hello World"); // .end() returns default_sentinel_t

  // inf_loop = {1, 2, 3, 4, 1, 2, 3, 4, ...}
  auto inf_loop = lz::loop(vec); // .end() returns default_sentinel_t

  // random = {random number between 0 and 32, total of 4 numbers}
  auto random = lz::random(0, 32, 4); // .end() returns default_sentinel_t
}
```

## Philosophy behind cpp-lazy
Cpp-lazy is implemented in such a way, that it tries to reduce redundant data usage as much as possible. For instance, if an iterable can determine whether the end is reached by only using the begin iterator ánd is less than a bidirectional iterator, it will return a sentinel instead of storing the end iterator as well. Or, if the iterable is already sentinelled, it will return the same sentinel type as the input iterable.

This is the case for `lz::take(std::forward_list<>{})` for instance, but not for `lz::take(std::list<>{})` or `lz::take(std::vector<>{})`. This is done to reduce memory usage. If you need a symmetrical `begin`/`end` iterator pair, you can use `lz::common`. Use `lz::common` as late as possible in your chain, because every `lz` iterable has the potential to return sentinels instead of storing the `end` iterator. 

```cpp
a | lz::map(...) | lz::filter(...) | lz::common; // OK
a | lz::common | lz::map(...) | lz::filter(...); // Not recommended, map or filter could have returned sentinels
```

`lz::common` tries to make the `begin`/`end` iterator pair symmetrical. Only as last resort it will use `common_iterator<I, S>` which is basically a fancy `[std::]variant` internally. It depends on the following conditions whether `lz::common` will use `common_iterator`:
- If `begin` and `end` are already the same type, the same instance will be returned;
- If the iterable passed is random access, `begin + (size_t)distance(begin, end)` will be used to create the `end` iterator;
- If `begin` is assignable from `end`, assign `end` to `begin` to create the `end` iterator. It will keep its `.size()` if applicable;
- Otherwise, `common_iterator<I, S>` will be used. As will be the case for all `std::` sentinelled iterables, but not for `lz` sentinelled iterables.

## Ownership
`lz` iterables will hold a reference to the input iterable if the input iterable is *not* inherited from `lz::lazy_view`. This means that the `lz` iterables will hold a reference to (but not excluded to) containers such as `std::vector`, `std::array` and `std::string`, as they do not inherit from `lz::lazy_view`. This is done by the class `lz::maybe_owned`. This can be altered using `lz::copied` or `lz::as_copied`. This will copy the input iterable instead of holding a reference to it. This is useful for cheap to copy iterables that are not inherited from `lz::lazy_view` (for example `boost::iterator_range`).

```cpp
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

  lz::maybe_owned<std::vector<int>> ref{ vec }; // Holds a reference to vec
  lz::maybe_owned<std::vector<int>>::holds_reference; // true

  using random_iterable = decltype(random);
  lz::maybe_owned<random_iterable> ref2{ random }; // Does NOT hold a reference to random
  lz::maybe_owned<random_iterable>::holds_reference; // false

  non_lz_iterable non_lz(vec.data(), vec.data() + vec.size());
  lz::maybe_owned<non_lz_iterable> ref{ non_lz }; // Holds a reference of non_lz! Watch out for this!
  lz::maybe_owned<non_lz_iterable>::holds_reference; // true

  // Instead, if you don't want this behaviour, you can use `lz::copied`:
  lz::copied<non_lz_iterable> copied{ non_lz }; // Holds a copy of non_lz = cheap to copy
  lz::copied<non_lz_iterable>::holds_reference; // false
  // Or use the helper function:
  copied = lz::as_copied(non_lz); // Holds a copy of non_lz = cheap to copy
}
```

## Iterating
Iterating over iterables with sentinels using range-based for loops is possible. However, a workaround for C++ versions < 17 is needed.

```cpp
#include <Lz/c_string.hpp>
#include <Lz/algorithm/algorithm.hpp>

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
## Options
The following CMake options are available, all of which are optional:
- `CPP_LAZY_USE_STANDALONE`: Use the standalone version of cpp-lazy. This will not use the library `{fmt}`. Default is `FALSE`
- `CPP_LAZY_LZ_USE_MODULES`: Use C++20 modules. Default is `FALSE`
- `CPP_LAZY_DEBUG_ASSERTIONS`: Enable debug assertions. Default is `TRUE` for debug mode, `FALSE` for release.
- `CPP_LAZY_USE_INSTALLED_FMT`: Use the system installed version of `{fmt}`. This will not use the bundled version. Default is `FALSE` and uses `find_package`. Use `CPP_LAZY_FMT_DEP_VERSION` to specify the version of `{fmt}` to use if needed.
- `CPP_LAZY_INSTALL`: Install cpp-lazy targets and config files. Default is `FALSE`.
- `CPP_LAZY_FMT_DEP_VERSION`: version of `{fmt}` to use. Used if `CPP_LAZY_USE_INSTALLED_FMT` is `TRUE` or `CPP_LAZY_USE_STANDALONE` is `FALSE`. May be empty.

### Using `FetchContent`
The following way is recommended (cpp-lazy version >= 5.0.1). Note that you choose the cpp-lazy-src.zip, and not the source-code.zip/source-code.tar.gz. This prevents you from downloading stuff that you don't need, and thus preventing pollution of the cmake build directory:
```cmake

# Uncomment this line to use the cpp-lazy standalone version or use -D CPP_LAZY_USE_STANDALONE=TRUE
# set(CPP_LAZY_USE_STANDALONE TRUE)

include(FetchContent)
FetchContent_Declare(cpp-lazy
        URL https://github.com/Kaaserne/cpp-lazy/releases/download/<TAG_HERE E.G. v9.0.0>/cpp-lazy-src.zip
        # Below is optional
        # URL_MD5 <MD5 HASH OF cpp-lazy-src.zip>
        # If using CMake >= 3.24, preferably set <bool> to TRUE
        # DOWNLOAD_EXTRACT_TIMESTAMP <bool>
)
# Optional settings:
# set(CPP_LAZY_USE_STANDALONE NO CACHE BOOL "") # Use {fmt}
# set(CPP_LAZY_USE_INSTALLED_FMT NO CACHE BOOL "") # Use bundled {fmt}, NO means use bundled, YES means use system installed {fmt}
# set(CPP_LAZY_USE_MODULES NO CACHE BOOL "") # Do not use C++20 modules
# set(CPP_LAZY_DEBUG_ASSERTIONS NO CACHE BOOL "") # Disable debug assertions in release mode
# set(CPP_LAZY_FMT_DEP_VERSION "" CACHE STRING "") # Specify version of {fmt} to use if needed
FetchContent_MakeAvailable(cpp-lazy)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} cpp-lazy::cpp-lazy)
```

An alternative ('less' recommended), add to your `CMakeLists.txt` the following:
```cmake
include(FetchContent)
FetchContent_Declare(cpp-lazy
        GIT_REPOSITORY https://github.com/Kaaserne/cpp-lazy
        GIT_TAG ... # Commit hash
        # If using CMake >= 3.24, preferably set <bool> to TRUE
        # DOWNLOAD_EXTRACT_TIMESTAMP <bool>
)
# Optional settings:
# set(CPP_LAZY_USE_STANDALONE NO CACHE BOOL "") # Use {fmt}
# set(CPP_LAZY_USE_INSTALLED_FMT NO CACHE BOOL "") # Use bundled {fmt}, NO means use bundled, YES means use system installed {fmt}
# set(CPP_LAZY_USE_MODULES NO CACHE BOOL "") # Do not use C++20 modules
# set(CPP_LAZY_DEBUG_ASSERTIONS NO CACHE BOOL "") # Disable debug assertions in release mode
# set(CPP_LAZY_FMT_DEP_VERSION "" CACHE STRING "") # Specify version of {fmt} to use. Only relevant if CPP_LAZY_USE_INSTALLED_FMT is YES
FetchContent_MakeAvailable(cpp-lazy)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} cpp-lazy::cpp-lazy)
```

## Using find_package
First, install the library. Then:
```cmake
find_package(cpp-lazy 9.0.0 REQUIRED CONFIG)
```

## Without CMake
### Without `{fmt}`
- Download the source code from the releases page
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
The time is equal to one iteration. One iteration excludes the creation of the iterable and includes one iteration of that iterable. Compiled with: gcc version 13.3.0.

<div style="text-align:center"><img src="https://github.com/Kaaserne/cpp-lazy/blob/master/bench/benchmarks-iterators-23.png?raw=true" />
</div>




