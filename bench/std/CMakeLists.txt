cmake_minimum_required(VERSION 3.10...3.12.4)

project(CompeTimeBenchmark)

include(FetchContent)

add_executable(CompileTimeBenchmarks compile-times.cpp)
target_compile_options(CompileTimeBenchmarks
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-ftime-report>
        $<$<CXX_COMPILER_ID:Clang>:-ftime-report>
)
target_compile_features(CompileTimeBenchmarks PRIVATE cxx_std_23)
