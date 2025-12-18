#include <ranges>
#include <vector>

int main() {
    std::vector<int> vec;
    static_cast<void>(std::ranges::views::zip(vec, vec, vec));
    static_cast<void>(std::ranges::views::filter(vec, [](int){ return true; }));
}
