#include "aoc.h"
#include <fmt/std.h>
#include <iostream>
#include <cassert>

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

auto find_overlap = [](auto pack) {
    auto [left, right] = pack;
    auto in_right = [&](char c) {
        return std::ranges::find(right, c) != right.end();
    };
    return *std::ranges::find_if(left, in_right);
};

auto make_rucksacks(std::string_view s) {
    auto midpoint = s.begin() + s.size() / 2;
    return std::tuple{
        std::string_view{s.begin(), midpoint }
      , std::string_view{midpoint, s.end()}
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
        | sv::transform(make_rucksacks)
        | sv::transform(find_overlap)
        | sv::transform(get_score)
      , 0
    );
}

auto run_b(std::string_view s) {
    return 0;
}

int main() {
    assert(find_overlap(std::tuple{ "vJrwpWtwJgWr"sv, "hcsFMMfFFhFp"sv }) == 'p');
    run(run_a, run_b, 157, -1, test_data, get_input(3));
}