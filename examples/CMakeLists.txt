cmake_minimum_required(VERSION 3.14)

project(examples LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(cpp-lazy SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/..")
FetchContent_MakeAvailable(cpp-lazy)

set(examples
    algorithm
    any_iterable
    as_iterator
    basic_iterable
    c_string
    cached_reverse
    cached_size
    cartesian_product
    chunk_if
    chunks
    common
    concatenate
    custom_iterator
    drop_while
    drop
    enumerate
    except
    exclude
    exclusive_scan
    filter
    flatten
    generate_while
    generate
    group_by
    inclusive_scan
    interleave
    intersection
    iter_tools
    join_where
    loop
    map
    pipe
    print_and_format
    random
    range
    ref_or_view
    regex_split
    repeat
    rotate
    slice
    split
    take_every
    take_while
    take
    to_container
    unique
    zip_longest
    zip
)

foreach(name IN LISTS examples)
    add_executable(example_${name} ${name}.cpp)
    target_link_libraries(example_${name} PRIVATE cpp-lazy::cpp-lazy)
endforeach()
