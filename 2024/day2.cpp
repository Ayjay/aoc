//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc2024.h"

#include <vector>
#include <tuple>
#include <string_view>
#include <utility>

#include <boost/hana/tuple.hpp>
#include <boost/unordered_map.hpp>

#include <catch2/catch_test_macros.hpp>

namespace day2 {
using result_type = long long;
const auto test_data = std::vector{ std::tuple<std::string_view, std::optional<result_type>, std::optional<result_type>>
{R"(7 6 4 2 1
1 2 7 8 9
9 7 6 2 1
1 3 2 4 5
8 6 4 4 1
1 3 6 7 9)", 2, {}}
};

auto parse(std::string_view s) {
    auto vals = *bp::parse(s, *(bp::long_long >> bp::long_long), bp::ws);
    using namespace hana::literals;
    return std::tuple{
        vals | rv::transform([](auto a) { return a[0_c]; }) | ranges::to<std::vector>,
        vals | rv::transform([](auto a) { return a[1_c]; }) | ranges::to<std::vector>
    };
}

static auto run_a(std::string_view s) {
    return -1;
}

static auto run_b(std::string_view s) {
    return -1;
}

TEST_CASE("day2a", "[day2]")
{
    const auto [s,expected,_] = test_data[0];
    if (expected) {
        REQUIRE(run_a(s) == *expected);
    }
}

TEST_CASE("day2b", "[day2]")
{
    const auto [s,_,expected] = test_data[0];
    if (expected) {
        REQUIRE(run_b(s) == *expected);
    }
}

}

WEAK void entry() {
    using namespace day2;
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}