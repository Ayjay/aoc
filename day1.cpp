#include "aoc.h"
#include <fmt/std.h>
#include <iostream>

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
    if (s.back() == '\n') {
        // std::view::split generates empty tokens if a string ends in the token
        // remove that trailing \n
        s = { s.begin(), s.end() - 1 };
    }

    auto elf_total = [](auto elf) { 
        auto item_values = elf | sv::split("\n"sv) | sv::transform(to_int);
        return ranges::accumulate(item_values, 0); 
    };
    auto elf_totals = s | sv::split("\n\n"sv) | sv::transform(elf_total);
    return ranges::max(elf_totals);
}

auto run_b(std::string_view s) {
    return 0;
}

int main() {
    run(run_a, run_b, 24000, -1, test_data, get_input(1));
}