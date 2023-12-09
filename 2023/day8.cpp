//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <vector>
#include <tuple>
#include <string_view>
#include <boost/unordered_map.hpp>

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

    auto instructions = std::string{};
    auto it = s.begin();
    phrase_parse(it, s.end(),
        x3::lexeme[*alpha],
        space, instructions);

    auto graph = std::vector<std::tuple<std::string, std::string, std::string>>{};
    phrase_parse(it, s.end(),
        *(*alpha >> '=' >> lit('(') >> *alpha >> ',' >> *alpha >> ')'),
        space, graph);

    auto to_map = graph
        | rv::transform([](auto v) {
            const auto& [node, left, right] = v;
            return std::pair{ node, std::tuple{left,right} };
            })
        | ranges::to<std::unordered_map>;

    return std::tuple{ instructions, to_map };
}

auto run_a(std::string_view s) {
    const auto [instructions,map] = parse(s);
    auto steps = 0;
    auto node = std::string{ "AAA" };
    while (node != "ZZZ") {
        const auto& [left, right] = map.at(node);
        auto direction = instructions[steps % instructions.size()];
        if (direction == 'L')
            node = left;
        else
            node = right;
        ++steps;
    }

    return steps;
}

auto run_b(std::string_view s) {
    return -1;
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
