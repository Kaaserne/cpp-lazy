cmake_minimum_required(VERSION 3.10...3.12.4)

project(CompeTimeBenchmark)

include(FetchContent)

FetchContent_Declare(cpp-lazy
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../"
)
set(CPP-LAZY_USE_STANDALONE ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(cpp-lazy)

add_executable(CompileTimeBenchmarks compile-times.cpp)
target_compile_options(CompileTimeBenchmarks
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-ftime-report>
        $<$<CXX_COMPILER_ID:Clang>:-ftime-report -ftime-trace>
)
target_link_libraries(CompileTimeBenchmarks PRIVATE cpp-lazy::cpp-lazy)
target_compile_features(CompileTimeBenchmarks PRIVATE cxx_std_23)
