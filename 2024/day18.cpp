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

namespace day18 {
using i64 = long long;
const auto test_data = std::vector{
    std::tuple<std::string_view, std::optional<i64>, std::optional<i64>>{
        R"(5,4
4,2
4,5
3,0
2,1
6,3
2,4
1,5
0,6
3,3
2,6
5,1
1,2
5,5
2,5
6,5
1,4
0,4
6,4
1,1
6,1
1,0
0,5
1,6
2,0)",
        22, {}}
    };

using namespace grid;

struct edge {
    vector2 start;
    vector2 direction;
    auto operator<=>(const edge&) const = default;
    friend std::ostream& operator<<(std::ostream& os, const edge& e) {
        return os << fmt::format("[{}] {}", e.start,
                                 dir_to_string(e.direction));
    }
};
BOOST_DESCRIBE_STRUCT(edge, (), (start, direction));
}  // namespace day18

template <>
struct fmt::formatter<day18::edge> : ostream_formatter {};

namespace day18 {
vector2 source(edge e, const grid_t& g) {
    return e.start;
}

vector2 target(edge e, const grid_t& g) {
    return {e.start.pos + e.direction, e.direction};
}

struct edge_iterator
    : public boost::iterator_adaptor<edge_iterator,
                                    decltype(directions.begin()),
                                    boost::forward_traversal_tag,
                                    edge> {
    const grid_t* g = nullptr;
    vector2 start = {0, 0};

    edge_iterator(const grid_t& g, vector2 start, auto it) : g(&g), start(start), iterator_adaptor_(it) {
        forward();
    }

    static auto range(const grid_t& g, vector2 start) {
        return std::pair{
            edge_iterator{&g, start, directions.begin()},
            edge_iterator{&g, start, directions.end()}
        };
    }

    edge dereference() const { return start + *base(); }
    bool equal(const edge_iterator& other) const {
        return base() == other.base();
    }
    void increment() {
        ++base();
        forward();
    }

   private:
    void forward() {
        for (; base() != directions.end() and g->get(*start.pos + stage_to_dir()) == '#';
             ++stage)
            ;
    }
};

auto out_edges(vector2 u, const grid_t& g) {
    return std::pair{edge_iterator{g, u}, edge_iterator{}};
}

auto out_degree(vector2 u, const grid_t& g) {
    auto edges = out_edges(u, g);
    return std::distance(edges.first, edges.second);
}

}  // namespace day18

namespace boost {
template <>
struct graph_traits<grid::grid_t> {
    using vertex_descriptor = grid_t::vector2;
    using edge_descriptor = day18::edge;
    using directed_category = undirected_tag;
    using edge_parallel_category = disallow_parallel_edge_tag;
    using traversal_category = incidence_graph_tag;
    using out_edge_iterator =
        decltype(day18::out_edges(std::declval<day18::vector2>(),
                                  std::declval<grid::grid_t>())
                     .first);
    using degree_size_type =
        decltype(day18::out_degree(std::declval<day18::vector2>(),
                                   std::declval<grid::grid_t>()));

    static_assert(std::input_iterator<out_edge_iterator>);

    static vertex_descriptor null_vertex() { return {{0, 0}, {0, 0}}; }
};
}  // namespace boost

BOOST_CONCEPT_ASSERT((boost::concepts::Graph<grid::grid_t>));
BOOST_CONCEPT_ASSERT((boost::concepts::IncidenceGraph<grid::grid_t>));

namespace day18 {

i64 run_a(std::string_view s) {
    return -1;
}

auto run_b(std::string_view s) {
    return -1;
}

TEST_CASE("day18a", "[day18]") {
    for (const auto& test : test_data) {
        const auto [s, expected, _] = test;
        if (expected) {
            REQUIRE(run_a(s) == *expected);
        }
    }
}

TEST_CASE("day18b", "[day18]") {
    for (const auto& test : test_data) {
        const auto [s, _, expected] = test;
        if (expected) {
            REQUIRE(run_b(s) == *expected);
        }
    }
}

}  // namespace day18

WEAK void entry() {
    using namespace day18;
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