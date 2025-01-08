#include "aoc2024.h"

#include <array>
#include <iterator>
#include <string_view>
#include <tuple>
#include <utility>
#include <variant>
#include <vector>

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <fmt/std.h>

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#include <range/v3/view/join.hpp>

#include <catch2/catch_test_macros.hpp>

#include "grid.hpp"
#include "simple_timer.hpp"

namespace day21 {
using i64 = long long;
using namespace grid;

using dir_btn = std::variant<char, vector2>;

const auto number_positions = boost::unordered_map<char, vector2>{
    std::pair{'0', vector2{3, 1}}, std::pair{'1', vector2{2, 0}},
    std::pair{'2', vector2{2, 1}}, std::pair{'3', vector2{2, 2}},
    std::pair{'4', vector2{1, 0}}, std::pair{'5', vector2{1, 1}},
    std::pair{'6', vector2{1, 2}}, std::pair{'7', vector2{0, 0}},
    std::pair{'8', vector2{0, 1}}, std::pair{'9', vector2{0, 2}},
    std::pair{'A', vector2{3, 2}}};
const auto number_keypad = [](char c) { return number_positions.at(c); };

struct panic {};
const auto get_second = [](const auto& kv) { return kv.second; };

const auto make_numpad_presser = [](auto f) {
    return [f, pos = number_keypad('A')](dir_btn b) mutable {
        if (auto p = std::get_if<vector2>(&b)) {
            pos += *p;
            if (not ranges::contains(number_positions, pos, get_second))
                throw panic{};
        } else {
            auto it = ranges::find(number_positions, pos, get_second);
            assert(it != number_positions.end());
            f(it->first);
        }
    };
};

TEST_CASE("numpad simulator", "[day21]") {
    auto s = std::string{};
    auto robot = make_numpad_presser([&](char c) { s.push_back(c); });
    robot('A');
    REQUIRE(s == "A");
    robot(left);
    robot('A');
    REQUIRE(s == "A0");
    robot(up);
    robot('A');
    REQUIRE(s == "A02");
}

const auto to_char = [](dir_btn d) {
    if (auto v = std::get_if<vector2>(&d)) {
        if (*v == up)
            return '^';
        if (*v == down)
            return 'v';
        if (*v == left)
            return '<';
        if (*v == right)
            return '>';
    }
    return 'A';
};

const auto char_to_btn = [](char c) -> dir_btn {
    switch (c) {
        case '<':
            return left;
        case '^':
            return up;
        case '>':
            return right;
        case 'v':
            return down;
        case 'A':
            return 'A';
    }
    assert(false);
    std::unreachable();
};

static const auto dir_positions = boost::unordered_map<dir_btn, vector2>{
    std::pair{up, vector2{0, 1}}, std::pair{left, vector2{1, 0}},
    std::pair{down, vector2{1, 1}}, std::pair{right, vector2{1, 2}},
    std::pair{'A', vector2{0, 2}}};
const auto direction_keypad = [](dir_btn c) { return dir_positions.at(c); };

const auto make_dirpad_presser = [](auto f) {
    return [f, pos = direction_keypad('A')](dir_btn b) mutable {
        if (auto p = std::get_if<vector2>(&b)) {
            pos += *p;
            if (not ranges::contains(dir_positions, pos, get_second))
                throw panic{};
        } else {
            auto it = ranges::find(dir_positions, pos, get_second);
            assert(it != dir_positions.end());
            f(it->first);
        }
    };
};

TEST_CASE("dirpad simulator", "[day21]") {
    auto s = std::string{};
    auto robot =
        make_dirpad_presser([&](dir_btn b) { s.push_back(to_char(b)); });
    robot('A');
    REQUIRE(s == "A");
}

const auto moves_to_nudges = [](vector2 move) {
    const auto [rows, cols] = move;
    return rv::concat(
        rv::repeat(dir_btn{cols < 0 ? left : right}) | rv::take(std::abs(cols)),
        rv::repeat(dir_btn{rows < 0 ? up : down}) | rv::take(std::abs(rows)),
        rv::single(dir_btn{'A'}));
};

std::vector<vector2> positions_to_moves(auto positions) {
    auto moves = std::vector<vector2>{};
    ranges::adjacent_difference(positions, std::back_inserter(moves),
                                [](vector2 a, vector2 b) { return a - b; });
    moves.erase(moves.begin());
    return moves;
}

const auto next_level = [](auto r) {
    const auto moves = positions_to_moves(r);
    const auto nudges =
        moves | rv::for_each(moves_to_nudges) | ranges::to<std::vector>;
    return nudges;
};

const auto sequence = [](auto s) {
    const auto level1_positions =
        rv::concat(rv::single('A'), s) | rv::transform(number_keypad);
    const auto level1_nudges = next_level(level1_positions);
    fmt::println("1pos: {}", level1_positions);
    fmt::println("1nud: {}",
                 fmt::join(level1_nudges | rv::transform(to_char), ""));

    const auto level2_positions = rv::concat(rv::single('A'), level1_nudges) |
                                  rv::transform(direction_keypad);
    const auto level2_nudges = next_level(level2_positions);
    fmt::println("2pos: {}", level2_positions);
    fmt::println("2nud: {}",
                 fmt::join(level2_nudges | rv::transform(to_char), ""));

    const auto level3_positions = rv::concat(rv::single('A'), level2_nudges) |
                                  rv::transform(direction_keypad);
    const auto level3_nudges = next_level(level3_positions);
    fmt::println("3pos: {}", level3_positions);
    fmt::println("3nud: {}",
                 fmt::join(level3_nudges | rv::transform(to_char), ""));

    return level3_nudges;
};

const auto sequence_length = [](auto s) {
    return ranges::distance(sequence(s));
};

const auto complexity = [](auto s) {
    return *bp::parse(s, bp::long_long >> 'A') * sequence_length(s);
};

i64 run_a(std::string_view s) {
    return reduce(s | rv::split('\n') | rv::transform(complexity));
}

i64 run_b(std::string_view s) {
    return -1;
}

std::string actual_scenario(auto inputs) {
    auto s = std::string{};
    auto f = make_dirpad_presser(make_dirpad_presser(
        make_numpad_presser([&](char c) { s.push_back(c); })));
    for (auto i : inputs)
        f(i);
    return s;
}

TEST_CASE("aoc 029A", "[day21]") {
    auto seq = rv::transform(
        "<vA<AA>>^AvAA<^A>A<v<A>>^AvA^A<vA>^A<v<A>^A>AAvA^A<v<A>A>^AAAvA<^A>A"sv,
        char_to_btn);
    CHECK(actual_scenario(seq) == "029A");
}

TEST_CASE("aoc 179A", "[day21]") {
    auto seq = rv::transform(
        "<v<A>>^A<vA<A>>^AAvAA<^A>A<v<A>>^AAvA^A<vA>^AA<A>A<v<A>A>^AAAvA<^A>A"sv,
        char_to_btn);
    CHECK(actual_scenario(seq) == "179A");
}

TEST_CASE("aoc 456A", "[day21]") {
    auto seq = rv::transform(
        "<v<A>>^AA<vA<A>>^AAvAA<^A>A<vA>^A<A>A<vA>^A<A>A<v<A>A>^AAvA<^A>A"sv,
        char_to_btn);
    CHECK(actual_scenario(seq) == "456A");
}

TEST_CASE("inverses", "[day21]") {
    CHECK(actual_scenario(sequence("029A"sv)) == "029A");
    CHECK(actual_scenario(sequence("980A"sv)) == "980A");
    CHECK(actual_scenario(sequence("179A"sv)) == "179A");
    CHECK(actual_scenario(sequence("456A"sv)) == "456A");
    CHECK(actual_scenario(sequence("379A"sv)) == "379A");
}

TEST_CASE("029A", "[day21]") {
    using namespace std::string_view_literals;
    const auto expected_seq =
        "<vA<AA>>^AvAA<^A>A<v<A>>^AvA^A<vA>^A<v<A>^A>AAvA^A<v<A>A>^AAAvA<^A>A"sv;
    REQUIRE(sequence_length("029A"sv) == expected_seq.length());
}

TEST_CASE("980A", "[day21]") {
    using namespace std::string_view_literals;
    const auto expected_seq =
        "<v<A>>^AAAvA^A<vA<AA>>^AvAA<^A>A<v<A>A>^AAAvA<^A>A<vA>^A<A>A"sv;
    REQUIRE(sequence_length("980A"sv) == expected_seq.length());
}

TEST_CASE("179A", "[day21]") {
    using namespace std::string_view_literals;
    const auto expected_seq =
        "<v<A>>^A<vA<A>>^AAvAA<^A>A<v<A>>^AAvA^A<vA>^AA<A>A<v<A>A>^AAAvA<^A>A"sv;
    REQUIRE(sequence_length("179A"sv) == expected_seq.length());
}

TEST_CASE("456A", "[day21]") {
    using namespace std::string_view_literals;
    const auto expected_seq =
        "<v<A>>^AA<vA<A>>^AAvAA<^A>A<vA>^A<A>A<vA>^A<A>A<v<A>A>^AAvA<^A>A"sv;
    REQUIRE(sequence_length("456A"sv) == expected_seq.length());
}

TEST_CASE("379A", "[day21]") {
    using namespace std::string_view_literals;
    const auto expected_seq =
        "<v<A>>^AvA^A<vA<AA>>^AAvA<^A>AAvA^A<vA>^AA<A>A<v<A>A>^AAAvA<^A>A"sv;
    REQUIRE(sequence_length("379A"sv) == expected_seq.length());
}

TEST_CASE("day21a", "[day21]") {
    const auto test_data =
        R"(029A
980A
179A
456A
379A)";

    CHECK(run_a(test_data) == 126384);
}

// TEST_CASE("day21b", "[day21]") {
// }

}  // namespace day21

WEAK void entry() {
    using namespace day21;
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
