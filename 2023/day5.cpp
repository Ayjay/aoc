#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <vector>
#include <tuple>
#include <string_view>
#include <unordered_set>
#include <chrono>
#include <iostream>

#include <boost/unordered_map.hpp>
#include <boost/icl/interval_map.hpp>

const auto test_data = std::vector{ std::tuple
{R"(seeds: 79 14 55 13

seed-to-soil map:
50 98 2
52 50 48

soil-to-fertilizer map:
0 15 37
37 52 2
39 0 15

fertilizer-to-water map:
49 53 8
0 11 42
42 0 7
57 7 4

water-to-light map:
88 18 7
18 25 70

light-to-temperature map:
45 77 23
81 45 19
68 64 13

temperature-to-humidity map:
0 69 1
1 0 69

humidity-to-location map:
60 56 37
56 93 4)", 35, -2}
};

using namespace boost::icl;
using mapping = interval_map<int, int>;
using ascii::alpha;

const x3::rule<class mapping_, mapping> mapping_ = "mapping";
const auto add_mapping = [](auto& ctx) {
    auto dest = at_c<0>(_attr(ctx));
    auto source = at_c<1>(_attr(ctx));
    auto length = at_c<2>(_attr(ctx));
    _val(ctx).add(std::make_pair(interval<int>::right_open(source, source + length), source - dest));
};
const auto mapping__def = *((int_ >> int_ >> int_)[add_mapping]);
BOOST_SPIRIT_DEFINE(mapping_);

auto parse(std::string_view s) {
    auto it = s.begin();
    auto seeds = std::vector<int>{};
    phrase_parse(it, s.end(),
        "seeds: " >> *int_,
        space, seeds);

    auto raw_maps = std::tuple<std::string, std::string>{};
    phrase_parse(it, s.end(),
        *alpha >> "-to-" >> *alpha >> "map:",
        space, raw_maps);

    //auto maps = raw_maps 
    //    | rv::transform([](auto ms) {
    //        auto [from, to, m] = ms;
    //        return std::tuple{ from, std::tuple{to,m} };
    //    })
    //    | ranges::to<std::unordered_map<std::string, std::tuple<std::string, mapping>>>;
    //return std::tuple{ seeds, maps };
    return seeds;
}

auto run_a(std::string_view s) {
    //const auto [seeds, maps] = parse(s);
    parse(s);

    return -1;
}

auto run_b(std::string_view s) {
    return -1;
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
