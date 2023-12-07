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

#include <cppcoro/generator.hpp>

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
56 93 4)", 35, 46}
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

auto parse_a(std::string_view s) {
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
    const auto [seeds, maps] = parse_a(s);

    const auto locations = seeds
       | rv::transform([&](auto seed) { return get_location(maps, seed, "seed"); })
       | ranges::to<std::vector>;

    return *ranges::min_element(locations);
}

struct seed_range_t {
    long long lo;
    long long hi;
    friend auto operator<=>(const seed_range_t& left, const seed_range_t& right) {
        return left.lo <=> right.lo;
    }
};
BOOST_FUSION_ADAPT_STRUCT(seed_range_t, lo, hi);

const x3::rule<class seed_range, seed_range_t> seed_range = "seed range";
const auto make_seed_range = [](auto& ctx) {
    _val(ctx).lo = at_c<0>(_attr(ctx));
    _val(ctx).hi = _val(ctx).lo + at_c<1>(_attr(ctx));
};
const auto seed_range_def = (long_long >> long_long)[make_seed_range];
BOOST_SPIRIT_DEFINE(seed_range);

auto parse_b(std::string_view s) {
    auto it = s.begin();
    auto seeds = std::vector<seed_range_t>{};
    phrase_parse(it, s.end(),
        "seeds: " >> *seed_range,
        space, seeds);

    auto raw_maps = std::vector<std::tuple<std::string, std::string, std::vector<std::tuple<long long, long long, long long>>>>{};
    phrase_parse(it, s.end(),
        *(*alpha >> "-to-" >> x3::lexeme[*alpha] >> "map:" >> *(long_long >> long_long >> long_long)),
        space, raw_maps);

    auto maps = raw_maps
        | rv::transform([=](auto& v) {
            auto& [from, to, raw_map] = v;
            const auto make_mapping = [](auto m) {
                auto [dest, source, length] = m;
                return std::tuple{ seed_range_t{source, source + length}, dest - source };
            };
            auto map = raw_map | rv::transform(make_mapping) | ranges::to<std::vector>;
            ranges::sort(map, std::less{}, [](auto u) {
                auto [rng, mod] = u;
                return rng.lo;
            });

            return std::tuple{ std::move(from), std::tuple{ std::move(to), std::move(map) } };
        })
        | ranges::to<std::map>;

    return std::tuple{ seeds, maps };
}

auto get_intervals(const auto& map, const auto seed_range) {
    auto ret = std::vector<seed_range_t>{};

    const auto [lo, hi] = seed_range;
    auto it = ranges::find_if(map, [&](const auto& mapping) { 
        const auto& [rng, mod] = mapping;
        return lo >= rng.lo && lo < rng.hi;
    });
    if (it == map.end()) {
        it = std::upper_bound(
            map.begin(), map.end(),
            lo,
            [](const auto val, const auto& interval) { return val < std::get<0>(interval).hi; }
        );
        ret.push_back({ lo, it == map.end() ? hi : std::get<0>(*it).lo });
    }

    while (it != map.end() && std::get<0>(*it).lo < hi) {
        const auto& [rng, mod] = *it;
        ret.push_back({ std::max(rng.lo, lo) + mod, std::min(rng.hi, hi) + mod});

        const auto next = it+1;
        const auto next_hi = next == map.end() ? hi : std::min(std::get<0>(*next).lo, hi);
        if (rng.hi < next_hi)
            ret.push_back({ rng.hi, next_hi });
        it = next;
    }

    return ret;
}

auto get_locations(const auto& maps, const auto seed_range, const auto& type) {
    const auto& [to,map] = maps.at(type);
    auto intervals = get_intervals(map, seed_range);
    //assert(reduce(intervals | rv::transform([](auto r) { return r.hi - r.lo; })) == seed_range.hi - seed_range.lo);

    if (to == "location")
        return intervals;
    else
        return intervals
      | rv::transform([&](auto range) { return get_locations(maps, range, to); }) 
      | rv::join 
      | ranges::to<std::vector>;
};


auto run_b(std::string_view s) {
    const auto [seed_ranges, maps] = parse_b(s);
    const auto locations = seed_ranges
        | rv::transform([&](auto range) { return get_locations(maps, range, "seed"); })
        | ranges::to<std::vector>;
    const auto joined_locations = locations | rv::join | ranges::to<std::vector>;
    return std::min_element(joined_locations.begin(), joined_locations.end())->lo;
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
