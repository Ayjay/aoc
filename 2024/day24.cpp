#include "aoc2024.h"

#include <map>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>

#include <boost/dynamic_bitset.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#include <range/v3/action.hpp>

#include <catch2/catch_test_macros.hpp>

#include "simple_timer.hpp"

namespace day24 {
i64 read_wires(boost::unordered::unordered_map<std::string, bool>&& wires);

namespace detail {
using namespace boost;
using Graph = adjacency_list<vecS,
                             vecS,
                             bidirectionalS,
                             property<vertex_name_t, std::string>>;
}  // namespace detail
using detail::Graph;

const auto name_parser = bp::repeat(3)[bp::lower | bp::digit];
const auto wire_parser = name_parser > ": " > bp::int_;

const auto gate_parser =
    name_parser > +bp::upper > name_parser > "->" > name_parser;
auto parse(std::string_view s) {
    auto it = s.begin();
    return std::tuple{*bp::prefix_parse(it, s.end(), *(wire_parser > bp::eol)),
                      *bp::prefix_parse(it, s.end(), *gate_parser, bp::ws)};
}

auto make_graph(std::string_view s) {
    auto g = Graph{};
    auto vertices =
        boost::unordered_map<std::string, Graph::vertex_descriptor>{};
    auto wires = boost::unordered_map<std::string, bool>{};
    auto gates = boost::unordered_map<std::string, std::string>{};

    const auto [parsed_wires, parsed_gates] = parse(s);
    for (const auto [wire, value] : parsed_wires)
        wires[wire] = value == 1;

    const auto get_vertex = [&](std::string s) {
        auto it = vertices.find(s);
        if (it == vertices.end())
            it = vertices.insert({s, add_vertex(s, g)}).first;
        return it->second;
    };
    for (const auto [input1, gate, input2, output] : parsed_gates) {
        auto output_vertex = get_vertex(output);
        boost::add_edge(get_vertex(input1), output_vertex, g);
        boost::add_edge(get_vertex(input2), output_vertex, g);
        gates[output] = gate;
    }
    return std::tuple{g, vertices, wires, gates};
}

constexpr bool debug_output = false;

std::vector<char> get_wire_values(
    const boost::unordered::unordered_map<std::string, bool>& wires,
    char prefix) {
    auto wire_values = std::vector<char>{};
    for (auto i : rv::iota(0)) {
        const auto wire_name = fmt::format("{}{:02}", prefix, i);
        const auto it = wires.find(wire_name);
        if (it == wires.end())
            break;
        wire_values.push_back(it->second);
    }
    return wire_values;
}

i64 read_wires(const boost::unordered::unordered_map<std::string, bool>& wires,
               char prefix) {
    const auto wire_values = get_wire_values(wires, prefix);
    auto bits = boost::dynamic_bitset{wire_values.size()};
    for (const auto [bit, val] : rv::enumerate(wire_values))
        bits.set(bit, val);
    return bits.to_ulong();
}

i64 run_a(std::string_view s) {
    auto [g, vertices, wires, gates] = make_graph(s);
    const auto get_name =
        [vertex_name = get(boost::vertex_name, g)](Graph::vertex_descriptor v) {
            return vertex_name[v];
        };
    if (debug_output)
        boost::write_graphviz(std::cout, g, [&](auto& os, auto v) {
            auto name = get_name(v);
            if (auto it = gates.find(name); it != gates.end())
                os << fmt::format("[label=\"{}\n{}\"]", it->second, name);
            else
                os << fmt::format("[label=\"{}\"]", name);
        });
    auto order = std::vector<Graph::vertex_descriptor>{};
    boost::topological_sort(g, std::back_inserter(order));
    order |= ranges::actions::reverse;
    if (debug_output)
        fmt::println("{}", order | rv::transform(get_name));

    for (const auto v : order) {
        auto name = get_name(v);
        if (const auto gate = gates.find(name); gate != gates.end()) {
            auto in_it = boost::in_edges(v, g).first;
            const auto input1 = wires.at(get_name(boost::source(*in_it++, g)));
            const auto input2 = wires.at(get_name(boost::source(*in_it++, g)));
            auto& output = wires[name];
            if (gate->second == "AND")
                output = input1 and input2;
            else if (gate->second == "XOR")
                output = input1 xor input2;
            else if (gate->second == "OR")
                output = input1 or input2;
            else
                assert(false);
        }
    }
    return read_wires(wires, 'z');
}

struct gate_t {
    std::string input1;
    std::string operation;
    std::string input2;
    std::string output;
};

std::string run_b(std::string_view s) {
    auto wires = boost::unordered_set<std::string>{};
    auto it = s.begin();
    bp::prefix_parse(
        it, s.end(),
        *(name_parser > ": " > (bp::lit('0') | bp::lit('1')) > bp::eol), wires);
    auto gates = std::vector<gate_t>{};
    bp::prefix_parse(it, s.end(), *gate_parser, bp::ws, gates);

    const auto width = [&] {
        for (auto i : rv::iota(0))
            if (not wires.contains(fmt::format("x{:02}", i)))
                return i;
        std::unreachable();
    }();

    auto carry_bit = std::optional<std::string>{};
    auto to_swap = std::vector<std::string>{};
    for (auto i : rv::iota(0, width)) {
        auto x = fmt::format("x{:02}", i);
        auto y = fmt::format("y{:02}", i);
        auto z = fmt::format("z{:02}", i);

        const auto make_input_matcher = [](auto in1, auto in2) {
            return [=](const gate_t& g) {
                return (g.input1 == in1 and g.input2 == in2) or
                       (g.input1 == in2 and g.input2 == in1);
            };
        };

        const auto make_pred = [](auto op, auto in1, auto in2) {
            return [=](const gate_t& g) {
                return g.operation == op and
                       ((g.input1 == in1 and g.input2 == in2) or
                        (g.input1 == in2 and g.input2 == in1));
            };
        };

        auto xor_gate = ranges::find_if(gates, make_pred("XOR", x, y));
        auto and_gate = ranges::find_if(gates, make_pred("AND", x, y));
        auto output_gate = ranges::find(gates, z, &gate_t::output);

        if (carry_bit) {
            if (output_gate->operation != "XOR") {
                // output gate is bad
                to_swap.push_back(z);
                auto swapped = ranges::find_if(
                    gates, make_pred("XOR", *carry_bit, xor_gate->output));
                assert(swapped != gates.end());
                to_swap.push_back(swapped->output);
                std::swap(output_gate->output, swapped->output);
                output_gate = swapped;
            } else if (not make_input_matcher(*carry_bit,
                                              xor_gate->output)(*output_gate)) {
                to_swap.push_back(xor_gate->output);
                to_swap.push_back(and_gate->output);
                std::swap(xor_gate->output, and_gate->output);
            }

            if (auto carry_and_gate = ranges::find_if(
                    gates, make_pred("AND", *carry_bit, xor_gate->output));
                carry_and_gate != gates.end()) {
                if (auto carry_or_gate = ranges::find_if(
                        gates, make_pred("OR", carry_and_gate->output,
                                         and_gate->output));
                    carry_or_gate != gates.end()) {
                    carry_bit = carry_or_gate->output;
                } else {
                    fmt::println("Can't find OR gate for [{}] and [{}]",
                                 carry_and_gate->output, and_gate->output);
                }
            } else {
                fmt::println("Can't find AND gate for [{}] and [{}]",
                             *carry_bit, xor_gate->output);
            }
        } else {
            carry_bit = and_gate->output;
        }
    }
    ranges::actions::sort(to_swap);
    return fmt::format("{}", fmt::join(to_swap, ","));
}

TEST_CASE("day24a", "[day24]") {
    const auto test_data_1 = R"(x00: 1
x01: 1
x02: 1
y00: 0
y01: 1
y02: 0

x00 AND y00 -> z00
x01 XOR y01 -> z01
x02 OR y02 -> z02)";

    const auto test_data_2 = R"(x00: 1
x01: 0
x02: 1
x03: 1
x04: 0
y00: 1
y01: 1
y02: 1
y03: 1
y04: 1

ntg XOR fgs -> mjb
y02 OR x01 -> tnw
kwq OR kpj -> z05
x00 OR x03 -> fst
tgd XOR rvg -> z01
vdt OR tnw -> bfw
bfw AND frj -> z10
ffh OR nrd -> bqk
y00 AND y03 -> djm
y03 OR y00 -> psh
bqk OR frj -> z08
tnw OR fst -> frj
gnj AND tgd -> z11
bfw XOR mjb -> z00
x03 OR x00 -> vdt
gnj AND wpb -> z02
x04 AND y00 -> kjc
djm OR pbm -> qhw
nrd AND vdt -> hwm
kjc AND fst -> rvg
y04 OR y02 -> fgs
y01 AND x02 -> pbm
ntg OR kjc -> kwq
psh XOR fgs -> tgd
qhw XOR tgd -> z09
pbm OR djm -> kpj
x03 XOR y03 -> ffh
x00 XOR y04 -> ntg
bfw OR bqk -> z06
nrd XOR fgs -> wpb
frj XOR qhw -> z04
bqk OR frj -> z07
y03 OR x01 -> nrd
hwm AND bqk -> z03
tgd XOR rvg -> z12
tnw OR pbm -> gnj)";

    CHECK(run_a(test_data_1) == 4);
    CHECK(run_a(test_data_2) == 2024);
}

}  // namespace day24

WEAK void entry() {
    using namespace day24;
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
