#include "aoc2024.h"

#include <vector>
#include <tuple>
#include <string_view>
#include <utility>
#include <cmath>
#include <list>
#include <unordered_map>

#include <fmt/core.h>

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/multi_array.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graph_concepts.hpp>

#include <catch2/catch_test_macros.hpp>

#include "grid.hpp"
#include "simple_timer.hpp"

namespace day16 {
using i64 = long long;
const auto test_data = std::vector{ std::tuple<std::string_view, std::optional<i64>, std::optional<i64>>
{R"(###############
#.......#....E#
#.#.###.#.###.#
#.....#.#...#.#
#.###.#####.#.#
#.#.#.......#.#
#.#.#####.###.#
#...........#.#
###.#.#####.#.#
#...#.....#.#.#
#.#.#.###.#.#.#
#.....#...#.#.#
#.###.#.#.#.#.#
#S..#.....#...#
###############)", 7036, {}},
{R"(#################
#...#...#...#..E#
#.#.#.#.#.#.#.#.#
#.#.#.#...#...#.#
#.#.#.#.###.#.#.#
#...#.#.#.....#.#
#.#.#.#.#.#####.#
#.#...#.#.#.....#
#.#.#####.#.###.#
#.#.#.......#...#
#.#.###.#####.###
#.#.#...#.....#.#
#.#.#.#####.###.#
#.#.#.........#.#
#.#.#.#########.#
#S#.............#
#################)", 11048, {}}
};

using namespace grid;

struct vertex {
    vector2 pos;
    vector2 facing;
};

struct edge {
    vertex start;
    vector2 direction;
};

}

namespace boost {
    template <>
    struct graph_traits<grid::grid_t> {
        using vertex_descriptor = day16::vertex;
        using edge_descriptor = day16::edge;
        using directed_category = directed_tag;
        using edge_parallel_category = disallow_parallel_edge_tag;
        using traversal_category = adjacency_graph_tag;

        static vertex_descriptor null_vertex() {
            return {{0,0},{0,0}};
        }
    };
}

BOOST_CONCEPT_ASSERT(( boost::graph::Graph<grid::grid_t> ));

namespace day16 {
i64 run_a(std::string_view s) {
    // const auto maze = grid_t{s};
    // const auto start = ranges::find(maze.cells(), 'S', maze.cell_getter());
    // const auto end = ranges::find(maze.cells(), 'E', maze.cell_getter());
    // const auto can_move = [&](vector2 p) { return maze.get(p) != '#'; };
    // const auto vertices = maze.cells() | rv::filter(can_move) | ranges::to<std::vector>;

    // const auto edges = [&](vertex v) -> vertex {
    //     if (can_move(v.pos + turn_left(v.facing)))
    //         co_yield v.pos + turn_left(v.facing);
    //     if (can_move(v.pos + v.facing))
    //         co_yield v.pos + v.facing;
    //     if (can_move(v.pos + turn_right(v.facing)))
    //         co_yield v.pos + turn_right(v.facing);
    // };


    // auto distances = boost::multi_array<i64, 3>{boost::extents[maze.rows][maze.cols][4]};

    return 0;
}

auto run_b(std::string_view s) {
    return -1;
}

TEST_CASE("day16a", "[day16]") {
    for (const auto& test : test_data) {
        const auto [s,expected,_] = test;
        if (expected) {
            REQUIRE(run_a(s) == *expected);
        }
    }
}

TEST_CASE("day16b", "[day16]") {
    for (const auto& test : test_data) {
        const auto [s,_,expected] = test;
        if (expected) {
            REQUIRE(run_b(s) == *expected);
        }
    }
}

}

WEAK void entry() {
    using namespace day16;
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