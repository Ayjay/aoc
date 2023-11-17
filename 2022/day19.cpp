//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <tuple>
#include <cmath>
#include <array>
#include <iostream>
#include <deque>
#include <unordered_set>
#include <map>
#include <utility>

#include <fmt/std.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>

#include <boost/array.hpp>

#include <boost/unordered_set.hpp>
#include <boost/container_hash/hash.hpp>
#include <boost/icl/interval_set.hpp>

#include <boost/fusion/adapted.hpp>
namespace fusion = boost::fusion;

#include <boost/spirit/home/x3.hpp>

namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;

using x3::char_;
using x3::int_;
using x3::long_long;
using ascii::space;
using ascii::lit;
using boost::spirit::x3::phrase_parse;

const auto test_data = std::vector{ std::tuple
{R"(Blueprint 1:
  Each ore robot costs 4 ore.
  Each clay robot costs 2 ore.
  Each obsidian robot costs 3 ore and 14 clay.
  Each geode robot costs 2 ore and 7 obsidian.

Blueprint 2:
  Each ore robot costs 2 ore.
  Each clay robot costs 3 ore.
  Each obsidian robot costs 3 ore and 8 clay.
  Each geode robot costs 3 ore and 12 obsidian.)", 9, -1}
};

struct blueprint_t {
    int ore_ore;
    int clay_ore;
    int obsidian_ore;
    int obsidian_clay;
    int geode_ore;
    int geode_clay;
};

BOOST_FUSION_ADAPT_STRUCT(
    blueprint_t,
    ore_ore, clay_ore, obsidian_ore, obsidian_clay, geode_ore, geode_clay
)

auto parse(std::string_view s) {
    auto ret = std::vector<blueprint_t>{};
    phrase_parse(s.begin(), s.end(),
        *(lit_("Blueprint 1:") >> 
          "Each ore robot costs" >> int_ "ore." >>
          "Each clay robot costs" >> int_  >> "ore." >>
          "Each obsidian robot costs" int_ >> "ore and" >> int_ "clay." >>
          "Each geode robot costs" >> int_ >> "ore and" >> int_ >> "obsidian."
        space, ret);
    return ret;
}

auto run_a(std::string_view s) {
    auto blueprints = parse(s);
    return -2;
}

auto run_b(std::string_view s) {
    auto blueprints = parse(s);
    return -2;
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
