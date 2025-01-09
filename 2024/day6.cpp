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
        for (auto _ : rv::iota(0, 2)) {
            auto next = pos + facing;
            auto next_c = map.checked_get(next);
            if (not next_c)
                return visited.size();
            if (*next_c == '#') {
                facing = turn_right(facing);
                continue;
            }
            pos = next;
            visited.insert(pos);
            break;
        }
    }
}

constexpr bool debug_print = true;

static auto run_b(std::string_view s) {
    auto map = grid_t{s};

    auto cells = map.cells();
    auto pos = *ranges::find(cells, '^', map.cell_getter());
    map.get(pos) = '.';
    auto facing = up;
    auto visited = boost::unordered_map<vector2, i64>{std::pair{pos, i64{}}};
    auto corners = 0;
    auto circuit_positions = boost::unordered_set<vector2>{};

    const auto shoot_cornerline_backwards = [&] {
        auto behind = turn_right(turn_right(facing));
        for (auto p = pos + behind; map.checked_get(p) == '.'; p = p + behind)
            visited[p] = corners;
    };
    shoot_cornerline_backwards();
    while (true) {
        for (auto _ : rv::iota(0, 2)) {
            if constexpr (debug_print) {
                std::cout << std::endl;
                for (auto r : rv::iota(i64{}, map.rows)) {
                    for (auto c : rv::iota(i64{}, map.cols)) {
                        if (vector2{r, c} == pos) {
                            std::cout << dir_to_arrow(facing);
                        } else if (auto it = visited.find({r, c});
                                   it != visited.end()) {
                            std::cout << it->second % 10;
                        } else {
                            std::cout << map.map[r][c];
                        }
                    }
                    std::cout << std::endl;
                }
            }
            {
                const auto next = pos + facing;
                const auto [next_row, next_col] = next;
                auto next_c = map.checked_get(next);
                if (not next_c)
                    return circuit_positions.size();
                if (*next_c == '#') {
                    facing = turn_right(facing);
                    ++corners;
                    visited[pos] = corners;
                    shoot_cornerline_backwards();
                    continue;
                }
                pos = next;
            }
            {
                auto it = visited.find(pos);
                if (it != visited.end()) {
                    const auto next = pos + facing;
                    if (it->second == corners - 3 and
                        map.checked_get(next) == '.')
                        circuit_positions.insert(next);
                    it->second = corners;
                } else {
                    visited[pos] = corners;
                }
            }
            break;
        }
    }
}

TEST_CASE("day6a", "[day6]") {
    const auto [s, expected, _] = test_data[0];
    if (expected) {
        REQUIRE(run_a(s) == *expected);
    }
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
    fmt::println("A: {}", run_a(input));
    fmt::println("B: {}", run_b(input));
}