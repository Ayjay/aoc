#include "aoc.h"
#include <fmt/std.h>
#include <iostream>
#include <cassert>
#include <set>
#include <vector>
#include <boost/hana/core.hpp>
#include <boost/hana/fold.hpp>
namespace hana = boost::hana;
#include <boost/fusion/adapted/std_tuple.hpp>

auto test_data =
R"(vJrwpWtwJgWrhcsFMMfFFhFp
jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL
PmmdzqPrVvPwwTWBwg
wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn
ttgJtRGJQctTZtZT
CrZsJsPPZsGzwwsLwLmpwMDw)";

auto get_lines(std::string_view& s) {
    if (s.back() == '\n')
        s = { s.begin(), s.end() - 1 };
    return s | sv::split("\n"sv) | sv::transform([](auto a) { return std::string_view{ begin(a), end(a) }; });
}

auto intersect = [](auto&& x, auto&& y) {
    decltype(x) result;
    std::ranges::set_intersection(x, y, std::inserter(result, result.end()));
    return result;
};

auto find_overlap = [](auto sets) {
    auto result = hana::fold(sets, intersect);
    assert(result.size() == 1);
    return *result.begin();
};

auto make_rucksack(std::string_view s) {
    auto midpoint = s.begin() + s.size() / 2;
    return std::tuple{
        std::set<char>{s.begin(), midpoint }
      , std::set<char>{midpoint, s.end()}
    };
}

int get_score(char c) {
    if (c >= 'a' && c <= 'z')
        return c - 'a' + 1;
    else
        return c - 'A' + 27;
}

auto run_a(std::string_view s) {
    return ranges::accumulate(
          get_lines(s)
        | sv::transform(make_rucksack)
        | sv::transform(find_overlap)
        | sv::transform(get_score)
      , 0
    );
}

auto run_b(std::string_view s) {
    return ranges::accumulate(
          get_lines(s)
        | ranges::to<std::set<char>>()
        | rv::chunk(3)
        | sv::transform(find_overlap)
        | sv::transform(get_score)
      , 0
    );
    return -1;
}

int main() {
    run(run_a, run_b, 157, 70, test_data, get_input(3));
}