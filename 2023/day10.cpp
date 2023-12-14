//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <vector>
#include <tuple>
#include <string_view>

#include <unordered_set>
#include <unordered_map>
#include <boost/container_hash/hash.hpp>

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
    auto ret = std::unordered_map<point_t, char, boost::hash<point_t>>{};
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
const auto east  = point_t{ 0, 1};
const auto west  = point_t{ 0,-1};
const auto exits = std::unordered_map<char, std::vector<point_t>> {
    {'|', { north, south}},
    {'-', { east,  west }},
    {'L', { north, east }},
    {'J', { north, west }},
    {'7', { south, west }},
    {'F', { south, east }},
    {'S', { north, south, east, west }}
};

point_t add(point_t x, point_t y) {
    return {std::get<0>(x) + std::get<0>(y), std::get<1>(x) + std::get<1>(y)};
}

const auto offset_from = [](point_t from) {
    return [=](point_t p) { return add(from, p); };
};

auto build_loop(const auto& grid) {
    using grid_t = std::decay_t<decltype(grid)>;
    using grid_value_t = grid_t::value_type;

    const auto is_pipe = [&](point_t p) {
        auto it = grid.find(p);
        return it != grid.end() && exits.contains(it->second);
    };

    const auto start = ranges::find(grid, 'S', &grid_value_t::second)->first;
    auto loop_tiles = std::unordered_set<point_t, boost::hash<point_t>>{start};
    const auto not_in_loop = [&](point_t p) {
        return !loop_tiles.contains(p);
    };

    const auto find_pipe_pointing_at = [&](point_t p) {
        for (auto [out,in] : {std::tuple{north,south}, {south,north}, {east,west}, {west,east}}) {
            const auto neighbour = add(p,out);
            if (!is_pipe(neighbour))
                continue;
            if (ranges::contains(exits.at(grid.at(neighbour)), in))
                return neighbour;
        }
        std::unreachable();
    };
    auto tile = find_pipe_pointing_at(start);
    while (true) {
        loop_tiles.insert(tile);
        auto pipe_neighbours = exits.at(grid.at(tile))
            | rv::transform(offset_from(tile))
            | rv::filter(not_in_loop);
        if (ranges::empty(pipe_neighbours))
            break;
        tile = ranges::front(pipe_neighbours);
    }
    return loop_tiles;
}

auto run_a(std::string_view s) {
    const auto grid = parse_grid(s);
    const auto loop = build_loop(grid);
    return loop.size() / 2;
}

auto run_b(std::string_view s) {
    return -1;
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
