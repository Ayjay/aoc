// #define BOOST_SPIRIT_X3_DEBUG
#include "aoc2024.h"

#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <boost/hana/tuple.hpp>
#include <boost/unordered_map.hpp>

#include <catch2/catch_test_macros.hpp>

namespace day1 {
using result_type = long long;
const auto test_data =
    std::vector{std::tuple<std::string_view,
                           std::optional<result_type>,
                           std::optional<result_type>>{R"(3   4
4   3
2   5
1   3
3   9
3   3)",
                                                       11, 31}};

auto parse(std::string_view s) {
    auto vals = *bp::parse(s, *(bp::long_long >> bp::long_long), bp::ws);
    using namespace hana::literals;
    return std::tuple{vals | rv::transform([](auto a) { return a[0_c]; }) |
                          ranges::to<std::vector>,
                      vals | rv::transform([](auto a) { return a[1_c]; }) |
                          ranges::to<std::vector>};
}

static auto run_a(std::string_view s) {
    auto [left, right] = parse(s);
    ranges::sort(left);
    ranges::sort(right);
    return reduce(rv::zip_with([](auto a, auto b) { return std::abs(a - b); },
                               left, right));
}

static auto run_b(std::string_view s) {
    auto result = bp::parse(s, *(bp::long_long > bp::long_long), bp::ws);

    auto left = std::vector<result_type>{};
    auto right = boost::unordered_map<result_type, result_type>{};
    for (auto [l, r] : *result) {
        left.push_back(l);
        ++right[r];
    }

    const auto similarity_score = [&](auto i) {
        auto it = right.find(i);
        return i * (it == right.end() ? 0 : it->second);
    };

    return reduce(left | rv::transform(similarity_score));
}

TEST_CASE("day1a", "[day1]") {
    const auto [s, expected, _] = test_data[0];
    REQUIRE(run_a(s) == *expected);
}

TEST_CASE("day1b", "[day1]") {
    const auto [s, _, expected] = test_data[0];
    REQUIRE(run_b(s) == *expected);
}

}  // namespace day1

WEAK void entry() {
    using namespace day1;
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
