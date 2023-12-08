//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <vector>
#include <tuple>
#include <string_view>

const auto test_data = std::vector{ std::tuple
{R"(RL

AAA = (BBB, CCC)
BBB = (DDD, EEE)
CCC = (ZZZ, GGG)
DDD = (DDD, DDD)
EEE = (EEE, EEE)
GGG = (GGG, GGG)
ZZZ = (ZZZ, ZZZ))", 2, -2},
{R"(LLR

AAA = (BBB, BBB)
BBB = (AAA, ZZZ)
ZZZ = (ZZZ, ZZZ))", 6, -2},
};

auto parse(std::string_view s) {
    using ascii::alpha;

    using direction_map_t = std::map<std::string, std::tuple<std::string, std::string>>;
    auto map = std::tuple<std::string, direction_map_t>{};

    phrase_parse(s.begin(), s.end(),
        *alpha >> *(*alpha >> "=" >> directions),
        space, map);
    return map;
}

auto run_a(std::string_view s) {
    auto map = parse(s);
    return -1;
}

auto run_b(std::string_view s) {
    return -1;
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
