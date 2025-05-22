include(CMakeFindDependencyMacro)

if (NOT CPP-LAZY_USE_STANDALONE)
    find_dependency(fmt)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/cpp-lazyTargets.cmake")
