cmake_minimum_required(VERSION 3.14)

project(LazyTests LANGUAGES CXX)

set(CPP-LAZY_CATCH_VERSION "3.9.0" CACHE STRING "Version of Catch2 to use for testing")
Include(FetchContent)
FetchContent_Declare(Catch2 
	URL https://github.com/catchorg/Catch2/archive/refs/tags/v${CPP-LAZY_CATCH_VERSION}.tar.gz 
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(Catch2)

set(CPP-LAZY_DEBUG_ASSERTIONS ON CACHE BOOL "Enable debug assertions in cpp-lazy" FORCE)

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

add_subdirectory(cpp-lazy-ut-helper)

# ---- Tests ----
set(tests
	algorithm.cpp
	any_iterable.cpp
	as_iterator.cpp
	cached_size.cpp
	cartesian_product.cpp
	piping.cpp
	chunk_if.cpp
	chunks.cpp
	common.cpp
	concatenate.cpp
	cstring.cpp
	duplicates.cpp
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
	interleave.cpp
	intersection.cpp
	iter_tools.cpp
	iterator.cpp
	join_where.cpp
	loop.cpp
	map.cpp
	maybe_owned.cpp
	random.cpp
	range.cpp
	regex_split.cpp
	repeat.cpp
	reverse.cpp
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

foreach(test_file IN LISTS tests)
	get_filename_component(test_name "${test_file}" NAME_WE)
	add_executable(${test_name} init.cpp "${test_file}")
	target_link_libraries(${test_name}
		PRIVATE
			cpp-lazy::cpp-lazy
			Catch2::Catch2WithMain
			cpp-lazy-ut-helper
	)
	target_include_directories(${test_name}
		PRIVATE
			"${CMAKE_CURRENT_LIST_DIR}"
	)
	add_test(
		NAME ${test_name}
		COMMAND $<TARGET_FILE:${test_name}>
	)
	target_compile_options(${test_name}
		PRIVATE
			$<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive- /WX /diagnostics:caret>

			$<$<CXX_COMPILER_ID:GNU>:-Wpedantic -Wextra -Wall -Wshadow -Werror -Wconversion -Wcast-qual -Wcomma-subscript -Wctor-dtor-privacy -Wdeprecated-copy-dtor -Wdouble-promotion -Wduplicated-branches -Wduplicated-cond -Wenum-conversion -Wextra-semi -Wfloat-equal -Wformat-overflow=2 -Wformat-signedness -Wformat=2 -Wlogical-op -Wmismatched-tags -Wmultichar -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wrange-loop-construct -Wrestrict -Wstrict-null-sentinel -Wsuggest-attribute=format -Wsuggest-attribute=malloc -Wuninitialized -Wvla -Wvolatile -Wwrite-strings>

			$<$<CXX_COMPILER_ID:Clang,AppleClang>:-Wpedantic -Wextra -Wall -Wshadow -Werror -Wconversion -Wcast-qual -Wctor-dtor-privacy -Wdeprecated-copy-dtor -Wdouble-promotion  -Wenum-conversion -Wextra-semi -Wfloat-equal -Wformat-overflow -Wformat=2 -Wmismatched-tags -Wmultichar -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wrange-loop-construct -Wuninitialized -Wvla -Wwrite-strings>
	)
endforeach()
