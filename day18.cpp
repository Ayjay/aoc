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

using point_t = std::tuple<int,int,int>;
auto& x(point_t& p) { return std::get<0>(p); }
auto& y(point_t& p) { return std::get<1>(p); }
auto& z(point_t& p) { return std::get<2>(p); }

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

const auto directions = std::array<point_t, 6> {
    point_t { -1,  0,  0 },
    point_t {  1,  0,  0 },
    point_t {  0, -1,  0 },
    point_t {  0,  1,  0 },
    point_t {  0,  0, -1 },
    point_t {  0,  0,  1 }
};

auto run_a(std::string_view s) {
    auto cubes = parse(s);
    auto cubes_copy = cubes | ranges::to<std::vector>();
    auto get_sides = [&](auto cube) {
        return directions | rv::transform([=](auto dir) { return cube + dir; });
    };
    auto sides = cubes | rv::transform(get_sides) | rv::join | ranges::to<std::vector>();
    std::erase_if(sides, [&](auto cube) { return cubes.count(cube); });
    return sides.size();
}

auto run_b(std::string_view s) {
    return -2;
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
