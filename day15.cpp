//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <tuple>
#include <cmath>
#include <array>
#include <iostream>
#include <deque>
#include <unordered_set>
#include <map>

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
{R"(Sensor at x=2, y=18: closest beacon is at x=-2, y=15
Sensor at x=9, y=16: closest beacon is at x=10, y=16
Sensor at x=13, y=2: closest beacon is at x=15, y=3
Sensor at x=12, y=14: closest beacon is at x=10, y=16
Sensor at x=10, y=20: closest beacon is at x=10, y=16
Sensor at x=14, y=17: closest beacon is at x=10, y=16
Sensor at x=8, y=7: closest beacon is at x=2, y=10
Sensor at x=2, y=0: closest beacon is at x=2, y=10
Sensor at x=0, y=11: closest beacon is at x=2, y=10
Sensor at x=20, y=14: closest beacon is at x=25, y=17
Sensor at x=17, y=20: closest beacon is at x=21, y=22
Sensor at x=16, y=7: closest beacon is at x=15, y=3
Sensor at x=14, y=3: closest beacon is at x=15, y=3
Sensor at x=20, y=1: closest beacon is at x=15, y=3)", 26ll, 56000011ll}
};

using point_t = std::pair<int64_t, int64_t>;
const x3::rule<class point_, point_t> point_ = "point";
const auto point__def = "x=" >> long_long >> ", y=" >> long_long;
BOOST_SPIRIT_DEFINE(point_)

using scan_t = std::vector<std::pair<point_t, point_t>>;

auto parse(std::string_view s) {
    auto scan = scan_t{};
    phrase_parse(s.begin(), s.end(),
        *("Sensor at" >> point_ >> ": closest beacon is at" >> point_),
        space, scan);
    return scan;
}

auto impossible(const scan_t& scan, int row) {
    auto overlaps = boost::icl::interval_set<int>{};
    for (const auto [sensor, beacon] : scan) {
        const auto radius_to_beacon = abs(beacon.first - sensor.first) + abs(beacon.second - sensor.second);
        const auto distance_to_target_row = abs(row - sensor.second);
        const auto overlap_radius = radius_to_beacon - distance_to_target_row;
        if (overlap_radius >= 0)
            overlaps += boost::icl::interval<int>::closed(sensor.first - overlap_radius, sensor.first + overlap_radius);
    }
    for (const auto [sensor, beacon] : scan) {
        if (beacon.second == row) {
            if (boost::icl::contains(overlaps, beacon.first))
                overlaps -= boost::icl::interval<int>::closed(beacon.first, beacon.first);
        }
    }

    return overlaps.size();
}

auto run_a(std::string_view s, int64_t row) {
    auto scan = parse(s);
    return impossible(scan, row);
}

auto run_b(std::string_view s, int area_size) {
    auto scan = parse(s);

    return -3;
}

int main() {
    for (auto [test_data, expected_a, expected_b] : test_data) {
        fmt::print("Test A: {}\n", run_a(test_data, 10) == expected_a ? "pass" : "fail");
        fmt::print("Test B: {}\n", run_b(test_data, 20) == expected_b ? "pass" : "fail");
    }

    const auto input_data = get_input(AOC_DAY);
    fmt::print("Part A: {}\n", run_a(input_data, 2000000));
    fmt::print("Part B: {}\n", run_b(input_data, 4000000));
}
