cmake_minimum_required(VERSION 3.28)
project(ModuleTests LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(cpp-lazy SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../..")
FetchContent_MakeAvailable(cpp-lazy)

add_executable(module_test module_test.cpp)
target_link_libraries(module_test PRIVATE cpp-lazy::cpp-lazy)
