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

#include <catch2/catch_test_macros.hpp>

#include "grid.hpp"
#include "simple_timer.hpp"

namespace day11 {
using result_type = long long;
const auto test_data = std::vector{ std::tuple<std::string_view, std::optional<result_type>, std::optional<result_type>>
{R"(125 17)", 55312, {}}
};

using namespace hana::literals; 
using namespace grid; 

auto parse(std::string_view s) {
    auto raw_stones = *bp::parse(s, bp::long_long % ' ');
    auto stones = std::unordered_map<result_type, result_type>{};
    for (auto s : raw_stones)
        ++stones[s];
    return stones;
}

auto step(auto stones) {
    using map_type = decltype(stones);
    auto new_stones = map_type{};
    for (auto [value,count] : stones) {
        if (value == 0) {
            new_stones[1] += count;
        } else if (const auto digits = static_cast<result_type>(std::log10(value)) + 1; digits % 2 == 0) {
            const auto d = static_cast<result_type>(std::pow(10ll, digits/2));
            new_stones[value / d] += count;
            new_stones[value % d] += count;
        } else {
            new_stones[value * 2024] += count;
        }
    }
    return new_stones;
}

auto steps(auto stones, int count) {
    for (auto _ : rv::iota(0,count))
        stones = step(std::move(stones));
}

auto run_a(std::string_view s) {
    auto stones = parse(s);

    for (auto _ : rv::iota(0,25))
        stones = step(std::move(stones));

    return reduce(stones | rv::values);
}

auto run_b(std::string_view s) {
    return -1;
}

TEST_CASE("day11a", "[day11]") {
    for (const auto& test : test_data) {
        const auto [s,expected,_] = test;
        if (expected) {
            REQUIRE(run_a(s) == *expected);
        }
    }
}

TEST_CASE("day11b", "[day11]") {
    for (const auto& test : test_data) {
        const auto [s,_,expected] = test;
        if (expected) {
            REQUIRE(run_b(s) == *expected);
        }
    }
}

}

WEAK void entry() {
    using namespace day11;
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