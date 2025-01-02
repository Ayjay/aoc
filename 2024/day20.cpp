#include "aoc2024.h"

#include <execution>
#include <iterator>
#include <memory>
#include <queue>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/astar_search.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#include <catch2/catch_test_macros.hpp>

#include "grid.hpp"
#include "simple_timer.hpp"

namespace day20 {
using i64 = long long;
using namespace grid;
const auto test_data = std::vector{
    std::tuple<std::string_view, std::optional<i64>, std::optional<i64>>{
        R"(###############
#...#...#.....#
#.#.#.#.#.###.#
#S#...#.#.#...#
#######.#.#.###
#######.#.#...#
#######.#.###.#
###..E#...#...#
###.#######.###
#...###...#...#
#.#####.#.###.#
#.#...#.#.#...#
#.#.#.#.#.#.###
#...#...#...###
###############)",
        {},
        {}}};

struct cheat {
    vector2 pos;
    vector2 dir;
};

const auto get_cheats = [](const grid_t& g) { 
    auto cells = g.cells();
    auto valid_cells = cells | rv::filter([&](vector2 p) { return g.get(p) != '#'; });
    auto valid_cells = cells | rv::filter([&](vector2 p) { return g.get(p) != '#'; });
;

namespace detail {
using namespace boost;
using Graph = adjacency_list<vecS,
                             vecS,
                             undirectedS,
                             property<vertex_name_t, vector2>,
                             property<edge_weight_t, i64>>;
}  // namespace detail
using detail::Graph;

struct found_goal {};  // exception for termination

// visitor that terminates when we find the goal
template <class Vertex>
class astar_goal_visitor : public boost::default_astar_visitor {
   public:
    explicit astar_goal_visitor(Vertex goal) : m_goal(goal) {}
    template <class Graph>
    void examine_vertex(Vertex u, Graph& g) {
        if (u == m_goal)
            throw found_goal();
    }

   private:
    Vertex m_goal;
};

auto make_graph(const grid_t& g) {
    const auto is_valid = [&](vector2 v) { return g.get(v) != '#'; };
    Graph graph;
    auto vertex_name = get(boost::vertex_name, graph);
    auto edge_weights = get(boost::edge_weight, graph);
    auto vertices = boost::unordered_map<vector2, Graph::vertex_descriptor>{};
    for (const auto c : g.cells()) {
        if (is_valid(c)) {
            auto u = add_vertex(graph);
            vertex_name[u] = c;
            vertices[c] = u;
        }
    }

    for (const auto [u, v] : vertices) {
        if (is_valid(u + right)) {
            const auto [edge, inserted] =
                add_edge(v, vertices.at(u + right), graph);
            edge_weights[edge] = 1;
        }
        if (is_valid(u + down)) {
            const auto [edge, inserted] =
                add_edge(v, vertices.at(u + down), graph);
            edge_weights[edge] = 1;
        }
    }

    return std::tuple{graph, vertices};
}

auto time(const grid_t& g, std::optional<cheat> c) {
    const auto [graph, vertices] = make_graph(g);
    const auto start = g.find_single('S');
    const auto end = g.find_single('E');

    auto d = std::vector<i64>(num_vertices(graph));
    auto vertex_name = get(boost::vertex_name, graph);

    const auto heuristic = [&](Graph::vertex_descriptor p) {
        return distance(vertex_name[p], end);
    };

    try {
    boost::astar_search(
        graph, vertices.at(start), heuristic,
        boost::distance_map(boost::make_iterator_property_map(
                                d.begin(), get(boost::vertex_index, graph)))
            .visitor(astar_goal_visitor<Graph::vertex_descriptor>{vertices.at(end)}));
    } catch (found_goal) {}

    return d[vertices.at(end)];
}

TEST_CASE("no-cheat", "[day20]") {
    const auto [s, _a, _b] = test_data.front();
    auto g = grid_t{s};
    REQUIRE(time(g, {}) == 84);
}

i64 run_a(std::string_view s) {
    const auto m = grid_t{s};
    return -1;
}

auto run_b(std::string_view s) {
    return -1;
}

TEST_CASE("day20a", "[day20]") {
    for (const auto& test : test_data) {
        const auto [s, expected, _] = test;
        if (expected) {
            REQUIRE(run_a(s) == *expected);
        }
    }
}

TEST_CASE("day20b", "[day20]") {
    for (const auto& test : test_data) {
        const auto [s, _, expected] = test;
        if (expected) {
            REQUIRE(run_b(s) == *expected);
        }
    }
}

}  // namespace day20

WEAK void entry() {
    using namespace day20;
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
