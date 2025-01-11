#include "aoc2024.h"

#include <array>
#include <map>
#include <set>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>

#include <boost/container/static_vector.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#include <range/v3/action.hpp>

#include <catch2/catch_test_macros.hpp>

#include "grid.hpp"
#include "simple_timer.hpp"

namespace day25 {

using nums = std::array<int, 5>;
using namespace grid;

auto parse(std::string_view s) {
    auto g = grid_t{s};
    auto ret = nums{};
    for (auto c : rv::iota(0, g.cols)) {
        ret[c] = {};
        for (auto r : rv::iota(1, g.rows - 1)) {
            if (g.map[r][c] == '#')
                ++ret[c];
        }
    }
    return std::tuple{ret, g.map[0][0] == '#'};
}

auto overlaps(nums lock, nums key) {
    const auto get_index = [](auto x) { return std::get<0>(x); };
    const auto greater = [](auto v) {
        const auto [i, x] = v;
        return x > 5;
    };
    return rv::enumerate(rv::zip_with(std::plus{}, lock, key)) |
           rv::filter(greater) | rv::transform(get_index) |
           ranges::to<boost::container::static_vector<size_t, 5>>;
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
    CHECK(parse(first) == std::tuple{nums{0, 5, 3, 4, 3}, true});

    const auto second = R"(.....
#....
#....
#...#
#.#.#
#.###
#####)";
    CHECK(parse(second) == std::tuple{nums{5, 0, 2, 1, 3}, false});
}

TEST_CASE("overlaps", "[day25]") {
    CHECK(*overlaps({0, 5, 3, 4, 3}, {5, 0, 2, 1, 3}).begin() == 4);
    CHECK(*overlaps({0, 5, 3, 4, 3}, {4, 3, 4, 0, 2}).begin() == 1);
    CHECK(overlaps({0, 5, 3, 4, 3}, {3, 0, 2, 0, 1}).empty());
    CHECK(*overlaps({1, 2, 0, 5, 3}, {5, 0, 2, 1, 3}).begin() == 0);
    CHECK(overlaps({1, 2, 0, 5, 3}, {4, 3, 4, 0, 2}).empty());
    CHECK(overlaps({1, 2, 0, 5, 3}, {3, 0, 2, 0, 1}).empty());
}

i64 run_a(std::string_view s) {
    auto locks = std::vector<nums>{};
    auto keys = std::vector<nums>{};
    using namespace std::string_view_literals;
    for (auto d : s | sv::split("\n\n"sv)) {
        const auto [ns, is_lock] = parse(std::string_view(d.begin(), d.end()));
        (is_lock ? locks : keys).push_back(ns);
    }
    const auto any_overlaps = [](auto r) {
        const auto [lock, key] = r;
        return overlaps(lock, key).empty();
    };
    return ranges::distance(rv::cartesian_product(locks, keys) |
                            rv::filter(any_overlaps));
}

i64 run_b(std::string_view s) {
    return -1;
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
