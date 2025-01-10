#include "aoc2024.h"

#include <array>
#include <map>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>

#include <boost/dynamic_bitset.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#include <range/v3/action.hpp>

#include <catch2/catch_test_macros.hpp>

#include <type_safe/strong_typedef.hpp>

#include "simple_timer.hpp"

namespace day25 {

using nums = std::array<int, 5>;

i64 run_a(std::string_view s) {
    return -1;
}

i64 run_b(std::string_view s) {
    return -1;
}

nums parse(std::string_view s) {
    return {};
}

std::set<int> overlaps(nums lock, nums key) {
    return {};
}

TEST_CASE("parsing", "[day25]") {
    // lock
    const auto first = R"(#####
.####
.####
.####
.#.#.
.#...
.....)";
    CHECK(parse(first) == nums{0, 5, 3, 4, 3});

    const auto second = R"(.....
#....
#....
#...#
#.#.#
#.###
#####)";
    CHECK(parse(second) == nums{5, 0, 2, 1, 3});
}

TEST_CASE("overlaps", "[day25]") {
    CHECK(overlaps({0, 5, 3, 4, 3}, {5, 0, 2, 1, 3}) == std::set{4});
    CHECK(overlaps({0, 5, 3, 4, 3}, {4, 3, 4, 0, 2}) == std::set{1});
    CHECK(overlaps({0, 5, 3, 4, 3}, {3, 0, 2, 0, 1}).empty());
    CHECK(overlaps({1, 2, 0, 5, 3}, {5, 0, 2, 1, 3}) == std::set{0});
    CHECK(overlaps({1, 2, 0, 5, 3}, {4, 3, 4, 0, 2}).empty());
    CHECK(overlaps({1, 2, 0, 5, 3}, {3, 0, 2, 0, 1}).empty());
}

TEST_CASE("day25a", "[day25]") {
    const auto test_data = R"(#####
.####
.####
.####
.#.#.
.#...
.....

#####
##.##
.#.##
...##
...#.
...#.
.....

.....
#....
#....
#...#
#.#.#
#.###
#####

.....
.....
#.#..
###..
###.#
###.#
#####

.....
.....
.....
#....
#.#..
#.#.#
#####)";

    CHECK(run_a(test_data) == 3);
}

}  // namespace day25

WEAK void entry() {
    using namespace day25;
    const auto input = get_input(AOC_DAY);
    {
        auto t = SimpleTimer("Part A");
        fmt::println("A: {}", run_a(input));
    }
    {
        auto t = SimpleTimer("Part B");
        fmt::println("B: {}", run_b(input));
    }
}
