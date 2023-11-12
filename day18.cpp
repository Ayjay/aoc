//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <tuple>
#include <cmath>
#include <array>
#include <iostream>
#include <deque>
#include <unordered_set>
#include <map>
#include <valarray>

#include <fmt/std.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>

#include <boost/array.hpp>

#include <boost/unordered_set.hpp>
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
{R"(1,1,1
2,1,1)", 10, 10
},
{R"(2,2,2
1,2,2
3,2,2
2,1,2
2,3,2
2,2,1
2,2,3
2,2,4
2,2,6
1,2,5
3,2,5
2,1,5
2,3,5)", 64, 58}
};

using point_t = boost::array<int,3>;
auto& x(point_t& p) { return p[0]; }
auto& y(point_t& p) { return p[1]; }
auto& z(point_t& p) { return p[2]; }
const auto& x(const point_t& p) { return p[0]; }
const auto& y(const point_t& p) { return p[1]; }
const auto& z(const point_t& p) { return p[2]; }

point_t operator+(point_t a, point_t b) {
    return point_t{ x(a) + x(b), y(a) + y(b), z(a) + z(b) }; 
}

auto parse(auto s) {
    boost::unordered_set<point_t> cubes;
    phrase_parse(s.begin(), s.end(),
        *(int_ >> ',' >> int_ >> ',' >> int_),
        space, cubes);
    return cubes;
}

constexpr auto directions = std::array<point_t, 6> {
    point_t { -1,  0,  0 },
    point_t {  1,  0,  0 },
    point_t {  0, -1,  0 },
    point_t {  0,  1,  0 },
    point_t {  0,  0, -1 },
    point_t {  0,  0,  1 }
};
auto get_sides = [](auto cube) {
    return directions | rv::transform([=](auto dir) { return cube + dir; });
};

auto run_a(std::string_view s) {
    auto cubes = parse(s);
    auto cube_empty = [&](auto cube) { return !cubes.contains(cube); };
    auto sides = cubes | rv::transform(get_sides) | rv::join;
    return ranges::count_if(sides, cube_empty);
}

auto get_boundary_tiles(auto min, auto max) {
    auto tiles = boost::unordered_set<point_t>{};
    constexpr auto dimension_slices = std::array{
        std::tuple{0, std::array{1,2}},
        std::tuple{1, std::array{0,2}},
        std::tuple{2, std::array{0,1}}
    };
    for (auto [dim1, other_dims] : dimension_slices) {
        auto [dim2,dim3] = other_dims;
        for (auto a : std::array{min[dim1], max[dim1]}) {
            for (auto b : rv::iota(min[dim2],max[dim2]+1)) {
                for (auto c : rv::iota(min[dim3],max[dim3]+1)) {
                    point_t p;
                    p[dim1] = a;
                    p[dim2] = b;
                    p[dim3] = c;
                    tiles.insert(p);
                }
            }
        }
    }
    return tiles;
}

auto get_bounding_cube(const auto& cubes) {
    using extents = std::tuple<point_t, point_t>;
    const auto first = *cubes.begin();
    return ranges::accumulate(
        cubes, extents{first,first},
        [](extents acc, point_t p) { 
            auto [min,max] = acc;
            x(min) = std::min(x(min), x(p));
            y(min) = std::min(y(min), y(p));
            z(min) = std::min(z(min), z(p));
            x(max) = std::max(x(max), x(p));
            y(max) = std::max(y(max), y(p));
            z(max) = std::max(z(max), z(p));
            return extents{min,max};
        }
    );
}

auto run_b(std::string_view s) {
    const auto cubes = parse(s);
    const auto cubes_copy = cubes | ranges::to<std::vector>();

    const auto [min,max] = get_bounding_cube(cubes);

    const auto inside_bounds = [&](point_t p) {
        return x(p) >= x(min) && x(p) <= x(max) &&
            y(p) >= y(min) && y(p) <= y(max) &&
            z(p) >= z(min) && z(p) <= z(max);
    };

    const auto get_outside_points = [&]() {
        auto accessible = get_boundary_tiles(min,max) | ranges::to<std::deque>();
        auto outside = boost::unordered_set<point_t>{};

        while (!accessible.empty()) {
            const auto p = accessible.front();
            accessible.pop_front();

            if (!inside_bounds(p))
                continue;

            if (outside.contains(p))
                continue;

            if (cubes.contains(p))
                continue;
            
            outside.insert(p);
            const auto neighbours = directions | rv::transform([=](auto dir) { return p + dir; });
            accessible.insert(accessible.end(), neighbours.begin(), neighbours.end());
        }

        return outside;
    };
    const auto outside = get_outside_points();
    const auto outside_copy = outside | ranges::to<std::vector>();
    const auto is_outside = [&](auto cube) { 
        return !cubes.contains(cube) && 
        (outside.contains(cube) || !inside_bounds(cube)); 
    };
    auto sides = cubes | rv::transform(get_sides) | rv::join;
    return ranges::count_if(sides, is_outside);
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
