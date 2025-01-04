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
    CHECK(run_day(1) == 8685429);
    CHECK(run_day(10) == 4700978);
    CHECK(run_day(100) == 15273692);
    CHECK(run_day(2024) == 8667524);
}

i64 run_a(std::string_view s) {
    auto initials = *bp::parse(s, bp::long_long % bp::eol);
    return reduce(initials | rv::transform(run_day));
}

using sequence = std::array<i64, 4>;
using price_map = std::map<sequence, i64>;

constexpr auto count = 2000;

auto get_prices(i64 i) {
    auto secrets = std::vector{i};
    secrets.reserve(count + 1);
    std::generate_n(std::back_inserter(secrets), count,
                    [&] { return i = next_secret(i); });
    const auto get_last_digit = [](i64 x) {
        while (x > 10)
            x %= 10;
        return x;
    };
    return secrets | rv::transform(get_last_digit) | ranges::to<std::vector>;
}

auto get_sequence_prices(i64 i) {
    const auto prices = get_prices(i);
    // fmt::println("{}", prices);
    auto diffs = std::vector<i64>{};
    diffs.reserve(count);
    ranges::adjacent_difference(prices, std::back_inserter(diffs));
    auto first_prices = price_map{};
    for (auto [window, price] :
         rv::zip(diffs | rv::drop(1) | rv::sliding(4),
                 ranges::subrange{prices.begin() + 4, prices.end()})) {
        auto seq = sequence{};
        ranges::copy(window, seq.begin());
        if (seq == std::array{-2ll, 1ll, -1ll, 3ll}) {
            int xx = 0;
        }
        first_prices.insert({seq, price});
    }
    return first_prices;
}

const auto combine_prices = [](const price_map& l, const price_map& r) {
    auto res = price_map{};
    auto first_it = l.begin();
    auto second_it = r.begin();
    while (first_it != l.end() and second_it != r.end()) {
        if (first_it->first == second_it->first) {
            res.insert({first_it->first, first_it->second + second_it->second});
            ++first_it;
            ++second_it;
        } else if (first_it->first < second_it->first) {
            res.insert(*first_it++);
        } else {
            res.insert(*second_it++);
        }
    }
    std::copy(first_it, l.end(), std::inserter(res, res.end()));
    std::copy(second_it, r.end(), std::inserter(res, res.end()));
    return res;
};

TEST_CASE("small sequence", "[day22]") {
    const auto seq = sequence{-2, 1, -1, 3};
    CHECK(get_sequence_prices(1).at(seq) == 7);
    CHECK(get_sequence_prices(2).at(seq) == 7);
    CHECK_FALSE(get_sequence_prices(3).contains(seq));
    CHECK(get_sequence_prices(2024).at(seq) == 9);
}

i64 run_b(std::string_view s) {
    auto initials = *bp::parse(s, bp::long_long % bp::eol);
    auto best_prices = ranges::accumulate(initials, price_map{}, combine_prices,
                                          get_sequence_prices);
    auto best_price = ranges::max_element(best_prices, std::less{},
                                          [](auto kv) { return kv.second; });
    return best_price->second;
}

TEST_CASE("day22a", "[day22]") {
    const auto test_data = R"(1
10
100
2024)";

    REQUIRE(run_a(test_data) == 37327623);
}

TEST_CASE("day22b", "[day22]") {
    const auto test_data = R"(1
2
3
2024)";
    REQUIRE(run_b(test_data) == 23);
}
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
