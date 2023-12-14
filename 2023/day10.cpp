//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <vector>
#include <tuple>
#include <string_view>

using result_type = long long;
const auto test_data = std::vector{ std::tuple<std::string_view, std::optional<result_type>, std::optional<result_type>>
{R"(.....
.S-7.
.|.|.
.L-J.
.....)", 4, {}},
{R"(..F7.
.FJ|.
SJ.L7
|F--J
LJ...)", 8, {}}
};

using point_t = std::tuple<int,int>;
auto parse_grid(std::string_view s) {
    auto ret = std::unordered_map<point_t, char>{};
    const auto lines = get_lines(s);
    for (const auto [row,line] : rv::enumerate(lines)) {
        for (auto [col,c] : rv::enumerate(line)) {
            ret[{row,col}] = c;
        }
    }
    return ret;
}

const auto north = point_t{-1, 0};
const auto south = point_t{ 1, 0};
const auto east  = point_t{ 0,-1};
const auto west  = point_t{ 0, 1};
const auto exits = std::unordered_map<char, std::array<point_t, 2>> {
    {'|', { north, south}},
    {'-', { east,  west }},
    {'L', { north, east }},
    {'J', { north, west }},
    {'7', { south, west }},
    {'F', { south, east }},
};

auto run_a() {
    const auto grid = parse_grid(s);
    const auto start = ranges::find(grid, &grid::value_type::second, 'S')->first;
    auto loop_tiles = std::unordered_set<point_t>{start};
    for (auto dir : {north,south,east,west}) {
        const auto next_tile_pos = start + dir;
        const auto next_tile_it = grid.find(next_tile_pos);
        if (next_tile_it == grid.end())
            continue;
        auto next_tile_exits = exits.find(next_tile_it->second);
        if (next_tile_exits == exits.end())
            continue;

        loop_tiles.insert(next_tile_it->first);
        do {
            for ()
        }
        break;
    }
    return -1;
}

auto run_b(std::string_view s) {
    return -1;
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
