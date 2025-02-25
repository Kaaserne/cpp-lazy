cmake_minimum_required(VERSION 3.14)

project(LazyTests LANGUAGES CXX)

set(CPP-LAZY_CATCH_VERSION "2.13.10" CACHE STRING "Version of Catch2 to use for testing")
Include(FetchContent)
FetchContent_Declare(Catch2 
	URL https://github.com/catchorg/Catch2/archive/refs/tags/v${CPP-LAZY_CATCH_VERSION}.tar.gz 
	URL_MD5 7a4dd2fd14fb9f46198eb670ac7834b7
)
FetchContent_MakeAvailable(Catch2)


# ---- Import root project ----
option(TEST_INSTALLED_VERSION "Import the library using find_package" OFF)
if (TEST_INSTALLED_VERSION)
	find_package(cpp-lazy REQUIRED CONFIG)
else ()
	# Enable warnings from includes
	set(cpp-lazy_INCLUDE_WITHOUT_SYSTEM ON CACHE INTERNAL "")

	FetchContent_Declare(cpp-lazy SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/..")
	FetchContent_MakeAvailable(cpp-lazy)
endif ()

include(CTest)

# ---- Tests ----
add_executable(cpp-lazy-tests
	algorithm.cpp
	any_iterable.cpp
	cartesian_product.cpp
	piping.cpp
	chunk_if.cpp
	chunks.cpp
	common.cpp
	concatenate.cpp
	cstring.cpp
	enumerate.cpp
	except.cpp
	exclude.cpp
	exclusive_scan.cpp
	filter.cpp
	flatten.cpp
	generate.cpp
	generate_while.cpp
	group_by.cpp
	inclusive_scan.cpp
	init.cpp
	intersection.cpp
	# iter_tools.cpp
	join_where.cpp
	loop.cpp
	map.cpp
	random.cpp
	range.cpp
	regex_split.cpp
	repeat.cpp
	rotate.cpp
	split.cpp
	standalone.cpp
	string_view.cpp
	take_every.cpp
	take.cpp
	take_while.cpp
	unique.cpp
	zip_longest.cpp
	zip.cpp
)

target_compile_options(cpp-lazy-tests
	PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive- /WX /diagnostics:caret>
	$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wpedantic -Wextra -Wall -Wshadow -Werror -Wconversion>
)
target_link_libraries(cpp-lazy-tests
	PRIVATE
		cpp-lazy::cpp-lazy
		Catch2::Catch2
)
add_test(
	NAME cpp-lazy-tests
	COMMAND $<TARGET_FILE:cpp-lazy-tests>
)
