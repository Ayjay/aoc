#include "aoc2024.h"

#include <vector>
#include <tuple>
#include <string_view>
#include <utility>
#include <bitset>
#include <cmath>

#include <fmt/core.h>

#include <boost/hana/tuple.hpp>
#include <boost/unordered_map.hpp>

#include <range/v3/action.hpp>

#include <catch2/catch_test_macros.hpp>

#include "grid.hpp"
#include "simple_timer.hpp"

namespace day8 {
using result_type = long long;
const auto test_data = std::vector{ std::tuple<std::string_view, std::optional<result_type>, std::optional<result_type>>
{R"(............
........0...
.....0......
.......0....
....0.......
......A.....
............
............
........A...
.........A..
............
............)", 14, {}}
};

using namespace hana::literals; 
using namespace grid; 

const auto antinode_pair = [](const auto ps) {
    const auto [a,b] = ps;
    const auto diff = b - a;
    return std::array{a - diff, b + diff};
};

TEST_CASE("antinode pair", "[day8]") {
    REQUIRE(antinode_pair(std::tuple{vector2{3,4}, vector2{5,5}}) == std::array{vector2{1,3}, vector2{7,6}});
}

const auto antinodes = [](const auto& as) {
    const auto less_than = [](const auto ps) {
        const auto [a,b] = ps;
        return a < b;
    };
    return rv::cartesian_product(as, as)
        | rv::filter(less_than)
        | rv::transform(antinode_pair)
        | rv::join;
};

TEST_CASE("antinodes", "[day8]") {
    REQUIRE(ranges::equal(
        antinodes(std::vector{vector2{3,4}, vector2{5,5}}),
        std::array{vector2{1,3}, vector2{7,6}}));
}

auto run_a(std::string_view s) {
    const auto map = get_lines(s);
    const auto get = indexer(map);
    const auto rows = map.size();
    const auto cols = map.front().size();
    auto antennae = boost::unordered_map<char, std::vector<vector2>>{};
    for (auto p : rv::cartesian_product(rv::iota(0u,rows), rv::iota(0u,cols))) {
        char c = get(p);
        if (c != '.') antennae[c].push_back(p);
    }
    const auto on_map = [&](vector2 pos) {
        const auto [row,col] = pos;
        return row >= 0 and row < rows and col >= 0 and col < cols;
    };

    auto all_antinodes = antennae
        | rv::values
        | rv::transform(antinodes)
        | rv::join
        | rv::filter(on_map)
        | ranges::to<std::vector>;
    ranges::sort(all_antinodes);
    return ranges::distance(all_antinodes | rv::unique);
}

static auto run_b(std::string_view s) {
    return -1;
}

TEST_CASE("day8a", "[day8]") {
    const auto [s,expected,_] = test_data[0];
    if (expected) {
        REQUIRE(run_a(s) == *expected);
    }
}

TEST_CASE("day8b", "[day8]") {
    const auto [s,_,expected] = test_data[0];
    if (expected) {
        REQUIRE(run_b(s) == *expected);
    }
}

}

WEAK void entry() {
    using namespace day8;
    const auto input = get_input(AOC_DAY);
    {
        auto t = SimpleTimer("Part A");
        fmt::println("A: {}", run_a(input));
    }
    {
        auto t = SimpleTimer("Part B");
        fmt::println("B: {}", run_b(input));
    }
}