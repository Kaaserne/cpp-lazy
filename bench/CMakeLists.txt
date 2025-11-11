cmake_minimum_required(VERSION 3.10...3.12.4)

project(Benchmark)

add_executable(Benchmark
        ${CMAKE_CURRENT_SOURCE_DIR}/benchmarks-iterators.cpp)

# Add cpp-lazy
include(FetchContent)
FetchContent_Declare(cpp-lazy SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/..")
FetchContent_MakeAvailable(cpp-lazy)

# Enable Google Benchmark to add dependencies for their tests
set(BENCHMARK_DOWNLOAD_DEPENDENCIES TRUE)
FetchContent_Declare(benchmark
    URL https://github.com/google/benchmark/archive/refs/tags/v1.9.1.tar.gz
    UPDATE_DISCONNECTED YES
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(benchmark)

target_link_libraries(Benchmark
    PRIVATE
        cpp-lazy
        benchmark::benchmark
)

math(EXPR CXX_STANDARD_INT "${CMAKE_CXX_STANDARD}")
target_compile_definitions(Benchmark PRIVATE CMAKE_CXX_STANDARD=${CXX_STANDARD_INT})

# Run as: ./Benchmark --benchmark_out=benchmark-iterators.json --benchmark_out_format=json
