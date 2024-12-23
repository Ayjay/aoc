#include "aoc2024.h"

#include <vector>
#include <tuple>
#include <string_view>
#include <utility>
#include <cmath>
#include <list>
#include <unordered_map>

#include <fmt/core.h>

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/multi_array.hpp>

#include <catch2/catch_test_macros.hpp>

#include "grid.hpp"
#include "simple_timer.hpp"

namespace day16 {
using i64 = long long;
const auto test_data = std::vector{ std::tuple<std::string_view, std::optional<i64>, std::optional<i64>>
{R"(###############
#.......#....E#
#.#.###.#.###.#
#.....#.#...#.#
#.###.#####.#.#
#.#.#.......#.#
#.#.#####.###.#
#...........#.#
###.#.#####.#.#
#...#.....#.#.#
#.#.#.###.#.#.#
#.....#...#.#.#
#.###.#.#.#.#.#
#S..#.....#...#
###############)", 7036, {}},
{R"(#################
#...#...#...#..E#
#.#.#.#.#.#.#.#.#
#.#.#.#...#...#.#
#.#.#.#.###.#.#.#
#...#.#.#.....#.#
#.#.#.#.#.#####.#
#.#...#.#.#.....#
#.#.#####.#.###.#
#.#.#.......#...#
#.#.###.#####.###
#.#.#...#.....#.#
#.#.#.#####.###.#
#.#.#.........#.#
#.#.#.#########.#
#S#.............#
#################)", 11048, {}}
};

using namespace grid;

struct vertex {
    vector2 pos;
    vector2 facing;
};

i64 run_a(std::string_view s) {
    const auto maze = grid_t{s};
    const auto start = ranges::find(maze.cells(), 'S', maze.cell_getter());
    const auto end = ranges::find(maze.cells(), 'E', maze.cell_getter());
    const auto can_move = [&](vector2 p) { return maze.get(p) != '#'; }
    const auto vertices = maze.cells() | rv::filter(can_move) | ranges::to<std::vector>;

    const auto edges = [&](vertex v) -> vertex {
        if (can_move(v.pos + turn_left(v.facing)))
            co_yield v.pos + turn_left(v.facing);
        if (can_move(v.pos + v.facing))
            co_yield v.pos + v.facing;
        if (can_move(v.pos + turn_right(v.facing)))
            co_yield v.pos + turn_right(v.facing);
    };


    auto distances = boost::multi_array<i64, 3>{boost::extents[maze.rows][maze.cols][4]};

    return 0;
}

auto run_b(std::string_view s) {
    const auto [grid,moves] = parse(s);
    return b::gps_sum(b::simulate(grid, moves));
}

TEST_CASE("day16a", "[day16]") {
    for (const auto& test : test_data) {
        const auto [s,expected,_] = test;
        if (expected) {
            REQUIRE(run_a(s) == *expected);
        }
    }
}

TEST_CASE("day16b", "[day16]") {
    for (const auto& test : test_data) {
        const auto [s,_,expected] = test;
        if (expected) {
            REQUIRE(run_b(s) == *expected);
        }
    }
}

}

WEAK void entry() {
    using namespace day16;
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