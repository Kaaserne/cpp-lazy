#include <Lz/filter.hpp>
#include <Lz/zip.hpp>
#include <vector>

int main() {
    std::vector<int> vec;
    static_cast<void>(lz::zip(vec, vec, vec));
    static_cast<void>(lz::filter(vec, [](int) { return true; }));
}
