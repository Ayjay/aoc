#include "aoc2024.h"

#include <vector>
#include <tuple>
#include <string_view>
#include <utility>
#include <bitset>
#include <cmath>

#include <fmt/core.h>

#include <boost/hana/tuple.hpp>
#include <boost/unordered_set.hpp>

#include <catch2/catch_test_macros.hpp>

namespace day7 {
using result_type = long long;
const auto test_data = std::vector{ std::tuple<std::string_view, std::optional<result_type>, std::optional<result_type>>
{R"(190: 10 19
3267: 81 40 27
83: 17 5
156: 15 6
7290: 6 8 6 15
161011: 16 10 13
192: 17 8 14
21037: 9 7 18 13
292: 11 6 16 20)", 3749, {}}
};

using namespace hana::literals; 

bp::rule<struct equation, hana::tuple<result_type, std::vector<result_type>>> equation = "equation";
const auto set_first = [](auto& ctx) { _val(ctx)[0_c] = _attr(ctx); };
const auto set_second = [](auto& ctx) { _val(ctx)[1_c] = _attr(ctx); };
auto const equation_def = bp::long_long[set_first] >> ": " 
                       >> (bp::long_long % ' ')[set_second]
                       >> -bp::eol; /* even though conceptually this eol shouldn't be part of this rule
                                       boost::parser really does not like it if you put it outside the rule */
BOOST_PARSER_DEFINE_RULES(equation);
auto parse(std::string_view s) {
    return *bp::parse(s, *equation);
}

TEST_CASE("parsing", "[day7]") {
    const auto res = parse("1: 2 3");
    REQUIRE(res.size() == 1);
    const auto& [test_value,operands] = res.front();
    REQUIRE(test_value == 1);
    REQUIRE(operands.size() == 2);

    REQUIRE(parse("1: 2 3\n").size() == 1);
    REQUIRE(parse("1: 2 3\n2: 3 4").size() == 2);
}

auto eval(const auto& operands, const auto& operations) {
    const auto op = [](const auto acc, const auto x) {
        const auto [operand, is_plus] = x;
        if (is_plus)
            return acc + operand;
        else
            return acc * operand;
    };

    return ranges::accumulate(rv::zip(operands | rv::drop(1), operations), ranges::front(operands), op);
}

auto make_operation_combinations(auto size) {
    const auto combinations = std::pow(2,size);
    auto make_operation = [combinations,size](auto i) {
        return rv::iota(0, combinations) | rv::transform([ops=std::bitset<64>(i)](auto j) { return ops.test(j); });
    };
    return rv::iota(0, combinations) | rv::transform(make_operation);
}

const auto could_be_true = [](const auto& equation) {
    const auto& [test_value, operands] = equation;
    auto op_combinations = make_operation_combinations(operands.size()-1);
    auto results = op_combinations | rv::transform([&](const auto& ops) { return eval(operands, ops); });
    return ranges::contains(results, test_value);
};

TEST_CASE("last-case", "[day7]") {
    REQUIRE(eval(std::array{11,6,16,20}, std::array{true,false,true}) == 292);
    REQUIRE(could_be_true(std::tuple{292,std::array{11,6,16,20}}) == true);
}

auto run_a(std::string_view s) {
    const auto equations = parse(s);
    return reduce(equations | rv::filter(could_be_true) | rv::transform([](const auto& a) { return a[0_c]; }));
}

static auto run_b(std::string_view s) {
    return -1;
}

TEST_CASE("day7a", "[day7]") {
    const auto [s,expected,_] = test_data[0];
    if (expected) {
        REQUIRE(run_a(s) == *expected);
    }
}

TEST_CASE("day7b", "[day7]")
{
    const auto [s,_,expected] = test_data[0];
    if (expected) {
        REQUIRE(run_b(s) == *expected);
    }
}

}

WEAK void entry() {
    using namespace day7;
    const auto input = get_input(AOC_DAY);
    fmt::println("A: {}", run_a(input));
    fmt::println("B: {}", run_b(input));
}