//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"
#include "grid.h"

#include <vector>
#include <tuple>
#include <string_view>
#include <stack>

#include <boost/multi_array.hpp>
#include <fmt/ranges.h>

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
    const auto max_straight_line = 3;

    using move_t = std::tuple<state_t,state_t>;
    auto s = std::stack<move_t>{};
    s.push({state_t{point_t{0,0}, south, 0, 0}, state_t{point_t{1,0}, south, 1, 0}});
    s.push({state_t{point_t{0,0}, east, 0, 0}, state_t{point_t{0,1}, east, 1, 0}});

    auto cache = boost::multi_array<std::optional<move_t>{boost::extents[max_rows][max_cols][directions.size()][max_straight_line]};
    const auto get_cached_value = [&](state_t state) -> decltype(auto) {
        const auto [row,col] = state.point;
        return cache[row][col][direction_to_index(state.facing)][state.straight_line_distance];
    };

    while (!s.empty()) {
        const auto [from,to] = s.top();
        s.pop();

        const auto [to_row,to_col] = to.point;

        const auto weight = to.accumulated_weight + (grid[to_row][to_col] - '0');
        auto& cached = get_cached_value(state);
        if (cached) {
            const auto& [cached_from, cached_to] = *cached;
            if (cached_to.accumulated_weight < weight)
                continue;
        }

        cached = move_t{from,to};
        if (row == max_rows - 1 && col == max_cols - 1) {
            continue;
        }

        for (const auto direction : directions) {
            if (direction == opposite(to.facing))
                continue;

            const auto new_p = add(to.point, direction);
            if (!in_bounds(new_p))
                continue;

            const auto add_state = [&](state_t state) {
                s.push({to,state});
            };

            if (direction == state.facing) {
                if (state.straight_line_distance < 2)
                    add_state({new_p, direction, state.straight_line_distance + 1, weight});
            } else {
                add_state({new_p, direction, 0, weight});
            }
        }
    }

    // for (const auto row : rv::iota(0, max_rows)) {
    //     auto mins = std::vector<long long>{};
    //     for (const auto col : rv::iota(0, max_cols)) {
    //         mins.push_back(9999999);
    //         for (const auto direction : rv::iota(0u, directions.size())) {
    //             for (auto cached : cache[row][col][direction]) {
    //                 if (cached) {
    //                     const auto [from,to] = *cached;
    //                     mins.back() = std::min(i, mins.back());
    //                 }
    //             }
    //         }
    //     }
    //     fmt::println("{}", mins);
    // }

    // auto first_cells = cache | rv::filter([&](const auto& kv) { 
    //     const auto& [state,weight] = kv;
    //     const auto [row,col] = state.point;
    //     return row == max_rows-1 && col == max_rows-1;
    // });
    auto min = std::optional<long long>{};
    for (const auto direction : rv::iota(0u, directions.size())) {
        for (auto cached : cache[max_rows-1][max_cols-1][direction]) {
            if (cached) {
                const auto [from,to] = *cached;
                if (!min || to.accumulated_weight < *min)
                    min = to.accumulated_weight;
            }
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
