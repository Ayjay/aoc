#include "aoc.h"
#include <vector>

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

template <class T>
struct sayer;

auto run_a(std::string_view s) {
    auto get_elf_total = [](auto elf) { 
        auto item_values = elf | sv::split("\n"sv) | sv::transform(to_int);
        return ranges::accumulate(item_values, 0); 
    };
    auto elf_totals = s | sv::split("\n\n"sv) | sv::transform(get_elf_total);
    return ranges::max(elf_totals);
}

auto run_b(std::string_view s) {
    auto get_elf_total = [](auto elf) { 
        auto item_values = elf | sv::split("\n"sv) | sv::transform(to_int);
        return ranges::accumulate(item_values, 0); 
    };
    auto elf_totals = s | sv::split("\n\n"sv) | sv::transform(get_elf_total) | ranges::to<std::vector>();
    std::ranges::sort(elf_totals);
    return reduce(elf_totals | sv::reverse | sv::take(3));
}

int main() {
    run(run_a, run_b, 24000, 45000, test_data, get_input(1));
}