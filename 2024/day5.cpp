#include "aoc2024.h"

#include <vector>
#include <tuple>
#include <string_view>
#include <utility>
#include <algorithm>

#include <fmt/core.h>

#include <boost/hana/tuple.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#include <boost/parser/search.hpp>

#include <boost/multi_array.hpp>

#include <catch2/catch_test_macros.hpp>

namespace day5 {
using result_type = long long;
const auto test_data = std::vector{ std::tuple<std::string_view, std::optional<result_type>, std::optional<result_type>>
{R"(47|53
97|13
97|61
97|47
75|29
61|13
75|53
29|13
97|29
53|29
61|53
97|53
61|29
47|13
75|47
97|75
47|61
75|61
47|29
75|13
53|13

75,47,61,53,29
97,61,53,29,13
75,29,13
75,97,47,61,53
61,13,29
97,13,75,29,47)", 143, 123}
};

auto parse(std::string_view s) {
    auto it = s.begin();
    auto rules = boost::unordered_set<std::tuple<result_type,result_type>>{};
    bp::prefix_parse(it, s.end(), *(bp::long_long > '|' > bp::long_long > bp::eol), rules);
    ++it;
    auto updates = *bp::prefix_parse(it, s.end(), *(bp::long_long % ',' > -bp::eol));
    return std::tuple{rules,updates};
}


auto run_a(std::string_view s) {
    const auto [rules,updates] = parse(s);

    const auto has_less_than_rule = [&](const auto i) {
        return [&rules,i](const auto j) {
            return rules.contains({i,j});
        };
    };

    const auto has_greater_than_rule = [&](const auto i) {
        return [&rules,i](const auto j) {
            return rules.contains({j,i});
        };
    };

    const auto correct_order = [&](const auto& update) {
        for (auto it = update.begin(); it != update.end(); ++it) {
            if (std::any_of(update.begin(), it, has_less_than_rule(*it)))
                return false;
            if (std::any_of(it+1, update.end(), has_greater_than_rule(*it)))
                return false;
        }
        return true;
    };

    const auto get_middle = [](const auto& update) {
        return update[update.size()/2];
    };
    
    auto correct_updates = updates | rv::filter(correct_order) | ranges::to<std::vector>;

    return reduce(correct_updates | rv::transform(get_middle));
}

static auto run_b(std::string_view s) {
    auto [rules,updates] = parse(s);
    auto less_thans = boost::unordered_map<result_type, boost::unordered_map<result_type, bool>>{};
    for (const auto [lt,gt] : rules) {
        less_thans[lt][gt] = true;
        less_thans[gt][lt] = false;
    }
    const auto sort_pred = [&](auto lt, auto gt) {
        return less_thans.at(lt).at(gt);
    };

    auto unsorted = updates | rv::filter([&](const auto& update) {
        return !ranges::is_sorted(update, sort_pred);
    });
    auto get_middle = [&](auto update) {
        auto it = update.begin() + update.size()/2;
        std::nth_element(update.begin(), it, update.end(), sort_pred);
        return *it;
    };
    return reduce(unsorted | rv::transform(get_middle));
}

TEST_CASE("day5a", "[day5]") {
    const auto [s,expected,_] = test_data[0];
    if (expected) {
        REQUIRE(run_a(s) == *expected);
    }
}

TEST_CASE("day5b", "[day5]")
{
    const auto [s,_,expected] = test_data[0];
    if (expected) {
        REQUIRE(run_b(s) == *expected);
    }
}

}

WEAK void entry() {
    using namespace day5;
    const auto input = get_input(AOC_DAY);
    fmt::println("A: {}", run_a(input));
    fmt::println("B: {}", run_b(input));
}