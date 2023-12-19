//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"
#include "grid.h"

#include <vector>
#include <tuple>
#include <string_view>
#include <stack>

#include <boost/multi_array.hpp>

using result_type = long long;
const auto test_data = std::vector{ std::tuple<std::string_view, std::optional<result_type>, std::optional<result_type>>
{R"(11111
55551
55551
55551
55552)", 13, {}},
{R"(2413432311323
3215453535623
3255245654254
3446585845452
4546657867536
1438598798454
4457876987766
3637877979653
4654967986887
4564679986453
1224686865563
2546548887735
4322674655533)", 102, {}}
};

struct state_t {
    point_t point;
    direction_t facing;
    long long straight_line_distance = 0;
    long long accumulated_weight = 0;

    auto tie() const {
        return std::tie(point, facing, straight_line_distance, accumulated_weight);
    }
};

bool operator==(const state_t& left, const state_t& right) {
    return left.tie() == right.tie();
}

std::size_t hash_value(const state_t& state) {
    auto seed = static_cast<std::size_t>(0);
    boost::hash_combine(seed, state.point);
    boost::hash_combine(seed, state.facing);
    boost::hash_combine(seed, state.straight_line_distance);
    boost::hash_combine(seed, state.accumulated_weight);
    return seed;
}

auto get_min_loss(const auto& grid) {
    const auto [max_rows,max_cols] = get_bounds(grid);
    const auto in_bounds = make_in_bounds(grid);

    auto s = std::stack<state_t>{};
    s.push(state_t{point_t{1,0}, south, 0, 0});
    s.push(state_t{point_t{0,1}, east, 0, 0});
    const auto max_straight_line = 3;
    auto cache = boost::multi_array<long long, 4>{boost::extents[max_rows][max_cols][directions.size()][max_straight_line]};

    while (!s.empty()) {
        const auto state = s.top();
        s.pop();

        const auto [row,col] = state.point;

        const auto weight = state.accumulated_weight + (grid[row][col] - '0');
        const auto get_cached_value = [&](state_t state) -> decltype(auto) {
            return cache[row][col][direction_to_index(state.facing)][state.straight_line_distance];
        };
        auto& cached = get_cached_value(state);
        if (cached != 0 && cached < weight)
            continue;

        cached = weight;
        if (row == max_rows-1 && col == max_cols-1)
        continue;

        for (const auto direction : directions) {
            if (direction == opposite(state.facing))
                continue;

            const auto new_p = add(state.point, direction);
            if (!in_bounds(new_p))
                continue;

            const auto add_state = [&](state_t state) {
                const auto& cached = get_cached_value(state);
                if (cached == 0 || cached > state.accumulated_weight)
                    s.push(state);
            };

            if (direction == state.facing) {
                if (state.straight_line_distance < 2)
                    add_state({new_p, direction, state.straight_line_distance + 1, weight});
            } else {
                add_state({new_p, direction, 0, weight});
            }
        }
    }

    // auto first_cells = cache | rv::filter([&](const auto& kv) { 
    //     const auto& [state,weight] = kv;
    //     const auto [row,col] = state.point;
    //     return row == max_rows-1 && col == max_rows-1;
    // });
    auto min = std::optional<long long>{};
    for (const auto direction : rv::iota(0u, directions.size())) {
        for (auto n : cache[max_rows-1][max_cols-1][direction]) {
            if (n != 0 && (!min || n < *min))
                min = n;
        }
    }
    return *min;
}

auto run_a(std::string_view s) {
    return get_min_loss(get_lines(s));
}

auto run_b(std::string_view s) {
    return -1;
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
