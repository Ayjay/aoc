#include "aoc2024.h"

#include <map>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>

#include <boost/unordered_map.hpp>

#include <catch2/catch_test_macros.hpp>

#include "simple_timer.hpp"

namespace day22 {
using i64 = long long;

i64 mix(i64 secret, i64 given) {
    return secret ^ given;
}

TEST_CASE("mix", "[day22]") {
    REQUIRE(mix(42, 15) == 37);
}

i64 prune(i64 secret) {
    return secret % 16777216;
}

TEST_CASE("prune", "[day22]") {
    REQUIRE(prune(100000000) == 16113920);
}

i64 next_secret(i64 i) {
    i = prune(mix(i, i * 64));
    i = prune(mix(i, i / 32));
    i = prune(mix(i, i * 2048));
    return i;
}

TEST_CASE("secrets", "[day22]") {
    auto i = i64{123};
    REQUIRE((i = next_secret(i)) == 15887950);
    REQUIRE((i = next_secret(i)) == 16495136);
    REQUIRE((i = next_secret(i)) == 527345);
    REQUIRE((i = next_secret(i)) == 704524);
    REQUIRE((i = next_secret(i)) == 1553684);
    REQUIRE((i = next_secret(i)) == 12683156);
    REQUIRE((i = next_secret(i)) == 11100544);
    REQUIRE((i = next_secret(i)) == 12249484);
    REQUIRE((i = next_secret(i)) == 7753432);
    REQUIRE((i = next_secret(i)) == 5908254);
}

const auto run_day(i64 i) {
    for (auto _ : rv::iota(0, 2000))
        i = next_secret(i);
    return i;
};

TEST_CASE("run_day", "[day22]") {
    REQUIRE(run_day(1) == 8685429);
    REQUIRE(run_day(10) == 4700978);
    REQUIRE(run_day(100) == 15273692);
    REQUIRE(run_day(2024) == 8667524);
}

i64 run_a(std::string_view s) {
    auto initials = *bp::parse(s, bp::long_long % bp::eol);
    return reduce(initials | rv::transform(run_day));
}

i64 run_b(std::string_view s) {
    return -1;
}

TEST_CASE("day22a", "[day22]") {
    const auto test_data = R"(1
10
100
2024)";
    REQUIRE(run_a(test_data) == 37327623);
}

TEST_CASE("day22b", "[day22]") {}

}  // namespace day22

WEAK void entry() {
    using namespace day22;
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
