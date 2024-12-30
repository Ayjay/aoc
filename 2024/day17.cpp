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
const auto test_data = std::vector{std::tuple<std::string_view,
                                              std::optional<std::string_view>,
                                              std::optional<std::string_view>>{
    R"(Register A: 729
Register B: 0
Register C: 0

Program: 0,1,5,4,3,0
)",
    "4,6,3,5,6,3,5,2,1,0",
    {}}};

struct computer {
    i64 a = 0;
    i64 b = 0;
    i64 c = 0;
    i64 ip = 0;
    std::vector<i64> program;
    std::vector<i64> output;

    struct program_halt {};

    i64 literal_operand() { return program[ip++]; }

    i64 combo_operand() {
        auto v = program[ip++];
        assert(v >= 0 and v <= 6);
        switch (v) {
            case 4:
                return a;
            case 5:
                return b;
            case 6:
                return c;
            default:
                return v;
        }
    }

    void dv(i64& i) { i = a / std::pow(2, combo_operand()); }

    void adv() { dv(a); }

    void bxl() { b ^= literal_operand(); }

    void bst() { b = combo_operand() % 8; }

    void jnz() {
        if (a != 0) {
            ip = literal_operand();
        } else {
            ++ip;
        }
    }

    void bxc() {
        b = b ^ c;
        ++ip;
    }

    void out() { output.push_back(combo_operand() % 8); }

    void bdv() { dv(b); }

    void cdv() { dv(c); }

    auto run() {
        while (ip < program.size()) {
            switch (program[ip++]) {
                case 0:
                    adv();
                    break;
                case 1:
                    bxl();
                    break;
                case 2:
                    bst();
                    break;
                case 3:
                    jnz();
                    break;
                case 4:
                    bxc();
                    break;
                case 5:
                    out();
                    break;
                case 6:
                    bdv();
                    break;
                case 7:
                    cdv();
                    break;
            }
        }
        return fmt::format("{}", fmt::join(output, ","));
    }
};

TEST_CASE("test1", "[day17]") {
    auto comp = computer{.c = 9, .program = {2, 6}};
    comp.run();
    REQUIRE(comp.b == 1);
}

TEST_CASE("test2", "[day17]") {
    auto comp = computer{.a = 10, .program = {5, 0, 5, 1, 5, 4}};
    REQUIRE(comp.run() == "0,1,2");
}

TEST_CASE("test3", "[day17]") {
    auto comp = computer{.a = 2024, .program = {0, 1, 5, 4, 3, 0}};
    REQUIRE(comp.run() == "4,2,5,6,7,7,7,7,3,1,0");
    REQUIRE(comp.a == 0);
}

TEST_CASE("test4", "[day17]") {
    auto comp = computer{.b = 29, .program = {1, 7}};
    comp.run();
    REQUIRE(comp.b == 26);
}

TEST_CASE("test5", "[day17]") {
    auto comp = computer{.b = 2024, .c = 43690, .program = {4, 0}};
    comp.run();
    REQUIRE(comp.b == 44354);
}

TEST_CASE("operand_halt", "[day17]") {
    auto comp = computer{.program = {5}};
    REQUIRE(comp.run() == "");
}

computer parse(std::string_view s) {
    auto ret = computer{};
    auto set_register = [](i64& r) {
        return [&](auto& ctx) { r = _attr(ctx); };
    };
    auto set_program = [&](auto& ctx) { ret.program = _attr(ctx); };
    bp::parse(s,
              "Register A:" > bp::long_long[set_register(ret.a)] >
                  "Register B:" > bp::long_long[set_register(ret.b)] >
                  "Register C:" > bp::long_long[set_register(ret.c)] >
                  "Program:" > (bp::long_long % ',')[set_program],
              bp::ws);
    return ret;
}

auto run_a(std::string_view s) {
    return parse(s).run();
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

TEST_CASE("day17b", "[day17]") {
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