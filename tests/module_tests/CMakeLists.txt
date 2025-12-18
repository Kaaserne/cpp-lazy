cmake_minimum_required(VERSION 3.28)
project(ModuleTests LANGUAGES CXX)

include(FetchContent)

option(CPP-LAZY_USE_MODULES_FIND_PACKAGE "Use find_package to locate cpp-lazy" OFF)

if (CPP-LAZY_USE_MODULES_FIND_PACKAGE)
    find_package(cpp-lazy REQUIRED CONFIG)
else()
    FetchContent_Declare(cpp-lazy SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../..")
    FetchContent_MakeAvailable(cpp-lazy)
endif()

add_executable(module_test module_test.cpp)
target_link_libraries(module_test PRIVATE cpp-lazy::cpp-lazy)
