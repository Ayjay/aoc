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

namespace day6 {
using result_type = long long;
const auto test_data =
    std::vector{std::tuple<std::string_view,
                           std::optional<result_type>,
                           std::optional<result_type>>{R"(....#.....
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

const auto up = std::tuple{-1, 0};
const auto down = std::tuple{1, 0};
const auto left = std::tuple{0, -1};
const auto right = std::tuple{0, 1};
auto turn_right(auto facing) {
    if (facing == up)
        return right;
    else if (facing == right)
        return down;
    else if (facing == down)
        return left;
    else
        return up;
}
auto add_pos(auto a, auto b) {
    const auto [a_r, a_c] = a;
    const auto [b_r, b_c] = b;
    return std::tuple{a_r + b_r, a_c + b_c};
}

auto run_a(std::string_view s) {
    auto map = get_lines(s) | ranges::to<std::vector<std::string>>;
    const auto rows = map.size();
    const auto cols = map.front().size();

    const auto on_map = [&](const auto p) {
        const auto [row, col] = p;
        return row >= 0 and row < rows and col >= 0 and col < cols;
    };

    auto pos = [&] {
        auto positions =
            rv::cartesian_product(rv::iota(0u, rows), rv::iota(0u, cols));
        const auto is_guard = [&](const auto pos) {
            const auto [row, col] = pos;
            return map[row][col] == '^';
        };
        return *ranges::find_if(positions, is_guard);
    }();
    auto& [row, col] = pos;
    map[row][col] = '.';
    auto facing = up;
    auto visited = boost::unordered_set{pos};
    while (true) {
        for (auto _ : rv::iota(0, 2)) {
            auto next = add_pos(pos, facing);
            const auto [next_row, next_col] = next;
            if (!on_map(next))
                return visited.size();
            if (map[next_row][next_col] == '#') {
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
    auto map = get_lines(s) | ranges::to<std::vector<std::string>>;
    const auto rows = map.size();
    const auto cols = map.front().size();

    const auto on_map = [&](const auto p) {
        const auto [row, col] = p;
        return row >= 0 and row < rows and col >= 0 and col < cols;
    };

    auto pos = [&] {
        auto positions =
            rv::cartesian_product(rv::iota(0u, rows), rv::iota(0u, cols));
        const auto is_guard = [&](const auto pos) {
            const auto [row, col] = pos;
            return map[row][col] == '^';
        };
        return *ranges::find_if(positions, is_guard);
    }();
    auto& [row, col] = pos;
    map[row][col] = '.';
    auto facing = up;
    auto visited = boost::unordered_map{std::pair{pos, i64{}}};
    auto corners = 0;
    auto circuit_positions = i64{};
    while (true) {
        for (auto _ : rv::iota(0, 2)) {
            if constexpr (debug_print) {
                std::cout << std::endl;
                for (auto r : rv::iota(0u, rows)) {
                    for (auto c : rv::iota(0u, cols)) {
                        if (row == r and col == c) {
                            if (facing == up)
                                std::cout << '^';
                            else if (facing == down)
                                std::cout << 'v';
                            else if (facing == left)
                                std::cout << '<';
                            else if (facing == right)
                                std::cout << '>';
                        } else if (auto it = visited.find(std::tuple{r, c});
                                   it != visited.end()) {
                            std::cout << it->second % 10;
                        } else {
                            std::cout << map[r][c];
                        }
                    }
                    std::cout << std::endl;
                }
            }
            {
                const auto next = add_pos(pos, facing);
                const auto [next_row, next_col] = next;
                if (!on_map(next))
                    return circuit_positions;
                if (map[next_row][next_col] == '#') {
                    facing = turn_right(facing);
                    ++corners;
                    continue;
                }
                pos = next;
            }
            {
                auto it = visited.find(pos);
                if (it != visited.end()) {
                    const auto next = add_pos(pos, facing);
                    const auto [next_row, next_col] = next;
                    if (it->second == corners - 3 and on_map(next) and
                        map[next_row][next_col] != '#')
                        ++circuit_positions;
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