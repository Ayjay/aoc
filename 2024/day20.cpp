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

#include <boost/container/static_vector.hpp>
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
const auto test_data =
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
###############)";

struct cheat {
    vector2 pos;
    vector2 end;
};

const auto get_cheats = [](const grid_t& g) {
    auto cells = g.cells();
    const auto is_valid = [&](vector2 p) {
        auto c = g.checked_get(p);
        return c and c != '#';
    };
    const auto cell_cheats = [is_valid](vector2 v) {
        const auto make_cheat =
            [v, is_valid](
                vector2 dir) -> boost::container::static_vector<cheat, 1> {
            if (not is_valid(v + dir) and is_valid(v + dir + dir))
                return {cheat{v, v + dir + dir}};
            else if (not is_valid(v + dir) and not is_valid(v + dir + dir) and
                     is_valid(v + dir + dir + dir))
                return {cheat{v, v + dir + dir + dir}};
            else
                return {};
        };
        static auto dirs = std::array{right, down};
        return dirs | rv::for_each(make_cheat);
    };
    return cells | rv::filter(is_valid) | rv::for_each(cell_cheats);
};

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

auto time(
    Graph& graph,
    const boost::unordered_map<vector2, Graph::vertex_descriptor>& vertices,
    vector2 start,
    vector2 end,
    std::optional<cheat> c) {
    auto edge = Graph::edge_descriptor{};
    if (c) {
        bool inserted;
        std::tie(edge, inserted) =
            add_edge(vertices.at(c->pos), vertices.at(c->end), graph);
        get(boost::edge_weight, graph)[edge] =
            manhattan_distance(c->pos, c->end);
    }

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
                .visitor(astar_goal_visitor<Graph::vertex_descriptor>{
                    vertices.at(end)}));
    } catch (found_goal) {
    }

    if (c) {
        remove_edge(edge, graph);
    }

    return d[vertices.at(end)];
}

auto time(const grid_t& g, std::optional<cheat> c) {
    auto [graph, vertices] = make_graph(g);
    const auto start = g.find_single('S');
    const auto end = g.find_single('E');
    return time(graph, vertices, start, end, c);
}

TEST_CASE("no-cheat", "[day20]") {
    auto g = grid_t{test_data};
    REQUIRE(time(g, {}) == 84);
}

TEST_CASE("specific cheat", "[day20]") {
    const auto g = grid_t{test_data};
    const auto t = time(g, cheat{{1, 7}, {1, 10}});
    REQUIRE(t == 72);
}

i64 run_a(std::string_view s) {
    const auto g = grid_t{s};

    auto cheats = get_cheats(g);
    auto [graph, vertices] = make_graph(g);
    const auto start = g.find_single('S');
    const auto end = g.find_single('E');
    const auto base_time = time(graph, vertices, start, end, {});
    return ranges::count_if(cheats, [&](cheat c) {
        return base_time - time(graph, vertices, start, end, c) >= 100;
    });
}

auto run_b(std::string_view s) {
    return -1;
}

TEST_CASE("day20a", "[day20]") {
    auto m = grid_t{test_data};
    const auto base_time = time(m, {});
    auto cheats = get_cheats(m);
    auto counts = boost::unordered_map<i64, i64>{};
    for (auto c : cheats)
        ++counts[base_time - time(m, c)];

    REQUIRE(counts[2] == 14);
    REQUIRE(counts[4] == 14);
    REQUIRE(counts[6] == 2);
    REQUIRE(counts[8] == 4);
    REQUIRE(counts[10] == 2);
    REQUIRE(counts[12] == 3);
    REQUIRE(counts[20] == 1);
    REQUIRE(counts[36] == 1);
    REQUIRE(counts[38] == 1);
    REQUIRE(counts[40] == 1);
    REQUIRE(counts[64] == 1);
}

TEST_CASE("day20b", "[day20]") {}

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
