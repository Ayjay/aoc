//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"
#include "grid.h"

#include <vector>
#include <tuple>
#include <set>
#include <string_view>
#include <stack>
#include <boost/container/small_vector.hpp>
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/unordered/unordered_flat_set.hpp>

#include <boost/multi_array.hpp>

using result_type = long long;
const auto test_data = std::vector{ std::tuple<std::string_view, std::optional<result_type>, std::optional<result_type>>
{R"(.|...\....
|.-.\.....
.....|-...
........|.
..........
.........\
..../.\\..
.-.-/..|..
.|....-|.\
..//.|....)", 46, 51}
};

inline auto parse_grid_to_array(std::string_view s) {
    const auto lines = get_lines(s);
    return std::tuple{ bounds_t { lines.size(), lines.front().size() }, lines };
}

auto count_energised(auto bounds, const auto& grid, point_t start, direction_t start_dir) {
    const auto [max_rows, max_cols] = bounds;
    const auto in_bounds = [=](point_t p) {
        const auto [row, col] = p;
        return row >= 0 && row < max_rows &&
            col >= 0 && col < max_cols;
        };

    using boost::container::small_vector;
    const auto lase = [&](point_t p, direction_t direction) -> small_vector<direction_t, 2> {
        const auto [row,col] = p;
        switch (grid[row][col]) {
        case '\\':
        if (direction == north) return { west  };
        if (direction == east)  return { south };
        if (direction == south) return { east  };
        if (direction == west)  return { north };
        break;
        case '/':
        if (direction == north) return { east  };
        if (direction == east)  return { north };
        if (direction == south) return { west  };
        if (direction == west)  return { south };
        break;
        case '|':
        if (direction == east) return { north, south };
        if (direction == west) return { north, south };
        break;
        case '-':
        if (direction == north) return { east, west };
        if (direction == south) return { east, west };
        break;
        }
        return { direction };
    };

    auto stack = std::stack<std::tuple<point_t, direction_t>>{};
    stack.push({ start, start_dir });
    auto handled = boost::multi_array<bool, 3>{boost::extents[max_rows][max_cols][4]};

    const auto direction_to_index = [](direction_t direction) {
        if (direction == north) return 0;
        if (direction == south) return 1;
        if (direction == west) return 2;
        if (direction == east) return 3;
        std::unreachable();
    };

    while (!stack.empty()) {
        const auto [point, direction] = stack.top();
        stack.pop();
        const auto [row,col] = point;

        auto& already_done = handled[row][col][direction_to_index(direction)];
        if (already_done)
            continue;

        already_done = true;

        for (auto exit : lase(point, direction)) {
            const auto target = add(point, exit);
            if (in_bounds(target))
                stack.push({ target, exit });
        }
    }

    auto count = 0;
    for (auto row : rv::iota(0, max_rows)) {
        for (auto col : rv::iota(0, max_cols)) {
            for (auto direction : rv::iota(0, 4)) {
                if (handled[row][col][direction]) {
                    ++count;
                    break;
                }
            }
        }
    }
    return count;
}

auto run_a(std::string_view s) {
    const auto [bounds, grid] = parse_grid_to_array(s);

    return count_energised(bounds, grid, { 0,0 }, east);
}

auto run_b(std::string_view s) {
    const auto [bounds, grid] = parse_grid_to_array(s);
    const auto [rows, cols] = bounds;
    const auto top    = rv::iota(0, cols) | rv::transform([&](auto col) { return count_energised(bounds, grid, { 0, col        }, south); }) | ranges::to<std::set>;
    const auto bottom = rv::iota(0, cols) | rv::transform([&](auto col) { return count_energised(bounds, grid, { rows - 1, col }, north); }) | ranges::to<std::set>;
    const auto left   = rv::iota(0, rows) | rv::transform([&](auto row) { return count_energised(bounds, grid, { row, 0        }, east);  }) | ranges::to<std::set>;
    const auto right  = rv::iota(0, rows) | rv::transform([&](auto row) { return count_energised(bounds, grid, { row, cols - 1 }, west);  }) | ranges::to<std::set>;
    const auto highest = std::vector{ *top.rbegin(), *bottom.rbegin(), *left.rbegin(), *right.rbegin() };
    return *ranges::max_element(highest);
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
