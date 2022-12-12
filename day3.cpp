#include "aoc.h"
#include <fmt/std.h>
#include <set>
#include <vector>
#include <array>

auto test_data =
R"(vJrwpWtwJgWrhcsFMMfFFhFp
jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL
PmmdzqPrVvPwwTWBwg
wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn
ttgJtRGJQctTZtZT
CrZsJsPPZsGzwwsLwLmpwMDw)";

auto intersect = [](const std::set<char>& x, const std::set<char>& y) {
    std::set<char> result;
    std::ranges::set_intersection(x, y, std::inserter(result, result.end()));
    return result;
};

auto find_overlap = []<class R>(R&& sets)
    requires std::is_same_v<std::ranges::range_value_t<R>, std::set<char>> {
    auto result = reduce(sets, intersect);
    assert(result.size() == 1);
    return *result.begin();
};

auto make_rucksack(std::string_view s) {
    auto midpoint = s.begin() + s.size() / 2;
    return std::array{
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
    auto lines = get_lines(s);
    auto sets = lines | sv::transform(make_rucksack);
    auto overlaps = sets | sv::transform(find_overlap);
    auto scores = overlaps | sv::transform(get_score);
    return ranges::accumulate(scores , 0);
}

auto run_b(std::string_view s) {
    auto lines = get_lines(s);
    auto sets = lines | sv::transform([](auto&& a) { return a | ranges::to<std::set>(); });
    auto chunks = sets | rv::chunk(3);
    auto overlaps = chunks | sv::transform(find_overlap);
    auto scores = overlaps | sv::transform(get_score);
    return ranges::accumulate(scores , 0);
}

int main() {
    run(run_a, run_b, 157, 70, test_data, get_input(3));
}
