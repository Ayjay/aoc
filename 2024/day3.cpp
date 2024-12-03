#include "aoc2024.h"

#include <vector>
#include <tuple>
#include <string_view>
#include <utility>

#include <fmt/core.h>

#include <boost/hana/tuple.hpp>
#include <boost/unordered_map.hpp>

#include <boost/parser/search.hpp>

#include <catch2/catch_test_macros.hpp>

namespace day3 {
using result_type = long long;
const auto test_data = std::vector{ std::tuple<std::string_view, std::optional<result_type>, std::optional<result_type>>
{R"(xmul(2,4)%&mul[3,7]!@^do_not_mul(5,5)+mul(32,64]then(mul(11,8)mul(8,5)))", 161, 48}
};

static auto run_a(std::string_view s) {
    auto it = s.begin();
    auto count = 0;
    const auto parser = "mul(" >> bp::long_long >> ',' >> bp::long_long >> ')';
    while (auto res = bp::prefix_parse(it, s.end(), bp::omit[*(bp::char_ - parser)] >> parser)) {
        using namespace hana::literals;
        count += (*res)[0_c] * (*res)[1_c];
    }
    return count;
}

static auto run_b(std::string_view s) {
    auto it = s.begin();
    auto count = 0;
    do {
        auto [do_end,next] = bp::search(it, s.end(), bp::lit("don't()"));
        count += run_a(std::string_view{it, do_end});
        it = bp::search(next, s.end(), bp::lit("do()")).end();
    } while (it != s.end());
    return count;
}

TEST_CASE("shorter string", "[day3]") {
    REQUIRE(run_b("xmul(1,2)don't()mul(3,4)"));
}

TEST_CASE("day3a", "[day3]") {
    const auto [s,expected,_] = test_data[0];
    if (expected) {
        REQUIRE(run_a(s) == *expected);
    }
}

TEST_CASE("day3b", "[day3]")
{
    REQUIRE(run_b("xmul(2,4)&mul[3,7]!^don't()_mul(5,5)+mul(32,64](mul(11,8)undo()?mul(8,5))") == 48);
}

}

WEAK void entry() {
    using namespace day3;
    const auto input = get_input(AOC_DAY);
    fmt::println("A: {}", run_a(input));
    fmt::println("B: {}", run_b(input));
}