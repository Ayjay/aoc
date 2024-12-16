#include "aoc2024.h"

#include <vector>
#include <tuple>
#include <string_view>
#include <utility>
#include <cmath>
#include <list>
#include <unordered_map>

#include <fmt/core.h>

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#include <catch2/catch_test_macros.hpp>

#include "grid.hpp"
#include "simple_timer.hpp"

namespace day12 {
using result_type = long long;
const auto test_data = std::vector{ std::tuple<std::string_view, std::optional<result_type>, std::optional<result_type>>
{R"(AAAA
BBCD
BBCC
EEEC)", 140, 80},
{R"(OOOOO
OXOXO
OOOOO
OXOXO
OOOOO)", 772, 436},
{R"(EEEEE
EXXXX
EEEEE
EXXXX
EEEEE)", {}, 236},
{R"(AAAAAA
AAABBA
AAABBA
ABBAAA
ABBAAA
AAAAAA)", {}, 368},
{R"(RRRRIICCFF
RRRRIICCCF
VVRRRCCFFF
VVRCCCJFFF
VVVVCJJCFE
VVIVCCJJEE
VVIIICJJEE
MIIIIIJJEE
MIIISIJEEE
MMMISSJEEE)", 1930, 1206}
};

using namespace hana::literals; 
using namespace grid; 


auto run_a(std::string_view s) {
    auto g = grid_t{s};
    auto regions = boost::unordered_map<vector2, char>{};
    auto price = result_type{};
    const auto explore_region = [&](vector2 p) {
        if (regions.contains(p))
            return;

        auto s = std::vector{p};
        auto region = g.get(p);
        auto perimeter = result_type{};
        auto area = result_type{};
        while (!s.empty()) {
            const auto q = s.back();
            s.pop_back();
            if (regions.contains(q))
                continue;
            ++area;
            regions[q] = region;
            auto cell_perimeter = result_type{4};
            for (auto dir : directions) {
                const auto neighbour_pos = q + dir;
                if (const auto neighbour = g.checked_get(neighbour_pos); neighbour and *neighbour == region) {
                    --cell_perimeter;
                    s.push_back(neighbour_pos);
                }
            }
            perimeter += cell_perimeter;
        }
        price += area * perimeter;
    };
    ranges::for_each(g.cells(), explore_region);
    return price;
}

auto run_b(std::string_view s) {
    auto g = grid_t{s};
    auto regions = boost::unordered_map<vector2, char>{};
    auto price = result_type{};
    const auto explore_region = [&](vector2 p) {
        if (regions.contains(p))
            return;

        auto s = std::vector{p};
        auto region = g.get(p);
        auto edges = boost::unordered_set<std::tuple<vector2,vector2>>{};
        auto area = result_type{};
        while (!s.empty()) {
            const auto q = s.back();
            s.pop_back();
            if (regions.contains(q))
                continue;
            ++area;
            regions[q] = region;
            const auto in_region = [&](const vector2 u) {
                return g.checked_get(u) == region;
            };
            for (const auto dir : directions) {
                if (in_region(q+dir)) {
                    s.push_back({q+dir});
                } else {
                    edges.insert({q,dir});
                }
            }
        }
        auto perimeter = result_type{};
        while (!edges.empty()) {
            auto [q,q_norm] = *edges.begin();
            ++perimeter;
            edges.erase(edges.begin());
            auto it = edges.begin();
            for (auto q_left = q + turn_left(q_norm); (it = edges.find({q_left, q_norm})) != edges.end(); q_left = q_left + turn_left(q_norm))
                edges.erase(it);
            for (auto q_right = q + turn_right(q_norm); (it = edges.find({q_right, q_norm})) != edges.end(); q_right = q_right + turn_right(q_norm))
                edges.erase(it);
        }
        price += area * perimeter;
    };
    ranges::for_each(g.cells(), explore_region);
    return price;
}

TEST_CASE("day12a", "[day12]") {
    for (const auto& test : test_data) {
        const auto [s,expected,_] = test;
        if (expected) {
            REQUIRE(run_a(s) == *expected);
        }
    }
}

TEST_CASE("day12b", "[day12]") {
    for (const auto& test : test_data) {
        const auto [s,_,expected] = test;
        if (expected) {
            REQUIRE(run_b(s) == *expected);
        }
    }
}

}

WEAK void entry() {
    using namespace day12;
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