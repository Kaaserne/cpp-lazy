add_library(c_string_forward_declarations c_string_forward_decl.cpp)
target_include_directories(c_string_forward_declarations PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
target_compile_options(c_string_forward_declarations
  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -pedantic>
)
target_link_libraries(c_string_forward_declarations
  PUBLIC
    cpp-lazy::cpp-lazy
)
