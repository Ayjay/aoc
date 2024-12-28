#include "aoc2024.h"

#include <bitset>
#include <cmath>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <fmt/core.h>

#include <boost/hana/tuple.hpp>
#include <boost/unordered_set.hpp>

#include <catch2/catch_test_macros.hpp>

#include "simple_timer.hpp"

namespace day7 {
using result_type = long long;
const auto test_data =
    std::vector{std::tuple<std::string_view,
                           std::optional<result_type>,
                           std::optional<result_type>>{R"(190: 10 19
3267: 81 40 27
83: 17 5
156: 15 6
7290: 6 8 6 15
161011: 16 10 13
192: 17 8 14
21037: 9 7 18 13
292: 11 6 16 20)",
                                                       3749, 11387}};

using namespace hana::literals;

bp::rule<struct equation, hana::tuple<result_type, std::vector<result_type>>>
    equation = "equation";
const auto set_first = [](auto& ctx) { _val(ctx)[0_c] = _attr(ctx); };
const auto set_second = [](auto& ctx) { _val(ctx)[1_c] = _attr(ctx); };
auto const equation_def =
    bp::long_long[set_first] >> ": " >> (bp::long_long % ' ')[set_second] >>
    -bp::eol; /* even though conceptually this eol shouldn't be part of this
                 rule boost::parser really does not like it if you put it
                 outside the rule */
BOOST_PARSER_DEFINE_RULES(equation);
auto parse(std::string_view s) {
    return *bp::parse(s, *equation);
}

TEST_CASE("parsing", "[day7]") {
    const auto res = parse("1: 2 3");
    REQUIRE(res.size() == 1);
    const auto& [test_value, operands] = res.front();
    REQUIRE(test_value == 1);
    REQUIRE(operands.size() == 2);

    REQUIRE(parse("1: 2 3\n").size() == 1);
    REQUIRE(parse("1: 2 3\n2: 3 4").size() == 2);
}

const auto num_digits(auto i) {
    auto digits = 1;
    while (i >= 10) {
        i /= 10;
        ++digits;
    }
    return digits;
}

TEST_CASE("num_digits", "[day7]") {
    REQUIRE(num_digits(0) == 1);
    REQUIRE(num_digits(1) == 1);
    REQUIRE(num_digits(10) == 2);
    REQUIRE(num_digits(11) == 2);
    REQUIRE(num_digits(99) == 2);
    REQUIRE(num_digits(585) == 3);
}

const auto equation_matches(auto it,
                            auto end,
                            auto acc,
                            auto target,
                            auto include_concat) {
    if (it == end)
        return acc == target;

    const auto val = *it++;

    if (equation_matches(it, end, acc + val, target, include_concat))
        return true;
    if (equation_matches(it, end, acc * val, target, include_concat))
        return true;
    if constexpr (include_concat) {
        return equation_matches(it, end,
                                acc * std::pow(10, num_digits(val)) + val,
                                target, include_concat);
    }
    return false;
}

const auto could_be_true = [](const auto include_concat) {
    return [include_concat](const auto& equation) {
        const auto& [test_value, operands] = equation;
        return equation_matches(operands.begin() + 1, operands.end(),
                                operands.front(), test_value, include_concat);
    };
};

auto run_a(std::string_view s) {
    auto t = SimpleTimer("Part A");
    const auto equations = parse(s);
    return reduce(equations | rv::filter(could_be_true(0_c)) |
                  rv::transform([](const auto& a) { return a[0_c]; }));
}

static auto run_b(std::string_view s) {
    auto t = SimpleTimer("Part B");
    const auto equations = parse(s);
    return reduce(equations | rv::filter(could_be_true(1_c)) |
                  rv::transform([](const auto& a) { return a[0_c]; }));
}

TEST_CASE("day7a", "[day7]") {
    const auto [s, expected, _] = test_data[0];
    if (expected) {
        REQUIRE(run_a(s) == *expected);
    }
}

TEST_CASE("day7b", "[day7]") {
    const auto [s, _, expected] = test_data[0];
    if (expected) {
        REQUIRE(run_b(s) == *expected);
    }
}

}  // namespace day7

WEAK void entry() {
    using namespace day7;
    const auto input = get_input(AOC_DAY);
    fmt::println("A: {}", run_a(input));
    fmt::println("B: {}", run_b(input));
}