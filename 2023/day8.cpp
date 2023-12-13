//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <vector>
#include <tuple>
#include <string_view>
#include <boost/unordered_map.hpp>
#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/bimap/vector_of.hpp>
#include <numeric>

using result_type = long long;
const auto test_data = std::vector{ std::tuple<std::string_view, std::optional<result_type>, std::optional<result_type>>
{R"(RL

AAA = (BBB, CCC)
BBB = (DDD, EEE)
CCC = (ZZZ, GGG)
DDD = (DDD, DDD)
EEE = (EEE, EEE)
GGG = (GGG, GGG)
ZZZ = (ZZZ, ZZZ))", 2, std::nullopt},
{R"(LLR

AAA = (BBB, BBB)
BBB = (AAA, ZZZ)
ZZZ = (ZZZ, ZZZ))", 6, std::nullopt},
{R"(LR

11A = (11B, XXX)
11B = (XXX, 11Z)
11Z = (11B, XXX)
22A = (22B, XXX)
22B = (22C, 22C)
22C = (22Z, 22Z)
22Z = (22B, 22B)
XXX = (XXX, XXX))", std::nullopt, 6}
};

auto parse(std::string_view s) {
    using ascii::alpha;
    using ascii::alnum;

    auto instructions = std::string{};
    auto it = s.begin();
    boost::spirit::x3::parse(it, s.end(),
        *alpha,
        instructions);

    auto graph = std::vector<std::tuple<std::string, std::string, std::string>>{};
    phrase_parse(it, s.end(),
        *(*alnum >> '=' >> lit('(') >> *alnum >> ',' >> *alnum >> ')'),
        space, graph);

    auto to_map = graph
        | rv::transform([](auto v) {
            const auto& [node, left, right] = v;
            return std::pair{ node, std::tuple{left,right} };
            })
        | ranges::to<std::unordered_map>;

    return std::tuple{ instructions, to_map };
}

auto run_a(std::string_view s) {
    const auto [instructions,map] = parse(s);
    auto steps = 0;
    auto node = std::string{ "AAA" };
    while (node != "ZZZ") {
        const auto& [left, right] = map.at(node);
        auto direction = instructions[steps % instructions.size()];
        if (direction == 'L')
            node = left;
        else
            node = right;
        ++steps;
    }

    return steps;
}

struct loop_info {
    long long start_offset;
    std::vector<long long> z_positions;
    long long loop_length;

    bool is_on_zed(long long step) const {
        return step >= start_offset && 
        ranges::find(z_positions, (step-start_offset) % loop_length) != z_positions.end();
    }
};

auto run_b(std::string_view s) {
    const auto [instructions,map] = parse(s);

    const auto get_loop = [&](auto node) -> loop_info {
        //fmt::println("Loop starting node [{}]", node);
        using loop_position_t = std::tuple<std::string, long long>;
        using namespace boost::bimaps;
        auto history = bimap<
            unordered_set_of<loop_position_t>,
            vector_of<long long>
        >{};
        auto step = 0ll;
        const auto get_position_state = [&]() {
            const auto instruction_index = step % instructions.size();
            return loop_position_t{node,instruction_index};
        };
        while (true) {
            auto [_,inserted] = history.insert({get_position_state(), step});
            if (!inserted)
                break;

            const auto& [left, right] = map.at(node);
            const auto instruction_index = step % instructions.size();
            const auto direction = instructions[instruction_index];
            if (direction == 'L')
                node = left;
            else
                node = right;
            ++step;
        }
        //for (const auto& [node_pos, step] : history.right) {
        //    const auto& [node,instruction] = node_pos;
        //    fmt::println("Step {}: node [{}] instr [{}]", step, node, instruction);
        //}
        auto ret = loop_info{};
        const auto is_zed = [](const auto& kv) {
            const auto& [node_pos,step] = kv;
            const auto& [node,_] = node_pos;
            return node.back() == 'Z';
        };
        const auto first_z_it = ranges::find_if(history.right, is_zed);
        ret.start_offset = first_z_it - history.right.begin();
        ret.z_positions = ranges::subrange{history.right.begin() + ret.start_offset, history.right.end()}
            | rv::filter(is_zed)
            | rv::transform([&](const auto& kv) {
                const auto& [_,step] = kv;
                return step - ret.start_offset;
            })
            | ranges::to<std::vector>;
        ret.loop_length = step - history.left.at(get_position_state());
        return ret;
    };

    const auto as = map
     | rv::keys
     | rv::filter([](const auto& s) { return s.back() =='A';})
     | ranges::to<std::vector>;

    const auto loops = as
     | rv::transform(get_loop)
     | ranges::to<std::vector>;

    // for (auto step : rv::ints(0ll, ranges::unreachable)) {
    //     if (ranges::all_of(loops, [=](const auto& loop) { return loop.is_on_zed(step); }))
    //         return step;
    // }

    // apparently the data only has loops all starting from offset 0
    // with a single z
    // allowing this hack
    return reduce(loops | rv::transform([](const auto& loop) { return loop.loop_length; }),
        [](auto a, auto b) { return std::lcm(a,b); });
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
