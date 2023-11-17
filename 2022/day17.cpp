//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <tuple>
#include <cmath>
#include <array>
#include <iostream>
#include <deque>
#include <unordered_set>
#include <map>
#include <utility>

#include <fmt/std.h>
#include <fmt/ostream.h>

#include <boost/container_hash/hash.hpp>
#include <boost/icl/interval_set.hpp>

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
{R"(>>><<><>><<<>><>>><<<>>><<<><<<>><>><<>>)", 3068ll, 1514285714288ll}
};

using point_t = std::pair<int64_t, int64_t>;
using shape_t = std::vector<point_t>;

constexpr auto parse_shape(std::string_view s) {
    auto lines = get_lines(s);
    ranges::reverse(lines);
    auto ret = shape_t{};
    for (auto row = 0; row < lines.size(); ++row) {
        for (auto col = 0; col < lines[row].size(); ++col) {
            if (lines[row][col] == '#')
                ret.push_back({ row,col });
        }
    }
    return ret;
}
const auto shapes = [] {
    using namespace std::string_view_literals;
    constexpr auto shape_strs = std::array{
R"(####)"sv,
R"(.#.
###
.#.)"sv,
R"(..#,
..#
###)"sv,
R"(#
#
#
#)"sv,
R"(##
##)"sv };
    auto ret = std::array<shape_t, std::tuple_size_v<decltype(shape_strs)>>{};
    for (int64_t i = 0; i < shape_strs.size(); ++i) {
        ret[i] = parse_shape(shape_strs[i]);
    }
    return ret;
}();

auto run_rock_sim(std::string_view s, int64_t rocks) {
    auto shape_sequence = shapes | rv::cycle;
    auto shape_it = begin(shape_sequence);
    auto jet_sequence = s | rv::cycle;
    auto jet_it = begin(jet_sequence);
    auto max_height = 0ll;

    auto occupied = std::unordered_set<point_t, boost::hash<point_t>>{};

    for (auto placed_shapes = 0ll; placed_shapes < rocks; ++placed_shapes) {
        const auto& shape = *shape_it++;
        auto pos = point_t{max_height+4, 2};

        constexpr bool render = false;
        if (render) {
            auto shape_rows = shape | rv::transform([](auto p) { return p.first; });
            auto shape_height = *ranges::max_element(shape_rows);
            for (int64_t shape_row = shape_height; shape_row >= 0; --shape_row) {
                fmt::print("|");
                for (int64_t col = 0; col < 7; ++col) {
                    if (ranges::find(shape, point_t{ shape_row, col-pos.second }) != shape.end())
                        fmt::print("@");
                    else
                        fmt::print(".");
                }
                fmt::print("|\n");
            }
            for (int64_t row = pos.first - 1; row > 0; --row) {
                fmt::print("|");
                for (int64_t col = 0; col < 7; ++col) {
                    fmt::print("{}", occupied.contains({row,col}) ? '#' : '.');
                }
                fmt::print("|\n");
            }
            fmt::print("+-------+\n");
        }

        auto move_is_blocked = [&](point_t to) {
            auto tile_is_blocked = [&](point_t p) { 
                auto tile_pos = point_t{ to.first + p.first, to.second + p.second };
                return 
                    tile_pos.first <= 0 ||
                    tile_pos.second < 0 || tile_pos.second >= 7 ||
                    occupied.contains(tile_pos); 
            };
            return ranges::any_of(shape, tile_is_blocked);
        };

        while (true) {
            auto dir = *jet_it++ == '<' ? -1 : 1;
            auto blown_pos = point_t{ pos.first, pos.second + dir };
            if (!move_is_blocked(blown_pos))
                pos = blown_pos;

            auto fall_pos = point_t{ pos.first - 1, pos.second };
            if (move_is_blocked(fall_pos)) {
                for (auto p : shape) {
                    auto new_tile = point_t{ pos.first + p.first, pos.second + p.second };
                    occupied.insert(new_tile);
                    max_height = std::max(max_height, new_tile.first);
                }
                break;
            }
            else {
                pos = fall_pos;
            }
        }
    }
    return max_height;
}

auto run_a(std::string_view s) {
    return run_rock_sim(s, 2022);
}

auto run_b(std::string_view s) {
    return run_rock_sim(s, 1000000000000ll);
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
