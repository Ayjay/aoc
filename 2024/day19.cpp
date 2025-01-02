#include "aoc2024.h"

#include <iterator>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>
#include <memory>
#include <execution>

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>

#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>

#include <catch2/catch_test_macros.hpp>

#include "simple_timer.hpp"

namespace day19 {
using i64 = long long;
const auto test_data = std::vector{std::tuple<std::string_view,
                                              std::optional<i64>,
                                              std::optional<i64>>{
    R"(r, wr, b, g, bwu, rb, gb, br

brwrr
bggr
gbbr
rrbgbr
ubwu
bwurrg
brgr
bbrgwb)",
    6, 16}};

const auto colour_to_index = [](char c) {
    switch (c) {
        case 'w': return 0;
        case 'u': return 1;
        case 'b': return 2;
        case 'r': return 3;
        case 'g': return 4;
    }
    std::unreachable();
};

struct node {
    std::array<std::unique_ptr<node>, 5> els;
    bool terminal = false;
};

void add_towel(node& n, auto begin, auto end) {
    if (begin == end) {
        n.terminal = true;
        return;
    }

    auto& c = n.els[colour_to_index(*begin++)];
    if (not c) c = std::make_unique<node>();
    add_towel(*c, begin, end);
}

auto parse(std::string_view s) {
    const auto [towels, designs] = *bp::parse(s, (+bp::lower) % ", " > bp::eol > bp::eol > (+bp::lower) % bp::eol);

    auto chains = std::make_unique<node>();

    for (const auto towel : towels) {
        add_towel(*chains, towel.begin(), towel.end());
    }

    return std::tuple{std::move(chains), designs};
}

constexpr bool debug_print = false;
i64 run_a(std::string_view s) {
    const auto [towels, designs] = parse(s);
    const auto possible = [&](std::string_view design) -> bool {
        if constexpr (debug_print) fmt::print("Checking '{}'...", design);
        auto seen = boost::unordered_set<std::string_view::iterator>{};
        auto s = std::vector<std::string_view::iterator>{};
        s.push_back(design.begin());

        while (not s.empty()) {
            auto it = s.back();
            s.pop_back();
            if (not seen.insert(it).second)
                continue;
            auto n = towels.get();

            for ( ; it != design.end(); ++it) {
                if (n->terminal)
                    s.push_back(it);

                auto& c = n->els[colour_to_index(*it)];
                if (not c)
                    break;

                n = c.get();
            }
            if (it == design.end() and n->terminal) {
                if constexpr (debug_print) fmt::println("yes");
                return true;
            }
        }
        if constexpr (debug_print) fmt::println("no");
        return false;
    };
    return std::count_if(std::execution::par_unseq, designs.begin(), designs.end(), possible);
}

auto run_b(std::string_view s) {
    const auto [towels, designs] = parse(s);
    const auto possible = [&](std::string_view design) -> i64 {
        if constexpr (debug_print) fmt::print("Checking '{}'...", design);
        auto seen = boost::unordered_map<std::string_view::iterator, i64>{};

        const auto check = [&](this auto&& self, std::string_view::iterator it) {
            auto cached = seen.find(it);
            if (cached != seen.end())
                return cached->second;

            const auto start = it;
            auto n = towels.get();
            auto count = i64{};

            for ( ; it != design.end(); ++it) {
                if (n->terminal)
                    count += self(it);

                auto& c = n->els[colour_to_index(*it)];
                if (not c)
                    break;

                n = c.get();
            }
            if (it == design.end() and n->terminal) {
                ++count;
            }
            seen[start] = count;
            return count;
        };

        return check(design.begin());
    };
    return reduce(designs | rv::transform(possible));
}

TEST_CASE("day19a", "[day19]") {
    for (const auto& test : test_data) {
        const auto [s, expected, _] = test;
        if (expected) {
            REQUIRE(run_a(s) == *expected);
        }
    }
}

TEST_CASE("day19b", "[day19]") {
    for (const auto& test : test_data) {
        const auto [s, _, expected] = test;
        if (expected) {
            REQUIRE(run_b(s) == *expected);
        }
    }
}

}  // namespace day19

WEAK void entry() {
    using namespace day19;
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
