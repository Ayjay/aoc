//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"
#include "grid.h"

#include <vector>
#include <tuple>
#include <string_view>
#include <stack>

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
..//.|....)", 46, {}}
};

auto run_a(std::string_view s) {
    const auto [bounds, grid] = parse_grid(s);

    const auto in_bounds = [=](point_t p) {
        const auto [max_rows, max_cols] = bounds;
        const auto [row, col] = p;
        return row >= 0 && row < max_rows &&
            col >= 0 && col < max_cols;
        };

    const auto lase = [&](point_t p, direction_t direction) -> std::vector<direction_t> {
        switch (grid.at(p)) {
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
    stack.push({ { 0,0 }, east });
    auto energised = std::unordered_set<point_t, boost::hash<point_t>>{};
    auto handled = std::unordered_set<std::tuple<point_t, direction_t>, boost::hash<std::tuple<point_t, direction_t>>>{};

    while (!stack.empty()) {
        const auto [point, direction] = stack.top();
        stack.pop();
        if (!handled.insert({ point,direction }).second)
            continue;
        energised.insert(point);

        for (auto exit : lase(point, direction)) {
            const auto target = add(point, exit);
            if (in_bounds(target))
                stack.push({ target, exit });
        }
    }

    return energised.size();
}

auto run_b(std::string_view s) {
    return -1;
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
