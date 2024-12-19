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

#include <catch2/catch_test_macros.hpp>

#include "grid.hpp"
#include "simple_timer.hpp"

namespace day15 {
using i64 = long long;
const auto test_data = std::vector{ std::tuple<std::string_view, std::optional<i64>, std::optional<i64>>
{R"(########
#..O.O.#
##@.O..#
#...O..#
#.#.O..#
#...O..#
#......#
########

<^^>>>vv<v>>v<<
)", 2028, {}},
{R"(##########
#..O..O.O#
#......O.#
#.OO..O.O#
#..O@..O.#
#O#..O...#
#O..O..O.#
#.OO.O.OO#
#....O...#
##########

<vv>^<v^>v>^vv^v>v<>v^v<v<^vv<<<^><<><>>v<vvv<>^v^>^<<<><<v<<<v^vv^v>^
vvv<<^>^v^^><<>>><>^<<><^vv^^<>vvv<>><^^v>^>vv<>v<<<<v<^v>^<^^>>>^<v<v
><>vv>v^v^<>><>>>><^^>vv>v<^^^>>v^v^<^^>v^^>v^<^v>v<>>v^v^<v>v^^<^^vv<
<<v<^>>^^^^>>>v^<>vvv^><v<<<>^^^vv^<vvv>^>v<^^^^v<>^>vvvv><>>v^<<^^^^^
^><^><>>><>^^<<^^v>>><^<v>^<vv>>v>>>^v><>^v><<<<v>>v<v<v>vvv>^<><<>^><
^>><>^v<><^vvv<^^<><v<<<<<><^v<<<><<<^^<v<^^^><^>>^<v^><<<^>>^v<v^v<v^
>^>>^v>vv>^<<^v<>><<><<v<<v><>v<^vv<<<>^^v^>^^>>><<^v>>v^v><^^>>^<>vv^
<><^^>^^^<><vvvvv^v<v<<>^v<v>v<<^><<><<><<<^^<<<^<<>><<><^^^>^^<>^>v<>
^^>vv<^v^v<vv>^<><v<^v>^^^>>>^^vvv^>vvv<>>>^<^>>>>>^<<^v>^vvv<>^<><<v>
v^^>>><<^^<>>^v^<v^vv<>v^<<>^<^v^v><^<<<><<^<v><v<>vv>>v><v^<vv<>v^<<^
)", 10092, {}}
};

constexpr auto EMPTY = '.';
constexpr auto BOX = 'O';
constexpr auto ROBOT = '@';
constexpr auto WALL = '#';

using namespace grid;
auto parse(std::string_view s) {
    using namespace std::string_view_literals;
    auto newlines = ranges::search(s, "\n\n"sv);
    return std::tuple{grid_t{{s.begin(), newlines.begin()}}, std::string_view{newlines.end(), s.end()}};
}

auto gps_sum(const auto& grid) {
    auto boxes = grid.cells() | rv::filter([&](auto p) {return grid.get(p) == BOX; });
    const auto gps_coord = [&](auto p) -> i64 {
        const auto [row,col] = p;
        return 100*row+col;
    };
    return reduce(boxes | rv::transform(gps_coord));
}

const auto get_dir_from_move = [](char c) -> vector2 {
    switch (c) {
        case '<': return left;
        case '>': return right;
        case '^': return up;
        case 'v': return down;
    }
    assert(false);
    std::unreachable();
};

const auto is_dir = [](char c) {
    switch (c) {
        case '<':
        case '>':
        case '^':
        case 'v': return true;
    }
    return false;
};

auto simulate(auto grid, const auto& moves) {
    auto c = grid.cells();
    auto pos = *ranges::find_if(c, [&](vector2 p) { return grid.get(p) == ROBOT; });
    grid.get(pos) = EMPTY;
    const auto make_move = [&](vector2 dir) {
        auto next_pos = pos+dir;
        auto& next_tile = grid.get(next_pos);
        if (next_tile == EMPTY) {
            pos = next_pos;
        } else if (next_tile == BOX) {
            auto last_box = next_pos+dir;
            while (grid.get(last_box) == BOX)
                last_box = last_box+dir;
            if (grid.get(last_box) == EMPTY) {
                pos = next_pos;
                next_tile = EMPTY;
                grid.get(last_box) = BOX;
            }
        }
    };
    for (auto move : moves | rv::filter(is_dir) | rv::transform(get_dir_from_move)) {
        make_move(move);
    }
    return grid;
}

auto run_a(std::string_view s) {
    const auto [grid,moves] = parse(s);
    return gps_sum(simulate(grid, moves));
}

auto run_b(std::string_view s) {
    return -1;
}

TEST_CASE("day15a", "[day15]") {
    for (const auto& test : test_data) {
        const auto [s,expected,_] = test;
        if (expected) {
            REQUIRE(run_a(s) == *expected);
        }
    }
}

TEST_CASE("day15b", "[day15]") {
    for (const auto& test : test_data) {
        const auto [s,_,expected] = test;
        if (expected) {
            REQUIRE(run_b(s) == *expected);
        }
    }
}

}

WEAK void entry() {
    using namespace day15;
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