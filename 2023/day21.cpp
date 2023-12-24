//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"
#include "grid.h"

#include <vector>
#include <tuple>
#include <string_view>
#include <unordered_set>

using result_type = long long;
const auto test_data = std::vector{ std::tuple<std::string_view, std::optional<result_type>, std::optional<result_type>>{}
};

template <class T>
struct speak;

auto possible_count(std::string_view s, auto steps) {
    const auto grid = get_lines(s);

    auto e = enumerate_grid(grid);

    const auto [start, _] = *ranges::find_if(
        e,
        [](const auto& kv) {
            const auto& [p, c] = kv;
            return c == 'S';
        });

    using point_set = std::unordered_set<point_t, boost::hash<point_t>>;
    auto locs = point_set{ start };
    const auto is_inbounds = make_in_bounds(grid);

    for (const auto _ : rv::iota(0, steps)) {
        auto next_locs = point_set{};
        for (const auto l : locs) {
            for (const auto direction : directions) {
                const auto new_pos = add(l, direction);
                const auto [r, c] = new_pos;
                if (is_inbounds(new_pos) && grid[r][c] != '#')
                    next_locs.insert(new_pos);
            }
        }
        locs = std::move(next_locs);
    }

    return locs.size();
}

auto run_a(std::string_view s) {
    return possible_count(s, 64);
}

auto run_b(std::string_view s) {
    return -1;
}

int main() {
    constexpr auto small_grid = R"(...........
.....###.#.
.###.##..#.
..#.#...#..
....#.#....
.##..S####.
.##..#...#.
.......##..
.##.#.####.
.##..##.##.
...........)";

    fmt::println("{} (should be {})", possible_count(small_grid, 6), 16);

    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
