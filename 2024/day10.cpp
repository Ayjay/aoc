#include "aoc2024.h"

#include <vector>
#include <tuple>
#include <string_view>
#include <utility>
#include <bitset>
#include <cmath>
#include <list>

#include <fmt/core.h>

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#include <catch2/catch_test_macros.hpp>

#include "grid.hpp"
#include "simple_timer.hpp"

namespace day10 {
using result_type = long long;
const auto test_data = std::vector{ std::tuple<std::string_view, std::optional<result_type>, std::optional<result_type>>
{R"(0123
1234
8765
9876)", 1, {}},
{R"(89010123
78121874
87430965
96549874
45678903
32019012
01329801
10456732)", 36, {}}
};

using namespace hana::literals; 
using namespace grid; 

auto run_a(std::string_view s) {
    auto g = grid_t(s);
    auto trails = boost::unordered_map<vector2, boost::unordered_set<vector2>>{};

    auto score = [&](this auto& self, vector2 p) -> boost::unordered_set<vector2>& {
        if (auto it = trails.find(p); it != trails.end()) {
            return it->second;
        }
        auto result = boost::unordered_set<vector2>{};
        if (g.get(p) == '9') {
            result.insert(p);
        } else {
            const auto get_adjacent = [&](vector2 dir) { return p + dir; };
            const auto is_step_up = [&](vector2 q) {
                auto c = g.checked_get(q);
                return c.has_value() and *c == g.get(p) + static_cast<char>(1);
            };
            const auto combine_sets = [](auto acc, const auto& s) {
                acc.insert(s.begin(), s.end());
                return acc;
            };
            auto step_ups = directions 
                | rv::transform(get_adjacent)
                | rv::filter(is_step_up);
            result = ranges::accumulate(step_ups, std::move(result), combine_sets, self);
        }
        auto [it,_] = trails.insert({p, std::move(result)});
        return it->second;
    };

    auto zeros = g.cells() | rv::filter([&](auto p) { return g.get(p) == '0'; });

    return reduce(zeros | rv::transform(score) | rv::transform([](const auto& s) { return s.size(); }));
}

auto run_b(std::string_view s) {
    return -1;
}

TEST_CASE("day10a", "[day10]") {
    for (const auto& test : test_data) {
        const auto [s,expected,_] = test;
        if (expected) {
            REQUIRE(run_a(s) == *expected);
        }
    }
}

TEST_CASE("day10b", "[day10]") {
    for (const auto& test : test_data) {
        const auto [s,_,expected] = test;
        if (expected) {
            REQUIRE(run_b(s) == *expected);
        }
    }
}

}

WEAK void entry() {
    using namespace day10;
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