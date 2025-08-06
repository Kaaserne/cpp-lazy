option("tests", { default = false, description = "Enable tests" })
option("cpp-lazy-use-modules", { default = false, description = "Use cpp-lazy with C++20's modules" })

add_requires("fmt >=5")

if has_config("tests") then
    add_requires("doctest v2.4.12")
end

add_rules("mode.debug", "mode.release")

target("cpp-lazy")
    add_includedirs("include", { public = true })
    add_packages("fmt", { public = true })
    if has_config("cpp-lazy-use-modules") then
        add_files("src/lz.cppm")
        set_languages("c++20")
        set_kind("static")
    else 
        set_kind("headeronly")
    end
    add_headerfiles("include/(Lz/*.hpp)")
    add_headerfiles("include/(Lz/detail/*.hpp)")
    add_headerfiles("include/(Lz/detail/adaptors/*.hpp)")
    add_headerfiles("include/(Lz/detail/iterables/*.hpp)")
    add_headerfiles("include/(Lz/detail/iterators/*.hpp)")
    add_headerfiles("include/(Lz/detail/iterators/any_iterable/*.hpp)")
target_end()

if has_config("tests") then
    target("cpp-lazy tests")
        set_kind("binary")
        add_files("tests/*.cpp")
        add_deps("cpp-lazy")
        add_packages("doctest")
        add_cxxflags("-Wpedantic -Wextra -Wall -Wshadow -Wno-unused-function -Werror -Wconversion", { tools = { "gcc", "clang" } })
        add_cxxflags("/W4 /permissive- /WX", { tools = { "clang-cl", "msvc" } })
end
