add_library(cpp-lazy-ut-helper INTERFACE)
add_library(cpp-lazy-ut-helper::cpp-lazy-ut-helper ALIAS cpp-lazy-ut-helper)

target_include_directories(cpp-lazy-ut-helper
  INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/include"
)
target_link_libraries(cpp-lazy-ut-helper
  INTERFACE
    cpp-lazy::cpp-lazy
    doctest::doctest
)
target_precompile_headers(cpp-lazy-ut-helper INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include/cpp-lazy-ut-helper/pch.hpp")
