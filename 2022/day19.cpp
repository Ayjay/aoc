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
using x3::omit;
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
  Each geode robot costs 3 ore and 12 obsidian.)", 33, -1}
};

struct state_t {
    int time = 1;
    int ore_robots = 1;
    int clay_robots = 0;
    int obsidian_robots = 0;
    int geode_robots = 0;
    int ore = 0;
    int clay = 0;
    int obsidian = 0;
    int geodes = 0;

    void step() {
        ++time;
        ore += ore_robots;
        clay += clay_robots;
        obsidian += obsidian_robots;
        geodes += geode_robots;
    }
};

struct blueprint_t {
    int id;
    int ore_ore;
    int clay_ore;
    int obsidian_ore;
    int obsidian_clay;
    int geode_ore;
    int geode_obsidian;
};

BOOST_FUSION_ADAPT_STRUCT(
    blueprint_t,
    id, ore_ore, clay_ore, obsidian_ore, obsidian_clay, geode_ore, geode_obsidian
)

auto parse(std::string_view s) {
    auto ret = std::vector<blueprint_t>{};
    phrase_parse(s.begin(), s.end(),
        *(lit("Blueprint") >> int_ >> ':' >>
          "Each ore robot costs" >> int_ >> "ore." >>
          "Each clay robot costs" >> int_  >> "ore." >>
          "Each obsidian robot costs" >> int_ >> "ore and" >> int_ >> "clay." >>
          "Each geode robot costs" >> int_ >> "ore and" >> int_ >> "obsidian."),
        space, ret);
    return ret;
}

enum robot_types {
    ore,
    clay,
    obsidian,
    geode
};

const auto breakpoints = std::unordered_map{
    std::pair{3,clay},
    {5,clay},
    {7,clay},
    {11,obsidian},
    {12,clay},
    {15,obsidian},
    {18,geode},
    {21,geode}
};

constexpr auto total_steps = 24;
int geode_count(const blueprint_t& blueprint, state_t initial_state, bool trace_path = true) {
    auto best = 0;
    for (auto next_to_build : rv::iota(static_cast<int>(ore),static_cast<int>(geode)+1)) {
        auto state = initial_state;
        auto build = false;
        while (!build && state.time <= total_steps) {
            switch (next_to_build) {
            case ore:
                if (state.ore >= blueprint.ore_ore) {
                    state.ore -= blueprint.ore_ore;
                    build = true;
                }
                break;
            case clay:
                if (state.ore >= blueprint.clay_ore) {
                    state.ore -= blueprint.clay_ore;
                    build = true;
                }
                break;
            case obsidian:
                if (state.ore >= blueprint.obsidian_ore && state.clay >= blueprint.obsidian_clay) {
                    state.ore -= blueprint.obsidian_ore;
                    state.clay -= blueprint.obsidian_clay;
                    build = true;
                }
                break;
            case geode:
                if (state.ore >= blueprint.geode_ore && state.obsidian >= blueprint.geode_obsidian) {
                    state.ore -= blueprint.geode_ore;
                    state.obsidian -= blueprint.geode_obsidian;
                    build = true;
                }
                break;
            }
            state.step();
            if (build) {
                switch (next_to_build) {
                case 0: ++state.ore_robots; break;
                case 1: ++state.clay_robots; break;
                case 2: ++state.obsidian_robots; break;
                case 3: ++state.geode_robots; break;
                }
            }
        }
        if (build) {
            auto breakpoint = breakpoints.find(state.time);
            best = std::max(best, geode_count(blueprint, state, trace_path && breakpoint != breakpoints.end() && breakpoint->second == next_to_build));
        }
        else {
            best = std::max(best, state.geodes);
        }
    }

    return best;
}

auto quality_level(const blueprint_t& blueprint) {
    return blueprint.id * geode_count(blueprint, {});
}

auto run_a(std::string_view s) {
    auto blueprints = parse(s);
    return reduce(blueprints | rv::transform([](const auto& bp) { return quality_level(bp); }));
}
 
auto run_b(std::string_view s) {
    auto blueprints = parse(s);
    return -2;
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
