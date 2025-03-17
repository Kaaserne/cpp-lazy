#include <Lz/group_by.hpp>
#include <vector>
#include <string>

int main() {
	std::vector<std::string> vec = {
		"hello", "hellp", "i'm", "done"
	};

	std::sort(vec.begin(), vec.end(), [](const std::string& a, const std::string& b) { return a.length() < b.length(); });
	auto grouper = lz::group_by(vec, [](const std::string& a, const std::string& b) { return a.length() == b.length(); });

	for (auto&& pair : grouper) {
        std::cout << "String length group: " << pair.first.length() << '\n';
        // Or use fmt::print("String length group: {}\n", pair.first.length());
        for (auto& str : pair.second) {
            std::cout << "value: " << str << '\n';
            // Or use fmt::print("value: {}\n", str);
        }
    }

    /* Output:
    String length group: 3
    value: i'm
    String length group: 4
    value: done
    String length group: 5
    value: hello
    value: hellp
    */

    std::cout << '\n';

    for (auto&& pair : vec | lz::group_by([](const std::string& a, const std::string& b) { return a.length() == b.length(); })) {
        std::cout << "String length group: " << pair.first.length() << '\n';
        // Or use fmt::print("String length group: {}\n", pair.first.length());
        for (auto& str : pair.second) {
            std::cout << "value: " << str << '\n';
            // Or use fmt::print("value: {}\n", str);
        }
    }
    /* Output:
    String length group: 3
    value: i'm
    String length group: 4
    value: done
    String length group: 5
    value: hello
    value: hellp
    */
}
