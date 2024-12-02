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
    auto res = bp::parse(s, *(+bp::long_long > -bp::eol), bp::blank);
    return *res;
}

TEST_CASE("terminated string", "[day2]") {
    const auto str = std::string_view{"1 2 3 4 5\n6 7 8\n"};
    auto res = bp::parse(str, +(+bp::long_long > -bp::eol), bp::blank, boost::parser::trace::on);
    REQUIRE(res);
}

TEST_CASE("unterminated string", "[day2]") {
    const auto str = std::string_view{"1 2 3 4 5\n6 7 8"};
    auto res = bp::parse(str, *(+bp::long_long > -bp::eol), bp::blank, boost::parser::trace::on);
    REQUIRE(res);
}

TEST_CASE("day2 parse", "[day2]") {
    const auto [s,_,_] = test_data[0];
    const auto parsed = parse(s);
    REQUIRE(parsed.size() == 6);
    REQUIRE(parsed.front().size() == 5);
}

static auto run_a(std::string_view s) {
    const auto reports = parse(s);
    const auto safe = [](const std::vector<result_type>& report) -> bool {
        if (report.size() <= 1) return true;
        const auto safe_pair = [increasing = report[1] > report[0]](auto window) {
            const auto gap = increasing ? window[1] - window[0] : window[0] - window[1];
            return gap >= 1 && gap <= 3;
        };
        return ranges::all_of(report | rv::sliding(2), safe_pair);
    };
    return ranges::distance(reports | rv::filter(safe));
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