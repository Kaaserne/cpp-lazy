// TODO
#include <Lz/algorithm.hpp>
#include <iostream>
#include <Lz/c_string.hpp>
#include <vector>

int main() {
    // All algorithms will try to use its std equivalent if possible. For instance:

    // std::vector<int> v = {1, 2, 3};
    // std::cout << (*lz::find(v, 2)) << '\n'; // Will call std::find

    // auto c_str = lz::c_string("Hello, world!");
    // std::cout << (*lz::find(c_str, 'w')) << '\n'; // Will not call std::find

    // But this for example, will call std::find again
    // auto common = lz::common(c_str);
    // std::cout << (*lz::find(common, 'w')) << '\n'; // Will call std::find
    std::cout << std::boolalpha;

    {
        std::cout << "Empty\n";
        std::vector<int> v;
        std::cout << lz::empty(v) << '\n'; // true
        v = {1, 2, 3};
        std::cout << lz::empty(v) << "\n\n"; // false
    }
    {
        std::cout << "Has one\n";
        std::vector<int> v;
        std::cout << lz::has_one(v) << '\n'; // false
        v = {1};
        std::cout << lz::has_one(v) << '\n'; // true
        v = {1, 2};
        std::cout << lz::has_one(v) << "\n\n"; // false
    }
    {
        std::cout << "Has many\n";
        std::vector<int> v;
        std::cout << lz::has_many(v) << '\n'; // false
        v = {1};
        std::cout << lz::has_many(v) << '\n'; // false
        v = {1, 2};
        std::cout << lz::has_many(v) << "\n\n"; // true
    }
    {
        std::cout << "Front\n";
        std::vector<int> v = {1, 2, 3};
        std::cout << lz::front(v) << '\n'; // 1
        v = {};
        // std::cout << lz::front(v) << '\n'; // Undefined behavior
    }
    {
        std::cout << "Back\n";
        std::vector<int> v = {1, 2, 3};
        std::cout << lz::back(v) << '\n'; // 3
        v = {};
        // std::cout << lz::back(v) << '\n'; // Undefined behavior
    }
    {
        std::cout << "Front or\n";
        std::vector<int> v = {1, 2, 3};
        std::cout << lz::front_or(v, 0) << '\n'; // 1
        v = {};
        std::cout << lz::front_or(v, 0) << "\n\n"; // 0
    }
    {
        std::cout << "Back or\n";
        std::vector<int> v = {1, 2, 3};
        std::cout << lz::back_or(v, 0) << '\n'; // 3
        v = {};
        std::cout << lz::back_or(v, 0) << "\n\n"; // 0
    }
    {
        std::cout << "Accumulate\n";
        std::vector<int> v = {1, 2, 3};
        std::cout << lz::accumulate(v, 0) << '\n'; // 6
        std::cout << lz::accumulate(v, 0, std::plus<int>()) << "\n\n"; // 6
    }
    {
        std::cout << "Max element\n";
        std::vector<int> v = {1, 2, 3};
        std::cout << *lz::max_element(v) << '\n'; // 3
        std::cout << *lz::max_element(v, std::greater<int>()) << "\n\n"; // 1
    }
    {
        std::cout << "Min element\n";
        std::vector<int> v = {1, 2, 3};
        std::cout << *lz::min_element(v) << '\n'; // 1
        std::cout << *lz::min_element(v, std::greater<int>()) << "\n\n"; // 3
    }
    {
        std::cout << "Find if\n";
        std::vector<int> v = {1, 2, 3};
        std::cout << (*lz::find_if(v, [](int i) { return i == 2; })) << '\n'; // 2
        std::cout << (lz::find_if(v, [](int i) { return i == 4; }) == std::end(v)) << "\n\n"; // true
    }
    {
        std::cout << "Find\n";
        std::vector<int> v = {1, 2, 3};
        std::cout << (*lz::find(v, 2)) << '\n'; // 2
        std::cout << (lz::find(v, 4) == std::end(v)) << "\n\n"; // true
    }
    {
        std::cout << "Find last\n";
        std::vector<int> v = {1, 2, 3, 2};
        std::cout << (*lz::find_last(v, 2)) << '\n'; // 2
        std::cout << (lz::find_last(v, 4) == std::end(v)) << "\n\n"; // true
    }
    {
        std::cout << "Find last if\n";
        std::vector<int> v = {1, 2, 3, 2};
        std::cout << (*lz::find_last_if(v, [](int i) { return i == 2; })) << '\n'; // 2
        std::cout << (lz::find_last_if(v, [](int i) { return i == 4; }) == std::end(v)) << "\n\n"; // true
    }
    {
        std::cout << "Search\n";
        std::vector<int> v = {1, 2, 3, 2};
        std::vector<int> v2 = {2, 3};
        auto pos = lz::search(v, v2);
        std::cout << *pos.first << ' ' << *pos.second << '\n'; // 1 2
        v2 = {4, 5};
        pos = lz::search(v, v2);
        std::cout << (pos.first == std::end(v)) << "\n\n"; // true
    }
    {
        std::cout << "Find if not\n";
        std::vector<int> v = {1, 2, 3};
        std::cout << (*lz::find_if_not(v, [](int i) { return i == 2; })) << '\n'; // 1
        std::cout << (lz::find_if_not(v, [](int i) { return i == 4; }) == std::end(v)) << "\n\n"; // true
    }
    {
        std::cout << "Find last if not\n";
        std::vector<int> v = {1, 2, 3, 2};
        std::cout << (*lz::find_last_if_not(v, [](int i) { return i == 2; })) << '\n'; // 3
        std::cout << (lz::find_last_if_not(v, [](int i) { return i == 4; }) == std::end(v)) << "\n\n"; // true
    }
    {
        std::cout << "Find or default\n";
        std::vector<int> v = {1, 2, 3};
        std::cout << lz::find_or_default(v, 2, 0) << '\n'; // 2
        std::cout << lz::find_or_default(v, 4, 0) << "\n\n"; // 0
    }
    {
        std::cout << "Find or default if\n";
        std::vector<int> v = {1, 2, 3};
        std::cout << lz::find_or_default_if(v, [](int i) { return i == 2; }, 0) << '\n'; // 2
        std::cout << lz::find_or_default_if(v, [](int i) { return i == 4; }, 0) << "\n\n"; // 0
    }
    {
        std::cout << "Find last or default\n";
        std::vector<int> v = {1, 2, 3, 2};
        std::cout << lz::find_last_or_default(v, 2, 0) << '\n'; // 2
        std::cout << lz::find_last_or_default(v, 4, 0) << "\n\n"; // 0
    }
    {
        std::cout << "Find last or default if\n";
        std::vector<int> v = {1, 2, 3, 2};
        std::cout << lz::find_last_or_default_if(v, [](int i) { return i == 2; }, 0) << '\n'; // 2
        std::cout << lz::find_last_or_default_if(v, [](int i) { return i == 4; }, 0) << "\n\n"; // 0
    }
    {
        std::cout << "Find last or default not\n";
        std::vector<int> v = {1, 2, 3, 2};
        std::cout << lz::find_last_or_default_not(v, 2, 0) << '\n'; // 3
        std::cout << lz::find_last_or_default_not(v, 4, 0) << "\n\n"; // 0
    }
    {
        std::cout << "Find last or default if not\n";
        std::vector<int> v = {1, 2, 3, 2};
        std::cout << lz::find_last_or_default_if_not(v, [](int i) { return i == 2; }, 0) << '\n'; // 3
        std::cout << lz::find_last_or_default_if_not(v, [](int i) { return i == 4; }, 0) << "\n\n"; // 0
    }
    {
        std::cout << "Index of\n";
        std::vector<int> v = {1, 2, 3};
        std::cout << lz::index_of(v, 2) << '\n'; // 1
        std::cout << lz::index_of(v, 4) << "\n\n"; // lz::npos
    }
    {
        std::cout << "Index of if\n";
        std::vector<int> v = {1, 2, 3};
        std::cout << lz::index_of_if(v, [](int i) { return i == 2; }) << '\n'; // 1
        std::cout << lz::index_of_if(v, [](int i) { return i == 4; }) << "\n\n"; // lz::npos
    }
    {
        std::cout << "Contains\n";
        std::vector<int> v = {1, 2, 3};
        std::cout << lz::contains(v, 2) << '\n'; // true
        std::cout << lz::contains(v, 4) << "\n\n"; // false
    }
    {
        std::cout << "Starts with\n";
        std::vector<int> v = {1, 2, 3};
        std::vector<int> v2 = {1, 2};
        std::cout << lz::starts_with(v, v2) << '\n'; // true
        v2 = {2, 3};
        std::cout << lz::starts_with(v, v2) << "\n\n"; // false
    }
    {
        std::cout << "Ends with\n";
        std::vector<int> v = {1, 2, 3};
        std::vector<int> v2 = {2, 3};
        std::cout << lz::ends_with(v, v2) << '\n'; // true
        v2 = {1, 2};
        std::cout << lz::ends_with(v, v2) << "\n\n"; // false
    }
    {
        std::cout << "Partition\n";
        std::vector<int> v = {1, 2, 3, 4, 5};
        auto it = lz::partition(v, [](int i) { return i % 2 == 0; });
        for (auto i = std::begin(v); i != it; ++i) {
            std::cout << *i << ' ';
        }
        // 1 3 5
        std::cout << '\n';
        for (auto i = it; i != std::end(v); ++i) {
            std::cout << *i << ' ';
        }
        // 2 4
        std::cout << "\n\n";
    }
    {
        std::cout << "Mean\n";
        std::vector<int> v = {1, 2, 3};
        std::cout << lz::mean(v) << '\n'; // 2
        std::cout << lz::mean(v, std::plus<int>()) << "\n\n"; // 6
    }
    {
        std::cout << "For each\n";
        std::vector<int> v = {1, 2, 3};
        lz::for_each(v, [](int i) { std::cout << i << ' '; });
        // 1 2 3
        std::cout << "\n\n";
    }
    {
        std::cout << "For each while\n";
        std::vector<int> v = {1, 2, 3};
        lz::for_each_while(v, [](int i) { 
            std::cout << i << ' ';
            return i != 2;
        });
        // 1 2
        std::cout << "\n\n";
    }
    {
        std::cout << "Copy\n";
        std::vector<int> v = {1, 2, 3};
        std::vector<int> v2;
        lz::copy(v, std::back_inserter(v2));
        for (auto i : v2) {
            std::cout << i << ' ';
        }
        // 1 2 3
        std::cout << "\n\n";
    }
    {
        std::cout << "Transform\n";
        std::vector<int> v = {1, 2, 3};
        std::vector<int> v2;
        lz::transform(v, std::back_inserter(v2), [](int i) { return i * 2; });
        for (auto i : v2) {
            std::cout << i << ' ';
        }
        // 2 4 6
        std::cout << "\n\n";
    }
    {
        std::cout << "Equal\n";
        std::vector<int> v = {1, 2, 3};
        std::vector<int> v2 = {1, 2, 3};
        std::cout << lz::equal(v, v2) << '\n'; // true
        v2 = {1, 2, 4};
        std::cout << lz::equal(v, v2) << "\n\n"; // false
    }
    {
        std::cout << "Lower bound\n";
        std::vector<int> v = {1, 2, 3}; // Must be sorted
        std::cout << *lz::lower_bound(v, 2) << '\n'; // 2
        std::cout << *lz::lower_bound(v, 3, std::greater<int>()) << '\n'; // 1
        std::cout << (lz::lower_bound(v, 4) == std::end(v)) << "\n\n"; // true
    }
    {
        std::cout << "Upper bound\n";
        std::vector<int> v = {1, 2, 3}; // Must be sorted
        std::cout << *lz::upper_bound(v, 2) << '\n'; // 3
        std::cout << *lz::upper_bound(v, 3, std::greater<int>()) << '\n'; // 1
        std::cout << (lz::upper_bound(v, 4) == std::end(v)) << "\n\n"; // true
    }
    {
        std::cout << "Binary Search\n";
        std::vector<int> v = {1, 2, 3}; // Must be sorted
        std::cout << lz::binary_search(v, 2) << '\n'; // true
        std::cout << lz::binary_search(v, 4) << "\n\n"; // false
    }
    {
        std::cout << "All of\n";
        std::vector<int> v = {1, 2, 3};
        std::cout << lz::all_of(v, [](int i) { return i > 0; }) << '\n'; // true
        std::cout << lz::all_of(v, [](int i) { return i > 1; }) << "\n\n"; // false
    }
    {
        std::cout << "Any of\n";
        std::vector<int> v = {1, 2, 3};
        std::cout << lz::any_of(v, [](int i) { return i == 2; }) << '\n'; // true
        std::cout << lz::any_of(v, [](int i) { return i == 4; }) << "\n\n"; // false
    }
    {
        std::cout << "None of\n";
        std::vector<int> v = {1, 2, 3};
        std::cout << lz::none_of(v, [](int i) { return i == 2; }) << '\n'; // false
        std::cout << lz::none_of(v, [](int i) { return i == 4; }) << "\n\n"; // true
    }
    {
        std::cout << "Adjacent find\n";
        std::vector<int> v = {1, 2, 2, 3, 2};
        auto pos = lz::adjacent_find(v);
        auto distance = lz::distance(v.begin(), pos);
        std::cout << *pos << " with distance " << distance << '\n'; // 2 with distance 1
        ++pos;
        lz::sized_iterable<decltype(v.begin())> iterable(pos, v.end());
        auto new_pos = lz::adjacent_find(iterable);
        std::cout << (new_pos == iterable.end()) << "\n\n"; // true
    }
    {
        std::cout << "Count if\n";
        std::vector<int> v = {1, 2, 3};
        std::cout << lz::count_if(v, [](int i) { return i == 2; }) << '\n'; // 1
        std::cout << lz::count_if(v, [](int i) { return i == 4; }) << "\n\n"; // 0
    }
    {
        std::cout << "Count\n";
        std::vector<int> v = {1, 2, 3};
        std::cout << lz::count(v, 2) << '\n'; // 1
        std::cout << lz::count(v, 4) << "\n\n"; // 0
    }
    {
        std::cout << "Is sorted\n";
        std::vector<int> v = {1, 2, 3};
        std::cout << lz::is_sorted(v) << '\n'; // true
        v = {3, 2, 1};
        std::cout << lz::is_sorted(v) << "\n\n"; // false
        std::cout << lz::is_sorted(v, std::greater<int>()) << '\n'; // true
    }
}