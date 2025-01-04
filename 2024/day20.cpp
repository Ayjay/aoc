#include "aoc2024.h"

#include <map>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/unordered_map.hpp>

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
    friend std::ostream& operator<<(std::ostream& os, const cheat& c) {
        return os << fmt::format("[{}-{}]", c.pos, c.end);
    }
};

}  // namespace day20

template <>
struct fmt::formatter<day20::cheat> : ostream_formatter {};

namespace day20 {

namespace detail {
using namespace boost;
using Graph = adjacency_list<vecS, vecS, undirectedS>;
}  // namespace detail
using detail::Graph;

auto make_graph(const grid_t& g) {
    const auto is_valid = [&](vector2 v) { return g.get(v) != '#'; };
    Graph graph;
    auto vertices = boost::unordered_map<vector2, Graph::vertex_descriptor>{};
    for (const auto c : g.cells()) {
        if (is_valid(c)) {
            auto u = add_vertex(graph);
            vertices[c] = u;
        }
    }

    for (const auto [u, v] : vertices) {
        if (is_valid(u + right))
            add_edge(v, vertices.at(u + right), graph);
        if (is_valid(u + down))
            add_edge(v, vertices.at(u + down), graph);
    }

    return std::tuple{graph, vertices};
}

auto solve(const grid_t& g, i64 max_cheat_length) {
    auto [graph, vertices] = make_graph(g);
    const auto start = g.find_single('S');
    const auto end = g.find_single('E');

    auto start_d = std::vector<i64>(num_vertices(graph));
    auto end_d = std::vector<i64>(num_vertices(graph));

    boost::breadth_first_search(
        graph, vertices.at(start),
        boost::visitor(boost::make_bfs_visitor(
            boost::record_distances(start_d.data(), boost::on_tree_edge()))));

    boost::breadth_first_search(
        graph, vertices.at(end),
        boost::visitor(boost::make_bfs_visitor(
            boost::record_distances(end_d.data(), boost::on_tree_edge()))));

    const auto base_distance = start_d[vertices.at(end)];

    const auto time_save = [&](cheat c) {
        const auto [c_pos_r, c_pos_c] = c.pos;
        const auto t = start_d[vertices.at(c.pos)] +
                       manhattan_distance(c.pos, c.end) +
                       end_d[vertices.at(c.end)];
        return t < base_distance ? base_distance - t : 0;
    };

    const auto is_valid = [&](vector2 p) {
        auto c = g.checked_get(p);
        return c and c != '#';
    };

    const auto cheats = [&](vector2 v) {
        const auto [v_r, v_c] = v;
        const auto to_cheat = [=](auto u) { return cheat{v, u}; };
        const auto make_column = [=](auto row) {
            auto cols = max_cheat_length - std::abs(row);
            return rv::iota(-cols, cols + 1) | rv::transform([=](auto col) {
                       return vector2{v_r + row, v_c + col};
                   });
        };
        return rv::iota(-max_cheat_length, max_cheat_length + 1) |
               rv::for_each(make_column) | rv::filter(is_valid) |
               rv::transform(to_cheat);
    };

    auto cells = g.cells();
    auto counts = std::map<i64, i64>{};
    for (auto t : cells | rv::filter(is_valid) | rv::for_each(cheats) |
                      rv::transform(time_save)) {
        counts[t]++;
    }

    return counts;
}

i64 counts_gt(const auto& m, i64 threshold) {
    return reduce(ranges::subrange(m.lower_bound(threshold), m.end()) |
                  rv::values);
}

i64 run_a(std::string_view s) {
    const auto counts = solve(grid_t{s}, 2);
    return counts_gt(counts, 100);
}

auto run_b(std::string_view s) {
    const auto counts = solve(grid_t{s}, 20);
    return counts_gt(counts, 100);
}

TEST_CASE("day20a", "[day20]") {
    auto counts = solve(grid_t(test_data), 2);

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

TEST_CASE("day20b", "[day20]") {
    auto counts = solve(grid_t(test_data), 20);

    REQUIRE(counts[50] == 32);
    REQUIRE(counts[52] == 31);
    REQUIRE(counts[54] == 29);
    REQUIRE(counts[56] == 39);
    REQUIRE(counts[58] == 25);
    REQUIRE(counts[60] == 23);
    REQUIRE(counts[62] == 20);
    REQUIRE(counts[64] == 19);
    REQUIRE(counts[66] == 12);
    REQUIRE(counts[68] == 14);
    REQUIRE(counts[70] == 12);
    REQUIRE(counts[72] == 22);
    REQUIRE(counts[74] == 4);
    REQUIRE(counts[76] == 3);
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
