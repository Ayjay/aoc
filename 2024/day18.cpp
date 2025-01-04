#include "aoc2024.h"

#include <cmath>
#include <iterator>
#include <queue>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>

#include <boost/container_hash/hash.hpp>
#include <boost/describe.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#include <range/v3/algorithm/min.hpp>

#include <catch2/catch_test_macros.hpp>

#include "grid.hpp"
#include "simple_timer.hpp"

using namespace grid;

namespace day18 {
using i64 = long long;
const auto test_data = std::vector{std::tuple<std::string_view,
                                              std::optional<i64>,
                                              std::optional<std::string_view>>{
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
    22, "6,1"}};

struct memory {
    i64 rows;
    i64 cols;
    boost::unordered_set<vector2> corrupted;
    bool valid(vector2 pos) const {
        const auto [row, col] = pos;
        return row >= 0 and row < rows and col >= 0 and col < cols and
               not corrupted.contains(pos);
    }

    std::optional<i64> path_length() const;
};

struct edge {
    vector2 start;
    vector2 direction;
    auto operator<=>(const edge&) const = default;
    friend std::ostream& operator<<(std::ostream& os, const edge& e) {
        return os << fmt::format("[{}] {}", e.start,
                                 grid::dir_to_string(e.direction));
    }
};
BOOST_DESCRIBE_STRUCT(edge, (), (start, direction));
}  // namespace day18

template <>
struct fmt::formatter<day18::edge> : ostream_formatter {};

namespace day18 {
vector2 source(edge e, const memory& g) {
    return e.start;
}

vector2 target(edge e, const memory& g) {
    return e.start + e.direction;
}

struct edge_iterator
    : public boost::iterator_adaptor<edge_iterator,
                                     decltype(directions.begin()),
                                     edge,
                                     boost::forward_traversal_tag,
                                     edge> {
    const memory* g = nullptr;
    vector2 start = {0, 0};

    edge_iterator(const memory& g, vector2 start, base_type it)
        : g(&g), start(start), iterator_adaptor_(it) {
        forward();
    }

    static auto range(const memory& g, vector2 start) {
        return std::pair{edge_iterator{g, start, directions.begin()},
                         edge_iterator{g, start, directions.end()}};
    }

    edge dereference() const { return {start, *base()}; }
    bool equal(const edge_iterator& other) const {
        assert(g == other.g);
        assert(start == other.start);
        return base() == other.base();
    }
    void increment() {
        ++base_reference();
        forward();
    }

   private:
    void forward() {
        for (; base_reference() != directions.end() and
               not g->valid(start + *base_reference());
             ++base_reference())
            ;
    }
};

auto out_edges(vector2 u, const memory& g) {
    return edge_iterator::range(g, u);
}

auto out_degree(vector2 u, const memory& g) {
    auto edges = out_edges(u, g);
    return std::distance(edges.first, edges.second);
}

}  // namespace day18

namespace boost {
template <>
struct graph_traits<day18::memory> {
    using vertex_descriptor = grid::vector2;
    using edge_descriptor = day18::edge;
    using directed_category = undirected_tag;
    using edge_parallel_category = disallow_parallel_edge_tag;
    using traversal_category = incidence_graph_tag;
    using out_edge_iterator =
        decltype(day18::out_edges(std::declval<vector2>(),
                                  std::declval<day18::memory>())
                     .first);
    using degree_size_type =
        decltype(day18::out_degree(std::declval<vector2>(),
                                   std::declval<day18::memory>()));

    static_assert(std::input_iterator<out_edge_iterator>);

    static vertex_descriptor null_vertex() { return {0, 0}; }
};
}  // namespace boost

BOOST_CONCEPT_ASSERT((boost::concepts::Graph<day18::memory>));
BOOST_CONCEPT_ASSERT((boost::concepts::IncidenceGraph<day18::memory>));

namespace day18 {

const auto byte_parser = *(bp::long_long > ',' > bp::long_long > -bp::eol);

std::optional<i64> memory::path_length() const {
    auto q = std::queue<vector2>{};
    const auto start = vector2{i64{}, i64{}};
    const auto end = vector2{rows - 1, cols - 1};
    q.push(start);
    auto predecessors = boost::unordered_map<vector2, vector2>{};
    while (!q.empty()) {
        const auto v = q.front();
        assert(valid(v));
        q.pop();

        for (auto [edge_it, edge_end] = out_edges(v, *this);
             edge_it != edge_end; ++edge_it) {
            const auto e = *edge_it;
            const auto w = target(e, *this);
            assert(valid(w));
            if (predecessors.insert({w, v}).second)
                q.push(w);
        }
    }

    if (not predecessors.contains(end))
        return {};

    auto dist = 0ll;
    for (auto p = end; p != start; p = predecessors.at(p))
        ++dist;
    return dist;
}

i64 run_a(std::string_view s, i64 rows, i64 cols, i64 falling_bytes) {
    auto m = memory{rows, cols};
    auto it = s.begin();
    bp::prefix_parse(
        it, s.end(),
        bp::repeat(
            falling_bytes)[bp::long_long > ',' > bp::long_long > -bp::eol],
        m.corrupted);
    return *m.path_length();
}

auto run_b(std::string_view s, i64 rows, i64 cols) {
    auto m = memory{rows, cols};
    auto falling_bytes = std::vector<vector2>{};
    bp::parse(s, *(bp::long_long > ',' > bp::long_long > -bp::eol),
              falling_bytes);
    for (auto i : rv::iota(1)) {
        auto i_bytes = falling_bytes | rv::take(i);
        m.corrupted.clear();
        m.corrupted.insert(i_bytes.begin(), i_bytes.end());
        if (not m.path_length()) {
            const auto [r, c] = falling_bytes[i - 1];
            return fmt::format("{},{}", r, c);
        }
    }
    std::unreachable();
}

TEST_CASE("day18a", "[day18]") {
    for (const auto& test : test_data) {
        const auto [s, expected, _] = test;
        if (expected) {
            REQUIRE(run_a(s, 7, 7, 12) == *expected);
        }
    }
}

TEST_CASE("day18b", "[day18]") {
    for (const auto& test : test_data) {
        const auto [s, _, expected] = test;
        if (expected) {
            REQUIRE(run_b(s, 7, 7) == *expected);
        }
    }
}

}  // namespace day18

WEAK void entry() {
    using namespace day18;
    const auto input = get_input(AOC_DAY);
    {
        auto t = SimpleTimer("Part A");
        fmt::println("A: {}", run_a(input, 71, 71, 1024));
    }
    {
        auto t = SimpleTimer("Part B");
        fmt::println("B: {}", run_b(input, 71, 71));
    }
}
