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
#include <boost/unordered_set.hpp>

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
        7036, 45},
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
     11048, 64}};

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

const auto edge_weight = [](edge e) {
    if (e.direction == e.start.facing)
        return 1;
    else
        return 1001;
};

class astar_goal_visitor : public boost::default_astar_visitor {
   public:
};

auto find_paths(std::string_view s) {
    const auto maze = grid_t{s};
    const auto cells = maze.cells();
    const auto start = *ranges::find(cells, 'S', maze.cell_getter());
    const auto start_vertex = vertex{start, right};
    const auto end = *ranges::find(cells, 'E', maze.cell_getter());

    auto predecessors = boost::unordered_multimap<vertex, vertex>{};
    auto distances = boost::unordered_map<vertex, int>{};
    distances[start_vertex] = 0;
    const auto distance_prop_fn = [&](vertex v) -> int& {
        return distances.insert({v, std::numeric_limits<int>::max()})
            .first->second;
    };

    auto stack = std::vector{start_vertex};
    while (!stack.empty()) {
        const auto v = stack.back();
        stack.pop_back();

        auto& v_distance = distance_prop_fn(v);
        for (auto [edge_it, edge_end] = out_edges(v, maze); edge_it != edge_end;
             ++edge_it) {
            const auto e = *edge_it;
            const auto w = target(e, maze);
            const auto e_weight = edge_weight(e);
            auto& w_weight = distance_prop_fn(w);
            if (w_weight == std::numeric_limits<int>::max())
                stack.push_back(w);
            if (v_distance + e_weight < w_weight) {
                w_weight = v_distance + e_weight;
                predecessors.erase(w);
            }
            if (v_distance + e_weight <= w_weight)
                predecessors.insert({w, v});
        }
    }

    return std::tuple{maze, start, end, predecessors, distances};
}

i64 run_a(std::string_view s) {
    const auto [_, _, end, _, distances] = find_paths(s);
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

auto find_all_minimal(auto range, auto proj = std::identity{}) {
    using monoid = std::decay_t<decltype(proj(
        std::declval<ranges::range_value_t<decltype(range)>>()))>;
    const auto combine = [&](auto acc, auto el) {
        auto& [current, min_elements] = acc;
        auto val = proj(el);
        if (val < current) {
            current = val;
            min_elements.clear();
        }
        if (val <= current)
            min_elements.push_back(el);
        return std::move(acc);
    };
    auto get_elements = [](auto acc) { return std::get<1>(acc); };
    return std::get<1>(ranges::accumulate(
        range,
        std::tuple{std::numeric_limits<monoid>::max(),
                   std::vector<ranges::range_value_t<decltype(range)>>{}},
        combine));
}

TEST_CASE("find_all_minimal", "[day16]") {
    REQUIRE(find_all_minimal<std::vector<int>, std::identity>(
                std::vector{1, 1, 2, 3}) == std::vector{1, 1});
}

auto run_b(std::string_view s) {
    const auto [_, start, end, predecessors, distances] = find_paths(s);

    fmt::println("{}", fmt::join(predecessors, "\n"));

    const auto goal_distance = [&](vertex v) {
        auto it = distances.find(v);
        return it == distances.end() ? std::numeric_limits<int>::max()
                                     : it->second;
    };
    const auto goal_vertices = find_all_minimal(
        directions |
            rv::transform([&](vector2 dir) { return vertex{end, dir}; }),
        goal_distance);
    auto shortest_tiles = boost::unordered_set<vertex>{};
    auto shortest_stack = goal_vertices;
    while (not shortest_stack.empty()) {
        const auto v = shortest_stack.back();
        shortest_stack.pop_back();

        if (v != vertex{start, right} and shortest_tiles.insert(v).second) {
            const auto predec = predecessors.equal_range(v);
            const auto predec_range =
                ranges::subrange{predec.first, predec.second};
            const auto get_second = [](auto kv) { return kv.second; };
            const auto predec_values = predec_range | rv::transform(get_second);
            shortest_stack.insert(shortest_stack.end(), predec_values.begin(),
                                  predec_values.end());
        }
    }
    return shortest_tiles.size();
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