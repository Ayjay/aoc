#include "aoc2024.h"

#include <cmath>
#include <iterator>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#include <catch2/catch_test_macros.hpp>

#include "simple_timer.hpp"

namespace day17 {
using i64 = long long;
const auto test_data = std::vector{
    std::tuple<std::string_view, std::optional<std::string_view>, std::optional<std::string_view>>{
        R"(Register A: 729
Register B: 0
Register C: 0

Program: 0,1,5,4,3,0)",
        "4,6,3,5,6,3,5,2,1,0", {}}
};

struct computer {
    i64 a = 0;
    i64 b = 0;
    i64 c = 0;
    i64 ip = 0;
    std::vector<i64> program;
    std::vector<i64> output;

    i64& operand() {
        auto v = program[ip++];
        switch (v) {
            case 4: return a;
            case 5: return b;
            case 6: return c;
            default: return v;
        }
    }

    void dv(i64& i) {
        ++ip;
        i = i / std::pow(2, operand());
    }

    void adv() {
        dv(a);
    }

    void bxl() {
        ++ip;
        b ^= operand();
    }

    void bst() {
        ++ip;
        b %= operand();
    }

    void jnz() {
        if (a != 0) {
            ip = operand();
        } else {
            ip += 2;
        }
    }

    void bxc() {
        b = b ^ c;
        ip += 2;
    }

    void out() {
        ++ip;
        output.push_back(operand() % 8);
    }

    void bdv() {
        dv(b);
    }

    void cdv() {
        dv(c);
    }

    void run() {
        while (ip < program.size()) {
            switch (program[ip]) {
            case 0: adv(); break;
            case 1: bxl(); break;
            case 2: bst(); break;
            case 3: jnz(); break;
            case 4: bxc(); break;
            case 5: out(); break;
            case 6: bdv(); break;
            case 7: cdv(); break;
            }
        }
        return fmt::join(output, ",");
    }
};

auto parse(std::string_view s) {
    auto stones = std::unordered_map<result_type, result_type>{};
    for (auto s : raw_stones)
        ++stones[s];
    return stones;
}

auto run_a(std::string_view s) {
    return "";
}

auto run_b(std::string_view s) {
    return "";
}

TEST_CASE("day17a", "[day19]") {
    for (const auto& test : test_data) {
        const auto [s, expected, _] = test;
        if (expected) {
            REQUIRE(run_a(s) == *expected);
        }
    }
}

TEST_CASE("day18b", "[day18]") {
    for (const auto& test : test_data) {
        const auto [s, _, expected] = test;
        if (expected) {
            REQUIRE(run_b(s) == *expected);
        }
    }
}

}  // namespace day17

WEAK void entry() {
    using namespace day17;
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