#include "aoc2024.h"

#include <cmath>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>
#include <set>

#include <fmt/core.h>

#include <boost/unordered_set.hpp>
#include <boost/container/static_vector.hpp>

#include <catch2/catch_test_macros.hpp>

#include "grid.hpp"
#include "simple_timer.hpp"

namespace day15 {
using i64 = long long;
const auto test_data = std::vector{
    std::tuple<std::string_view, std::optional<i64>, std::optional<i64>>{
        R"(########
#..O.O.#
##@.O..#
#...O..#
#.#.O..#
#...O..#
#......#
########

<^^>>>vv<v>>v<<
)",
        2028,
        {}},
    {R"(#######
#...#.#
#.....#
#..OO@#
#..O..#
#.....#
#######

<vv<<^^<<^^
)",
     {},
     {}},
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
)",
     10092, 9021}};

constexpr auto EMPTY = '.';
constexpr auto ROBOT = '@';
constexpr auto WALL = '#';

using namespace grid;
auto parse(std::string_view s) {
    using namespace std::string_view_literals;
    auto newlines = ranges::search(s, "\n\n"sv);
    return std::tuple{grid_t{{s.begin(), newlines.begin()}},
                      std::string_view{newlines.end(), s.end()}};
}

const auto get_dir_from_move = [](char c) -> vector2 {
    switch (c) {
        case '<':
            return left;
        case '>':
            return right;
        case '^':
            return up;
        case 'v':
            return down;
    }
    assert(false);
    std::unreachable();
};

const auto is_dir = [](char c) {
    switch (c) {
        case '<':
        case '>':
        case '^':
        case 'v':
            return true;
    }
    return false;
};

namespace a {
constexpr auto BOX = 'O';
auto gps_sum(const auto& grid) {
    auto boxes =
        grid.cells() | rv::filter([&](auto p) { return grid.get(p) == BOX; });
    const auto gps_coord = [&](auto p) -> i64 {
        const auto [row, col] = p;
        return 100 * row + col;
    };
    return reduce(boxes | rv::transform(gps_coord));
}

auto simulate(auto grid, const auto& moves) {
    auto c = grid.cells();
    auto pos =
        *ranges::find_if(c, [&](vector2 p) { return grid.get(p) == ROBOT; });
    grid.get(pos) = EMPTY;
    const auto make_move = [&](vector2 dir) {
        auto next_pos = pos + dir;
        auto& next_tile = grid.get(next_pos);
        if (next_tile == EMPTY) {
            pos = next_pos;
        } else if (next_tile == BOX) {
            auto last_box = next_pos + dir;
            while (grid.get(last_box) == BOX)
                last_box = last_box + dir;
            if (grid.get(last_box) == EMPTY) {
                pos = next_pos;
                next_tile = EMPTY;
                grid.get(last_box) = BOX;
            }
        }
    };
    for (auto move :
         moves | rv::filter(is_dir) | rv::transform(get_dir_from_move)) {
        make_move(move);
    }
    return grid;
}
}  // namespace a

auto run_a(std::string_view s) {
    const auto [grid, moves] = parse(s);
    return a::gps_sum(a::simulate(grid, moves));
}

namespace b {

constexpr auto BOX_L = '[';
constexpr auto BOX_R = ']';
const auto is_box = [](char c) { return c == BOX_L or c == BOX_R; };

auto gps_sum(const auto& grid) {
    auto boxes =
        grid.cells() | rv::filter([&](auto p) { return grid.get(p) == BOX_L; });
    const auto gps_coord = [&](auto p) -> i64 {
        const auto [row, col] = p;
        return 100 * row + col;
    };
    return reduce(boxes | rv::transform(gps_coord));
}

const auto expand_char = [](char c) -> std::string_view {
    switch (c) {
        case '#':
            return "##";
        case '.':
            return "..";
        case 'O':
            return "[]";
        case '@':
            return "@.";
    }
    assert(false);
    std::unreachable();
};

auto expand_grid(auto grid) {
    for (auto& row : grid.map) {
        row = row | rv::transform(expand_char) | rv::join |
              ranges::to<std::string>;
    }
    grid.cols *= 2;
    return grid;
}

auto simulate(auto grid, const auto& moves) {
    grid = expand_grid(std::move(grid));
    auto c = grid.cells();
    auto pos =
        *ranges::find_if(c, [&](vector2 p) { return grid.get(p) == ROBOT; });
    grid.get(pos) = EMPTY;
    const auto fix_box_pos = [&](vector2 p) {
        if (grid.get(p) == BOX_L)
            return p;
        assert(grid.get(p) == BOX_R);
        return p + left;
    };
    const auto make_move = [&](vector2 dir) {
        auto next_pos = pos + dir;
        auto& next_tile = grid.get(next_pos);
        if (next_tile == EMPTY) {
            pos = next_pos;
        } else if (is_box(next_tile)) {
            const auto is_vertical = dir == up or dir == down;
            const auto compare = [&](vector2 p, vector2 q) {
                const auto [p_r,p_c] = p;
                const auto [q_r,q_c] = q;
                if (dir == up)
                    return std::tie(p_r,p_c) < std::tie(q_r,q_c);
                else if (dir == down)
                    return std::tie(p_r,p_c) > std::tie(q_r,q_c);
                else if (dir == left)
                    return std::tie(p_c,p_r) < std::tie(q_c,q_r);
                else
                    return std::tie(p_c,p_r) > std::tie(q_c,q_r);
            };
            auto boxes = std::set<vector2, decltype(compare)>{compare};
            auto found_boxes = boost::unordered_set<vector2>{fix_box_pos(next_pos)};
            auto blocked = false;
            while (not blocked and not found_boxes.empty()) {
                const auto box_it = found_boxes.begin();
                const auto box = *box_it;
                found_boxes.erase(box_it);
                assert(grid.get(box) == BOX_L);
                if(not boxes.insert(box).second)
                    continue;

                auto tiles_to_check = boost::container::static_vector<vector2,2>{};
                if (is_vertical) {
                    tiles_to_check.push_back(box+dir);
                    tiles_to_check.push_back(box+right+dir);
                } else if (dir == left) {
                    tiles_to_check.push_back(box+left);
                } else {
                    assert(dir == right);
                    tiles_to_check.push_back(box+right+right);
                }

                for (const auto tile : tiles_to_check) {
                    if (grid.get(tile) == WALL) {
                        blocked = true;
                        break;
                    } else if (is_box(grid.get(tile))) {
                        found_boxes.insert(fix_box_pos(tile));
                    }
                }
            }

            if (not blocked) {
                for (const auto box : boxes) {
                    if (is_vertical) {
                        grid.get(box + dir) = BOX_L;
                        grid.get(box) = EMPTY;
                        grid.get(box + right + dir) = BOX_R;
                        grid.get(box + right) = EMPTY;
                    } else if (dir == left) {
                        grid.get(box + left) = BOX_L;
                        grid.get(box) = BOX_R;
                        grid.get(box + right) = EMPTY;
                    } else {
                        assert(dir == right);
                        grid.get(box) = EMPTY;
                        grid.get(box+right) = BOX_L;
                        grid.get(box+right+right) = BOX_R;
                    }
                }
                pos = pos + dir;
            }
        }
        constexpr bool show_grid = false;
        if constexpr (show_grid) {
            fmt::println("--------------");
            assert(grid.get(pos) == EMPTY);
            grid.get(pos) = ROBOT;
            for (const auto row : grid.map) {
                fmt::println("{}", row);
            }
            grid.get(pos) = EMPTY;
            fmt::println("--------------");
        }
    };
    for (auto move :
         moves | rv::filter(is_dir) | rv::transform(get_dir_from_move)) {
        make_move(move);
    }
    return grid;
}
}  // namespace b

auto run_b(std::string_view s) {
    const auto [grid, moves] = parse(s);
    return b::gps_sum(b::simulate(grid, moves));
}

TEST_CASE("day15a", "[day15]") {
    for (const auto& test : test_data) {
        const auto [s, expected, _] = test;
        if (expected) {
            REQUIRE(run_a(s) == *expected);
        }
    }
}

TEST_CASE("day15b", "[day15]") {
    for (const auto& test : test_data) {
        const auto [s, _, expected] = test;
        if (expected) {
            REQUIRE(run_b(s) == *expected);
        }
    }
}

}  // namespace day15

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