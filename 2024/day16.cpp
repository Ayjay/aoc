#include "aoc2024.h"

#include <cmath>
#include <iterator>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>

#include <boost/container_hash/hash.hpp>
#include <boost/describe.hpp>
#include <boost/graph/astar_search.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/property_map/function_property_map.hpp>
#include <boost/unordered_map.hpp>

#include <range/v3/algorithm/min.hpp>

#include <catch2/catch_test_macros.hpp>

#include "grid.hpp"
#include "simple_timer.hpp"

namespace day16 {
using i64 = long long;
const auto test_data = std::vector{
    std::tuple<std::string_view, std::optional<i64>, std::optional<i64>>{
        R"(###############
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
###############)",
        7036,
        {}},
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
#################)",
     11048,
     {}}};

using namespace grid;

std::string_view dir_to_string(vector2 dir) {
    if (dir == up)
        return "up";
    if (dir == down)
        return "down";
    if (dir == left)
        return "left";
    return "right";
}

struct vertex {
    vector2 pos;
    vector2 facing;
    auto operator<=>(const vertex&) const = default;
    friend std::ostream& operator<<(std::ostream& os, const vertex& v) {
        return os << fmt::format("[p={} f={}]", v.pos, dir_to_string(v.facing));
    }
};
BOOST_DESCRIBE_STRUCT(vertex, (), (pos, facing));
}  // namespace day16
template <>
struct fmt::formatter<day16::vertex> : ostream_formatter {};

namespace day16 {
struct edge {
    vertex start;
    vector2 direction;
    auto operator<=>(const edge&) const = default;
    friend std::ostream& operator<<(std::ostream& os, const edge& e) {
        return os << fmt::format("[{}] {}", e.start,
                                 dir_to_string(e.direction));
    }
};
BOOST_DESCRIBE_STRUCT(edge, (), (start, direction));
}  // namespace day16

template <>
struct fmt::formatter<day16::edge> : ostream_formatter {};

namespace day16 {
vertex source(edge e, const grid_t& g) {
    return e.start;
}

vertex target(edge e, const grid_t& g) {
    return {e.start.pos + e.direction, e.direction};
}

struct edge_iterator
    : public boost::iterator_facade<edge_iterator,
                                    edge,
                                    boost::forward_traversal_tag,
                                    edge> {
    const grid_t* g = nullptr;
    vertex start = {{0, 0}, {0, 0}};
    int stage = 0;
    static const int max_stages = 3;
    edge_iterator() : stage(max_stages) {}

    edge_iterator(const grid_t& g, vertex start) : g(&g), start(start) {
        forward();
    }

    edge dereference() const { return {start, stage_to_dir()}; }
    bool equal(const edge_iterator& other) const {
        return stage == other.stage;
    }
    void increment() {
        ++stage;
        forward();
    }

   private:
    vector2 stage_to_dir() const {
        switch (stage) {
            case 0:
                return turn_left(start.facing);
            case 1:
                return start.facing;
            default:
                return turn_right(start.facing);
        }
    }
    void forward() {
        for (; stage < max_stages and g->get(start.pos + stage_to_dir()) == '#';
             ++stage)
            ;
    }
};

auto out_edges(vertex u, const grid_t& g) {
    return std::pair{edge_iterator{g, u}, edge_iterator{}};
}

auto out_degree(vertex u, const grid_t& g) {
    auto edges = out_edges(u, g);
    return std::distance(edges.first, edges.second);
}

}  // namespace day16

namespace boost {
template <>
struct graph_traits<grid::grid_t> {
    using vertex_descriptor = day16::vertex;
    using edge_descriptor = day16::edge;
    using directed_category = directed_tag;
    using edge_parallel_category = disallow_parallel_edge_tag;
    using traversal_category = incidence_graph_tag;
    using out_edge_iterator =
        decltype(day16::out_edges(std::declval<day16::vertex>(),
                                  std::declval<grid::grid_t>())
                     .first);
    using degree_size_type =
        decltype(day16::out_degree(std::declval<day16::vertex>(),
                                   std::declval<grid::grid_t>()));

    static_assert(std::input_iterator<out_edge_iterator>);

    static vertex_descriptor null_vertex() { return {{0, 0}, {0, 0}}; }
};
}  // namespace boost

BOOST_CONCEPT_ASSERT((boost::concepts::Graph<grid::grid_t>));
BOOST_CONCEPT_ASSERT((boost::concepts::IncidenceGraph<grid::grid_t>));

namespace day16 {

class astar_goal_visitor : public boost::default_astar_visitor {
   public:
};

i64 run_a(std::string_view s) {
    const auto maze = grid_t{s};
    auto cells = maze.cells();
    const vector2 start = *ranges::find(cells, 'S', maze.cell_getter());
    const auto start_vertex = vertex{start, right};
    const vector2 end = *ranges::find(cells, 'E', maze.cell_getter());
    const auto heuristic = [=](vertex v) { return distance(v.pos, end); };

    const auto edge_weight = [](edge e) {
        if (e.direction == e.start.facing)
            return 1;
        else
            return 1001;
    };
    auto predecessors = boost::unordered_map<vertex, vertex>{};
    auto ranks = boost::unordered_map<vertex, decltype(heuristic({}))>{};
    auto distances = boost::unordered_map<vertex, int>{};
    distances[start_vertex] = 0;
    auto distance_prop_fn = [&](vertex v) -> int& {
        return distances.insert({v, std::numeric_limits<int>::max()})
            .first->second;
    };

    boost::astar_search_no_init_tree(
        maze, start_vertex, heuristic,
        boost::weight_map(
            boost::function_property_map<decltype(edge_weight), edge,
                                         decltype(edge_weight({}))>(
                edge_weight))
            .visitor(astar_goal_visitor{})
            .predecessor_map(boost::make_assoc_property_map(predecessors))
            .rank_map(boost::make_assoc_property_map(ranks))
            .distance_map(
                boost::function_property_map<decltype(distance_prop_fn), vertex,
                                             int&>(distance_prop_fn)));

    auto goal_vertices =
        directions |
        rv::transform([&](vector2 dir) { return vertex{end, dir}; }) |
        rv::transform([&](vertex v) {
            auto it = distances.find(v);
            return it == distances.end() ? std::numeric_limits<int>::max()
                                         : it->second;
        });
    return ranges::min(goal_vertices);
}

auto run_b(std::string_view s) {
    return -1;
}

TEST_CASE("day16a", "[day16]") {
    for (const auto& test : test_data) {
        const auto [s, expected, _] = test;
        if (expected) {
            REQUIRE(run_a(s) == *expected);
        }
    }
}

TEST_CASE("day16b", "[day16]") {
    for (const auto& test : test_data) {
        const auto [s, _, expected] = test;
        if (expected) {
            REQUIRE(run_b(s) == *expected);
        }
    }
}

}  // namespace day16

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