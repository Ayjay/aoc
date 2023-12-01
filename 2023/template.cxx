//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <vector>
#include <tuple>
#include <string_view>

const auto test_data = std::vector{ std::tuple
{R"", -2, -2}
};


auto run_a(std::string_view s) {
    return -1;
}

auto run_b(std::string_view s) {
    return -1;
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
