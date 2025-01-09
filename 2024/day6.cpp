#include "aoc2024.h"

#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <fmt/core.h>

#include <boost/hana/tuple.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#include <catch2/catch_test_macros.hpp>

#include "grid.hpp"
#include "simple_timer.hpp"

namespace day6 {
using namespace grid;
const auto test_data = std::vector{
    std::tuple<std::string_view, std::optional<i64>, std::optional<i64>>{
        R"(....#.....
.........#
..........
..#.......
.......#..
..........
.#..^.....
........#.
#.........
......#...)",
        41, 6}};

auto run_a(std::string_view s) {
    auto map = grid_t{s};
    auto cells = map.cells();
    auto pos = *ranges::find(cells, '^', map.cell_getter());
    auto facing = up;
    auto visited = boost::unordered_set{pos};
    while (true) {
        auto next = pos + facing;
        auto next_c = map.checked_get(next);
        if (not next_c)
            return visited.size();
        if (*next_c == '#') {
            facing = turn_right(facing);
        } else {
            pos = next;
            visited.insert(pos);
        }
    }
}

constexpr bool debug_print = true;

auto walk(const grid_t& map, vector2 start) {
    auto visited =
        boost::unordered_map<vector2, boost::unordered_set<vector2>>{};
    auto pos = start;
    auto facing = up;
    auto next = std::optional<char>{};
    while ((next = map.checked_get(pos + facing)) and
           visited[pos].insert(facing).second) {
        if (*next == '#')
            facing = turn_right(facing);
        else
            pos = pos + facing;
    }
    return std::tuple{visited, visited[pos].contains(facing)};
}

static auto run_b(std::string_view s) {
    auto map = grid_t{s};
    auto cells = map.cells();
    auto start = *ranges::find(cells, '^', map.cell_getter());
    auto blockers = boost::unordered_set<vector2>{};
    auto [path, _] = walk(map, start);
    for (auto c : path | rv::keys | ranges::to<boost::unordered_set>) {
        map.get(c) = '#';
        auto [_, loop] = walk(map, start);
        if (loop)
            blockers.insert(c);
        map.get(c) = '.';
    }
    return blockers.size();
}

TEST_CASE("day6a", "[day6]") {
    const auto [s, expected, _] = test_data[0];
    if (expected) {
        REQUIRE(run_a(s) == *expected);
    }
}

TEST_CASE("untouched", "[day6]") {
    const auto test_data = R"(....
...#
#^..
..#.)";
    REQUIRE(run_b(test_data) == 1);
}

TEST_CASE("untouched 2", "[day6]") {
    const auto test_data = R"(....
...#
...#
#^..
..#.)";
    REQUIRE(run_b(test_data) == 2);
}

TEST_CASE("reddit testcase", "[day6]") {
    const auto test_data = R"(...........#.....#......
...................#....
...#.....##.............
......................#.
..................#.....
..#.....................
....................#...
........................
.#........^.............
..........#..........#..
..#.....#..........#....
........#.....#..#......)";
    REQUIRE(run_b(test_data) == 19);
}

TEST_CASE("day6b", "[day6]") {
    const auto [s, _, expected] = test_data[0];
    if (expected) {
        REQUIRE(run_b(s) == *expected);
    }
}

}  // namespace day6

WEAK void entry() {
    using namespace day6;
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