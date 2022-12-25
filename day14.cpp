//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <tuple>
#include <cmath>
#include <array>
#include <iostream>
#include <deque>
#include <map>

#include <fmt/std.h>
#include <fmt/ostream.h>

#include <boost/container_hash/hash.hpp>

#include <boost/fusion/adapted.hpp>
namespace fusion = boost::fusion;

#include <boost/spirit/home/x3.hpp>

namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;

using x3::char_;
using x3::int_;
using x3::long_long;
using ascii::space;
using ascii::lit;
using boost::spirit::x3::phrase_parse;

const auto test_data = std::vector{ std::tuple
{R"(498,4 -> 498,6 -> 496,6
503,4 -> 502,4 -> 502,9 -> 494,9)", 24, -1}
};

enum class tile_t {
    empty,
    rock,
    sand
};
auto run_a(std::string_view s) {
    using point_t = std::pair<int, int>;
    auto scan = std::vector<std::vector<point_t>>{};
    phrase_parse(s.begin(), s.end(),
        *((int_ >> ',' >> int_) % "->"),
        space, scan);
    auto tiles = std::unordered_map<point_t, tile_t, boost::hash<point_t>>{};
    auto lowest_y = std::optional<int>{};
    for (const auto& structure : scan) {
        auto paths = rv::zip(
            ranges::subrange{ structure.begin(), structure.end() - 1 },
            ranges::subrange{ structure.begin() + 1, structure.end() });
        for (auto path : paths) {
            auto from = std::get<0>(path);
            auto to = std::get<1>(path);

            if (!lowest_y || *lowest_y < from.second) {
                lowest_y = from.second;
            }
            if (*lowest_y < to.second) {
                lowest_y = to.second;
            }

            auto step = point_t{ to.first - from.first, to.second - from.second };
            step = { step.first  ? step.first  / abs(step.first)  : 0, 
                     step.second ? step.second / abs(step.second) : 0 };
            for (; from != to; from = { from.first + step.first, from.second + step.second }) {
                tiles[from] = tile_t::rock;
            }
            tiles[to] = tile_t::rock;
        }
    }
    const auto sand_start = point_t{ 500,0 };
    for (auto sand_counter = 0; ; ++sand_counter) {
        auto sand = sand_start;
        auto try_move = [&](point_t to) {
            if (!tiles.contains(to)) {
                sand = to;
                return true;
            } else {
                return false;
            }
        };
        while (sand.second < *lowest_y) {
            if (!try_move({ sand.first, sand.second + 1 }) &&
                !try_move({ sand.first - 1, sand.second + 1 }) &&
                !try_move({ sand.first + 1, sand.second + 1 })) {
                tiles[sand] = tile_t::sand;
                break;
            }
        }
        if (sand.second >= *lowest_y)
            return sand_counter;
    }
}

auto run_b(std::string_view s) {
    return -2;
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
