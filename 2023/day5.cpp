//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <vector>
#include <tuple>
#include <string_view>
#include <unordered_set>
#include <chrono>
#include <iostream>

#include <boost/unordered_map.hpp>
#include <boost/icl/interval_map.hpp>
#include <fmt/ostream.h>

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
using mapping = interval_map<long long, long long>;
template <> struct fmt::formatter<mapping> : ostream_formatter {};

using ascii::alpha;

const x3::rule<class mapping_, mapping> mapping_ = "mapping";
const auto add_mapping = [](auto& ctx) {
    auto dest = at_c<0>(_attr(ctx));
    auto source = at_c<1>(_attr(ctx));
    auto length = at_c<2>(_attr(ctx));
    _val(ctx).add(std::make_pair(interval<long long>::right_open(source, source + length), dest - source));
};
const auto mapping__def = *(long_long >> long_long >> long_long)[add_mapping];
BOOST_SPIRIT_DEFINE(mapping_);

auto get_mapping(const mapping& map, long long index) {
    if (auto it = map.find(index); it != map.end())
        return index + it->second;
    return index;
}

auto parse(std::string_view s) {
    auto it = s.begin();
    auto seeds = std::vector<long long>{};
    phrase_parse(it, s.end(),
        "seeds: " >> *long_long,
        space, seeds);

    auto raw_maps = std::vector<std::tuple<std::string, std::string, mapping>>{};
    phrase_parse(it, s.end(),
        *(*alpha >> "-to-" >> x3::lexeme[*alpha] >> "map:" >> mapping_),
        space, raw_maps);

    auto maps = raw_maps 
        | rv::transform([](auto ms) {
            auto [from, to, m] = ms;
            auto get_mapping = [map=std::move(m)](long long index) {
                if (auto it = map.find(index); it != map.end())
                    return index + it->second;
                return index;
            };
            return std::pair{ from, std::tuple{to,std::move(get_mapping)} };
        })
        | ranges::to<std::unordered_map>;
    return std::tuple{ seeds, maps };
}

auto get_location(const auto& maps, long long seed, const auto& type) {
    const auto& [to, map] = maps.at(type);
    if (to == "location")
        return map(seed);
    else
        return get_location(maps, map(seed), to);
};

auto run_a(std::string_view s) {
    const auto [seeds, maps] = parse(s);

    const auto locations = seeds
       | rv::transform([&](auto seed) { return get_location(maps, seed, "seed"); })
       | ranges::to<std::vector>;

    return *ranges::min_element(locations);
}

auto run_b(std::string_view s) {
    return -1;
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
