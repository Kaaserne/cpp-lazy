cmake_minimum_required(VERSION 3.14)

project(LazyTests LANGUAGES CXX)

set(CPP-LAZY_DOCTEST_VERSION "2.4.12" CACHE STRING "Version of doctest to use for testing")
Include(FetchContent)
FetchContent_Declare(doctest 
	URL https://github.com/doctest/doctest/archive/refs/tags/v${CPP-LAZY_DOCTEST_VERSION}.tar.gz
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(doctest)

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
add_executable(tests
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
	c_string.cpp
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
	init.cpp
	interleave.cpp
	intersection.cpp
	iter_tools.cpp
	iterator.cpp
	join_where.cpp
	loop.cpp
	map.cpp
	maybe_owned.cpp
	pairwise.cpp
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

target_link_libraries(tests
	PRIVATE
		cpp-lazy::cpp-lazy
		cpp-lazy-ut-helper::cpp-lazy-ut-helper
		doctest::doctest
)
add_test(NAME tests	COMMAND $<TARGET_FILE:tests>)
target_compile_options(tests
	PRIVATE
		$<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive- /WX /diagnostics:caret>
		# -Wmismatched-tags causes internal compiler error for GCC. This in combination with #include <fstream> and #include <sstream> in pch
		$<$<CXX_COMPILER_ID:GNU>:-Wpedantic -Wextra -Wall -Wshadow -Werror -Wconversion -Wcast-qual -Wcomma-subscript -Wctor-dtor-privacy -Wdeprecated-copy-dtor -Wdouble-promotion -Wduplicated-branches -Wduplicated-cond -Wenum-conversion -Wextra-semi -Wfloat-equal -Wformat-overflow=2 -Wformat-signedness -Wformat=2 -Wlogical-op -Wmultichar -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wrange-loop-construct -Wrestrict -Wstrict-null-sentinel -Wsuggest-attribute=format -Wsuggest-attribute=malloc -Wuninitialized -Wvla -Wvolatile -Wwrite-strings -Wunused-result -Wunused-but-set-variable -Wmissing-declarations -Winvalid-pch -Weffc++ -Wdisabled-optimization -Wsign-conversion -Wcast-align -Wmissing-include-dirs -Wswitch-default -Wredundant-decls -Wundef -Wold-style-cast -Wstrict-overflow=5 -Wsuggest-override -Wtrampolines -Wzero-as-null-pointer-constant -Wuseless-cast -Wvector-operation-performance -Wnull-dereference -Walloc-zero -Walloca -Wduplicated-branches -Wcast-align=strict>

		$<$<CXX_COMPILER_ID:Clang,AppleClang>:-Wpedantic -Wextra -Wall -Werror -pedantic -pedantic-errors -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-c++98-compat-bind-to-temporary-copy -Wno-c++98-compat-local-type-template-args -Wno-unsafe-buffer-usage -Wno-padded -Wno-documentation-unknown-command -Wno-switch-enum -ftime-trace>
)
