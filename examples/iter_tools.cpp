#include <Lz/iter_tools.hpp>
#include <iostream>
#include <map>
#include <vector>

int main() {
#ifdef LZ_HAS_CXX_17
    // Lines
    std::string text = "Hello\nWorld\n!";
    auto lines = lz::lines(text); // or: `text | lz::lines;`
    for (const auto& line : lines) {
        std::cout.write(line.data(), static_cast<std::streamsize>(line.size()));
        std::cout << '\n';
    }
    // "Hello"
    // "World"
    // "!"
    std::cout << '\n';

    lz::string_view text2 = "Hello\nWorld\n!";
    auto lines2 =
        lz::lines(text2); // or: `text | lz::lines;` Does not hold a reference to the original string, since it is a string_view
    for (const auto& line : lines2) {
        std::cout.write(line.data(), static_cast<std::streamsize>(line.size()));
        std::cout << '\n';
    }
    // "Hello"
    // "World"
    // "!"
    std::cout << '\n';

#else

    // Lines
    std::string text = "Hello\nWorld\n!";
    auto lines = lz::lines(text); // or: `text | lz::lines;`
    lz::for_each(lines, [](const lz::string_view& line) {
        std::cout.write(line.data(), static_cast<std::streamsize>(line.size()));
        std::cout << '\n';
    });
    // "Hello"
    // "World"
    // "!"
    std::cout << '\n';

    lz::string_view text2 = "Hello\nWorld\n!";
    auto lines2 =
        lz::lines(text2); // or: `text | lz::lines;` Does not hold a reference to the original string, since it is a string_view
    lz::for_each(lines2, [](const lz::string_view& line) {
        std::cout.write(line.data(), static_cast<std::streamsize>(line.size()));
        std::cout << '\n';
    });
    // "Hello"
    // "World"
    // "!"
    std::cout << '\n';

#endif

    // As
    std::vector<int> numbers = { 1, 2, 3, 4, 5 };
#ifdef LZ_HAS_CXX_11

    auto floats = lz::as<float>{}(numbers); // or: `numbers | lz::as<float>{};`

#else

    auto floats = lz::as<float>(numbers); // or: `numbers | lz::as<float>;`

#endif

    for (const auto& f : floats) {
        std::cout << f << ' ';
    }
    // 1.0 2.0 3.0 4.0 5.0
    std::cout << '\n';

    // Pairwise_n
    std::vector<int> pairwise_numbers = { 1, 2, 3, 4, 5 };
#ifdef LZ_HAS_CXX_11

    auto pairwise_iterable = lz::pairwise_n<3>{}(pairwise_numbers); // or: `pairwise_numbers | lz::pairwise_n<3>{};`

#else

    auto pairwise_iterable = lz::pairwise_n<3>(pairwise_numbers); // or: `pairwise_numbers | lz::pairwise_n<3>;`

#endif

    for (const auto& tup : pairwise_iterable) {
        std::cout << '(' << std::get<0>(tup) << ", " << std::get<1>(tup) << ", " << std::get<2>(tup) << ") ";
    }
    // (1, 2, 3) (2, 3, 4) (3, 4, 5)
    std::cout << '\n';

    // Pairwise
    auto pairwise_iterable2 = lz::pairwise(pairwise_numbers); // or: `pairwise_numbers | lz::pairwise;`
    for (const auto& tup : pairwise_iterable2) {
        std::cout << '(' << std::get<0>(tup) << ", " << std::get<1>(tup) << ") ";
    }
    // (1, 2) (2, 3) (3, 4) (4, 5)
    std::cout << '\n';

    // Get_nth
    std::vector<std::tuple<int, int, int>> three_tuple_vec = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } };
#ifdef LZ_HAS_CXX_11

    auto nth_elements = lz::get_nth<2>{}(three_tuple_vec); // or: `three_tuple_vec | lz::get_nth<2>{};`

#else

    auto nth_elements = lz::get_nth<2>(three_tuple_vec); // or: `three_tuple_vec | lz::get_nth<2>;`

#endif
    for (const auto& elem : nth_elements) {
        std::cout << elem << ' ';
    }
    // 3 6 9
    std::cout << '\n';

    // Keys and Values
    std::map<int, std::string> m = { { 1, "hello" }, { 2, "world" }, { 3, "!" } };
    auto keys = lz::keys(m);     // or: `m | lz::keys;`
    auto values = lz::values(m); // or: `m | lz::values;`

    for (const auto& key : keys) {
        std::cout << key << ' ';
    }
    // 1 2 3
    std::cout << '\n';

    for (const auto& value : values) {
        std::cout << value << ' ';
    }
    // hello world !
    std::cout << '\n';

    // Filter map
    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    auto fm = lz::filter_map(vec, [](int i) { return i % 2 == 0; }, [](int i) { return i * 2; });
    // or: `vec | lz::filter_map([](int i) { return i % 2 == 0; }, [](int i) { return i * 2; });`
    for (const auto& f : fm) {
        std::cout << f << ' ';
    }
    // 4 8
    std::cout << '\n';

    // Select
    std::vector<int> select_numbers = { 1, 2, 3, 4, 5 };
    std::vector<bool> selectors = { true, false, true, false, true };
    auto selected = lz::select(select_numbers, selectors); // or: `select_numbers | lz::select(selectors);`
    for (const auto& s : selected) {
        std::cout << s << ' ';
    }
    // 1 3 5
    std::cout << '\n';

    // Drop back while
    std::vector<int> drop_back_numbers = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    auto drop_back_iterable = lz::drop_back_while(drop_back_numbers, [](int i) { return i >= 7; });
    // or: `drop_back_numbers | lz::drop_back_while([](int i) { return i >= 7; });`
    for (const auto& db : drop_back_iterable) {
        std::cout << db << ' ';
    }
    // 1 2 3 4 5 6
    std::cout << '\n';

    // Trim string
    std::string trim_text = "   Hello World!   ";
    auto trimmed = lz::trim(trim_text); // or: `trim_text | lz::trim;`
    for (const auto& t : trimmed) {
        std::cout << t << ' ';
    }
    // Hello World!
    std::cout << '\n';

    // Trim
    std::vector<int> trim_numbers = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto trimmed_numbers = lz::trim(trim_numbers, [](int i) { return i < 3; }, [](int i) { return i > 7; });
    // or: `trim_numbers | lz::trim([](int i) { return i < 3; }, [](int i) { return i > 7; });`
    for (const auto& tn : trimmed_numbers) {
        std::cout << tn << ' ';
    }
    // 3 4 5 6 7
    std::cout << '\n';
}
