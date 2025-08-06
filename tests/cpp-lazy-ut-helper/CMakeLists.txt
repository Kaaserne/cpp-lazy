add_library(cpp-lazy-ut-helper 
  src/c_string.cpp
  src/repeat.cpp
)
add_library(cpp-lazy-ut-helper::cpp-lazy-ut-helper ALIAS cpp-lazy-ut-helper)

target_include_directories(cpp-lazy-ut-helper
  PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/include"
)
target_compile_options(cpp-lazy-ut-helper
  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic>
)
target_link_libraries(cpp-lazy-ut-helper
  PUBLIC
    cpp-lazy::cpp-lazy
    doctest::doctest
)
