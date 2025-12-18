cmake_minimum_required(VERSION 3.14)

project(cpp-lazy
        VERSION 9.0.0
        DESCRIPTION
        "A fast C++11/14/17/20 header only library for lazy evaluation and function tools"
        HOMEPAGE_URL "https://github.com/Kaaserne/cpp-lazy"
        LANGUAGES CXX)

# ---- Warning guard ----
# Protect dependents from this project's warnings if the guard isn't disabled
set(cpp-lazy_warning_guard "SYSTEM")
if(cpp-lazy_INCLUDE_WITHOUT_SYSTEM)
    set(cpp-lazy_warning_guard "")
endif()

include(GNUInstallDirs)
set(CPP-LAZY_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}" CACHE STRING "Installation directory of the include files")

option(CPP-LAZY_USE_STANDALONE "Standalone library without {fmt}" NO)
option(CPP-LAZY_USE_INSTALLED_FMT "Import {fmt} using find_package" NO)
option(CPP-LAZY_USE_MODULES "Enable C++20's modules" NO)

if (NOT DEFINED CPP-LAZY_DEBUG_ASSERTIONS)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CPP-LAZY_DEBUG_ASSERTIONS ON)
    else()
        set(CPP-LAZY_DEBUG_ASSERTIONS OFF)
    endif()
endif()
option(CPP-LAZY_DEBUG_ASSERTIONS "Debug assertions" ${CPP-LAZY_DEBUG_ASSERTIONS})

if (CPP-LAZY_USE_MODULES)
	message(STATUS "cpp-lazy: using C++20 modules")
	set(CPP-LAZY_SOURCE_FILES "src/lz.cppm")
	set(CPP-LAZY_LINK_VISIBILITY "PUBLIC")
	set(CPP-LAZY_COMPILE_FEATURES_VISIBLITY "PUBLIC")
	set(CPP-LAZY_COMPILE_DEFINITIONS_VISIBLITY "PRIVATE")
	set(CPP-LAZY_COMPILE_INCLUDE_VISIBLITY "PUBLIC")
	set(CMAKE_CXX_SCAN_FOR_MODULES ON)
else()
	set(CPP-LAZY_LIB_TYPE "INTERFACE")
	set(CPP-LAZY_LINK_VISIBILITY "INTERFACE")
	set(CPP-LAZY_COMPILE_FEATURES_VISIBLITY "INTERFACE")
	set(CPP-LAZY_COMPILE_DEFINITIONS_VISIBLITY "INTERFACE")
	set(CPP-LAZY_COMPILE_INCLUDE_VISIBLITY "INTERFACE")
endif()

# ---- Import {fmt} ----
if (CPP-LAZY_USE_STANDALONE)
	message(STATUS "cpp-lazy: using standalone version")
else()
	if (CPP-LAZY_USE_INSTALLED_FMT)
		message(STATUS "cpp-lazy: using external {fmt} library")
		find_package(fmt REQUIRED CONFIG)
	else()
		message(STATUS "cpp-lazy: using bundled {fmt} library")
		set(FMT_OS YES CACHE INTERNAL "")
		set(FMT_INSTALL YES CACHE INTERNAL "" FORCE)
		include(FetchContent)
		set(CPP-LAZY_FMT_DEP_VERSION 12.0.0 CACHE STRING "Version of {fmt} to use")
		FetchContent_Declare(fmt 
			URL https://github.com/fmtlib/fmt/archive/refs/tags/${CPP-LAZY_FMT_DEP_VERSION}.tar.gz 
			UPDATE_DISCONNECTED YES 
			DOWNLOAD_EXTRACT_TIMESTAMP TRUE
		)
		FetchContent_MakeAvailable(fmt)
	endif()
endif()

message(STATUS "cpp-lazy: standalone version: ${CPP-LAZY_USE_STANDALONE}.")
message(STATUS "cpp-lazy: debug assertions: ${CPP-LAZY_DEBUG_ASSERTIONS}.")

# ---- Declare library ----
add_library(cpp-lazy ${CPP-LAZY_LIB_TYPE} "${CPP-LAZY_SOURCE_FILES}")
add_library(cpp-lazy::cpp-lazy ALIAS cpp-lazy)

if (CPP-LAZY_USE_MODULES)
	target_sources(cpp-lazy PUBLIC
		FILE_SET cxx_modules
		TYPE
			CXX_MODULES
		FILES "${CPP-LAZY_SOURCE_FILES}")
	target_compile_features(cpp-lazy PRIVATE cxx_std_20)
endif()

# Link lib stacktrace if debug assertions are enabled, on Linux with C++23 or later
set(IS_CXX_23 $<VERSION_GREATER_EQUAL:${CMAKE_CXX_STANDARD},23>)
set(IS_LINUX_COMPILER $<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>)
set(LZ_LINK_LIBBACKTRACE_CONDITION
    $<AND:$<BOOL:${CPP-LAZY_DEBUG_ASSERTIONS}>,${IS_LINUX_COMPILER},${IS_CXX_23}>
)
target_link_libraries(cpp-lazy ${CPP-LAZY_LINK_VISIBILITY}
	$<$<NOT:$<BOOL:${CPP-LAZY_USE_STANDALONE}>>:fmt::fmt>
	$<${LZ_LINK_LIBBACKTRACE_CONDITION}:stdc++_libbacktrace>
)
target_compile_definitions(cpp-lazy ${CPP-LAZY_COMPILE_DEFINITIONS_VISIBLITY}
	$<$<BOOL:${CPP-LAZY_USE_STANDALONE}>:LZ_STANDALONE>
	$<$<BOOL:${CPP-LAZY_DEBUG_ASSERTIONS}>:LZ_DEBUG_ASSERTIONS>
)

target_include_directories(cpp-lazy	${cpp-lazy_warning_guard}
	${CPP-LAZY_COMPILE_INCLUDE_VISIBLITY}
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
		"$<INSTALL_INTERFACE:${CPP-LAZY_INCLUDE_DIR}>"
)

# ---- Install ----
include(CMakePackageConfigHelpers)

set(cpp-lazy_install_cmakedir "${CMAKE_INSTALL_LIBDIR}/cmake/cpp-lazy")

option(CPP-LAZY_INSTALL "Install cpp-lazy" OFF)

if (CPP-LAZY_USE_MODULES AND CPP-LAZY_INSTALL)
	install(
		TARGETS cpp-lazy
        EXPORT cpp-lazyTargets
        INCLUDES DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        FILE_SET cxx_modules
		DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	)
elseif (CPP-LAZY_INSTALL)
	install(
		TARGETS cpp-lazy
		EXPORT cpp-lazyTargets
		INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
	)
endif()

if (CPP-LAZY_INSTALL)
	write_basic_package_version_file(
		cpp-lazyConfigVersion.cmake
		VERSION ${PROJECT_VERSION}
		COMPATIBILITY SameMajorVersion
		ARCH_INDEPENDENT
	)
	configure_package_config_file(
		"${PROJECT_SOURCE_DIR}/cmake/cpp-lazyConfig.cmake.in"
		"${PROJECT_BINARY_DIR}/cpp-lazyConfig.cmake"
		INSTALL_DESTINATION "${cpp-lazy_install_cmakedir}"
	)
	install(EXPORT cpp-lazyTargets
			NAMESPACE cpp-lazy::
			DESTINATION "${cpp-lazy_install_cmakedir}"
	)
	install(FILES
			"${PROJECT_BINARY_DIR}/cpp-lazyConfig.cmake"
			"${PROJECT_BINARY_DIR}/cpp-lazyConfigVersion.cmake"
			DESTINATION "${cpp-lazy_install_cmakedir}"
	)
	
	install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
			DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
	)
	include(CPack)
	set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}/${CPACK_PACKAGE_VERSION}")
	set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.md")
endif()

# ---- Release configuration zip package, bare essentials ----
option(CPP-LAZY_BUILD_ESSENTIALS_ZIP OFF)
if (CPP-LAZY_BUILD_ESSENTIALS_ZIP)
	add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-src.zip"
		COMMAND ${CMAKE_COMMAND} -E tar c "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-src.zip" --format=zip -- src/ include/ CMakeLists.txt LICENSE.md cmake/
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
		DEPENDS src/;include/;CMakeLists.txt;LICENSE.md
	)
	add_custom_target(${PROJECT_NAME}_package DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-src.zip")
endif ()
