#include <string_view>
#include "aoc.h"

auto test_data =
R"(1000
2000
3000

4000

5000
6000

7000
8000
9000

10000)";

auto run_a(std::string_view s) {
    using namespace std::string_view_literals;
    auto elf_total = [](auto elf) { return ranges::accumulate(elf | rv::split("\n") | rv::transform(to_int), 0); };
    return ranges::max(rv::transform(rv::split(s, rv::split("\n\n"), rv::transform(elf_total))));
}

auto run_b(std::string_view s) {
    return 0;
}

int main() {
    run(run_a, run_b, 24000, -1, test_data, get_input(1));
}