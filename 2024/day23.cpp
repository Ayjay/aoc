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
#include <boost/graph/connected_components.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#include <range/v3/action.hpp>

#include <catch2/catch_test_macros.hpp>

#include "simple_timer.hpp"

namespace day23 {

namespace detail {
using namespace boost;
using Graph = adjacency_list<vecS,
                             vecS,
                             undirectedS,

                             property<vertex_name_t, std::string>>;
}  // namespace detail
using detail::Graph;

auto make_graph(std::string_view s) {
    auto g = Graph{};
    auto vertices =
        boost::unordered_map<std::string, Graph::vertex_descriptor>{};
    const auto get_vertex = [&](std::string s) {
        auto it = vertices.find(s);
        if (it == vertices.end())
            it = vertices.insert({s, add_vertex(s, g)}).first;
        return it->second;
    };
    auto raw = *bp::parse(s, *(bp::repeat(2)[bp::lower] > '-' >
                               bp::repeat(2)[bp::lower] > -bp::eol));
    for (auto [first, second] : raw) {
        boost::add_edge(get_vertex(first), get_vertex(second), g);
    }
    return std::tuple{g, vertices};
}

i64 run_a(std::string_view s) {
    const auto [g, vertices] = make_graph(s);
    auto triplets =
        boost::unordered_set<boost::unordered_set<Graph::vertex_descriptor>>{};
    for (auto [v_it, v_end] = boost::vertices(g); v_it != v_end; ++v_it) {
        auto [a_it, a_end] = adjacent_vertices(*v_it, g);
        for (; a_it != a_end; ++a_it) {
            for (auto b_it = std::next(a_it); b_it != a_end; ++b_it) {
                auto [b_adj, b_adj_end] = adjacent_vertices(*b_it, g);
                if (ranges::contains(ranges::subrange(b_adj, b_adj_end),
                                     *a_it)) {
                    triplets.insert({*v_it, *a_it, *b_it});
                }
            }
        }
    }

    const auto has_t =
        [&g](const boost::unordered_set<Graph::vertex_descriptor>& triplet) {
            const auto starts_with_t = [&g](const auto& s) {
                auto vertex_name = get(boost::vertex_name, g);
                return vertex_name[s].front() == 't';
            };
            return ranges::any_of(triplet, starts_with_t);
        };
    return ranges::count_if(triplets, has_t);
}

std::string run_b(std::string_view s) {
    const auto [g, vertices] = make_graph(s);
    auto networks =
        boost::unordered_set<boost::unordered_set<Graph::vertex_descriptor>>{};

    const auto edge_exists = [&](auto v, auto u) {
        auto [adj, adj_end] = adjacent_vertices(v, g);
        return ranges::contains(ranges::subrange(adj, adj_end), u);
    };

    for (auto [v_it, v_end] = boost::vertices(g); v_it != v_end; ++v_it) {
        auto [a_it, a_end] = adjacent_vertices(*v_it, g);
        auto adjacent =
            boost::unordered_set<Graph::vertex_descriptor>{a_it, a_end};
        const auto create_network =
            [&](auto a_v) -> boost::unordered_set<Graph::vertex_descriptor> {
            auto network = boost::unordered_set<Graph::vertex_descriptor>{};
            network.insert(*v_it);
            network.insert(a_v);
            for (auto u : adjacent) {
                if (u == a_v)
                    continue;
                if (ranges::all_of(network,
                                   [&](auto w) { return edge_exists(u, w); })) {
                    network.insert(u);
                }
            }
            return network;
        };
        for (auto [a_it, a_end] = adjacent_vertices(*v_it, g); a_it != a_end;
             ++a_it) {
            networks.insert(create_network(*v_it));
        }
    }

    const auto get_name = [vertex_name = get(boost::vertex_name, g)](auto v) {
        return vertex_name[v];
    };

    auto best = *ranges::max_element(networks, std::less{},
                                     [](const auto& c) { return c.size(); }) |
                rv::transform(get_name) | ranges::to<std::vector> |
                ranges::actions::sort;
    return fmt::format("{}", fmt::join(best, ","));
}

const auto test_data = R"(kh-tc
qp-kh
de-cg
ka-co
yn-aq
qp-ub
cg-tb
vc-aq
tb-ka
wh-tc
yn-cg
kh-ub
ta-co
de-co
tc-td
tb-wq
wh-td
ta-ka
td-qp
aq-cg
wq-ub
ub-vc
de-ta
wq-aq
wq-vc
wh-yn
ka-de
kh-ta
co-tc
wh-qp
tb-vc
td-yn)";

TEST_CASE("day23a", "[day23]") {
    REQUIRE(run_a(test_data) == 7);
}

TEST_CASE("day23b", "[day23]") {
    REQUIRE(run_b(test_data) == "co,de,ka,ta");
}
}  // namespace day23

WEAK void entry() {
    using namespace day23;
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
