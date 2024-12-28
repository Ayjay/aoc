#include "aoc2024.h"

#include <cmath>
#include <list>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include <fmt/core.h>

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#include <catch2/catch_test_macros.hpp>

#include "grid.hpp"
#include "simple_timer.hpp"

namespace day13 {
using result_type = long long;
const auto test_data =
    std::vector{std::tuple<std::string_view,
                           std::optional<result_type>,
                           std::optional<result_type>>{R"(Button A: X+94, Y+34
Button B: X+22, Y+67
Prize: X=8400, Y=5400

Button A: X+26, Y+66
Button B: X+67, Y+21
Prize: X=12748, Y=12176

Button A: X+17, Y+86
Button B: X+84, Y+37
Prize: X=7870, Y=6450

Button A: X+69, Y+23
Button B: X+27, Y+71
Prize: X=18641, Y=10279)",
                                                       480, 875318608908}};

using namespace hana::literals;
using namespace grid;

struct machine {
    result_type a_x;
    result_type a_y;
    result_type b_x;
    result_type b_y;
    result_type prize_x;
    result_type prize_y;
};

bp::rule<struct equation, machine> equation = "equation";
auto const equation_def =
    "Button A: X+" > bp::long_long > ", Y+" > bp::long_long > bp::eol >
    "Button B: X+" > bp::long_long > ", Y+" > bp::long_long > bp::eol >
    "Prize: X=" > bp::long_long > ", Y=" > bp::long_long > -bp::eol;
BOOST_PARSER_DEFINE_RULES(equation);
auto parse(std::string_view s) {
    return *bp::parse(s, equation % bp::eol);
}

template <class T>
std::optional<T> integer_div(T a, T b) {
    if (a % b != 0)
        return {};
    else
        return a / b;
}

std::optional<result_type> solve(machine m) {
    const auto b_divisor = m.a_x * m.b_y - m.a_y * m.b_x;
    const auto b_quotient = m.prize_y * m.a_x - m.prize_x * m.a_y;
    const auto b = integer_div(b_quotient, b_divisor);
    if (!b)
        return {};
    const auto a = integer_div(m.prize_x - m.b_x * *b, m.a_x);
    if (!a)
        return {};
    return 3 * *a + *b;
}

auto run_a(std::string_view s) {
    auto eqs = parse(s);
    auto costs = eqs | rv::transform(solve);
    return ranges::accumulate(costs, result_type{}, std::plus{},
                              [](const auto x) { return x ? *x : 0ll; });
}

auto run_b(std::string_view s) {
    auto eqs = parse(s);
    const auto fix_conversion = [](machine m) {
        m.prize_x += 10000000000000;
        m.prize_y += 10000000000000;
        return m;
    };
    auto costs = eqs | rv::transform(fix_conversion) | rv::transform(solve);
    return ranges::accumulate(costs, result_type{}, std::plus{},
                              [](const auto x) { return x ? *x : 0ll; });
}

TEST_CASE("day13a", "[day13]") {
    for (const auto& test : test_data) {
        const auto [s, expected, _] = test;
        if (expected) {
            REQUIRE(run_a(s) == *expected);
        }
    }
}

TEST_CASE("day13b", "[day13]") {
    for (const auto& test : test_data) {
        const auto [s, _, expected] = test;
        if (expected) {
            REQUIRE(run_b(s) == *expected);
        }
    }
}

}  // namespace day13

WEAK void entry() {
    using namespace day13;
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