#include "aoc2024.h"

#include <array>
#include <exception>
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

const auto btn_to_char = [](dir_btn d) {
    if (auto v = std::get_if<vector2>(&d))
        return dir_to_arrow(*v);
    else
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

const auto number_positions = boost::unordered_map<char, vector2>{
    std::pair{'0', vector2{3, 1}}, std::pair{'1', vector2{2, 0}},
    std::pair{'2', vector2{2, 1}}, std::pair{'3', vector2{2, 2}},
    std::pair{'4', vector2{1, 0}}, std::pair{'5', vector2{1, 1}},
    std::pair{'6', vector2{1, 2}}, std::pair{'7', vector2{0, 0}},
    std::pair{'8', vector2{0, 1}}, std::pair{'9', vector2{0, 2}},
    std::pair{'A', vector2{3, 2}}};
const auto number_keypad = [](char c) { return number_positions.at(c); };

struct panic : std::exception {
    virtual const char* what() const noexcept override { return "Robot panic"; }
};
const auto get_second = [](const auto& kv) { return kv.second; };

std::vector<std::string> xxx;
void write_presses() {
    for (auto s : rv::reverse(xxx))
        std::cout << fmt::format("{}\n", s);
}

const auto make_numpad_presser = [](auto f) {
    auto index = xxx.size();
    xxx.push_back({});
    return [f, pos = number_keypad('A'), index](dir_btn b) mutable {
        xxx[index].push_back(btn_to_char(b));
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

static const auto dir_positions = boost::unordered_map<dir_btn, vector2>{
    std::pair{up, vector2{0, 1}}, std::pair{left, vector2{1, 0}},
    std::pair{down, vector2{1, 1}}, std::pair{right, vector2{1, 2}},
    std::pair{'A', vector2{0, 2}}};
const auto direction_keypad = [](dir_btn c) { return dir_positions.at(c); };

const auto make_dirpad_presser = [](auto f) {
    auto index = xxx.size();
    xxx.push_back({});
    return [f, pos = direction_keypad('A'), index](dir_btn b) mutable {
        xxx[index].push_back(btn_to_char(b));
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
        make_dirpad_presser([&](dir_btn b) { s.push_back(btn_to_char(b)); });
    robot('A');
    REQUIRE(s == "A");
}

auto positions_to_moves(auto positions, bool numpad) {
    const auto moves_to_nudges = [numpad](auto r) {
        auto it = r.begin();
        const auto [from_r, from_c] = *it++;
        const auto [to_r, to_c] = *it;
        const auto row_diff = to_r - from_r;
        const auto rows = rv::repeat(dir_btn{row_diff < 0 ? up : down}) |
                          rv::take(std::abs(row_diff));
        const auto col_diff = to_c - from_c;
        const auto cols = rv::repeat(dir_btn{col_diff < 0 ? left : right}) |
                          rv::take(std::abs(col_diff));
        const auto a = rv::single(dir_btn{'A'});

        if (numpad and (from_r == 3 and to_c == 0))
            return rv::concat(rows, cols, a);
        else if (not numpad and (from_r == 0 and to_c == 0))
            return rv::concat(rows, cols, a);
        else
            return rv::concat(cols, rows, a);
    };
    return positions | rv::sliding(2) | rv::transform(moves_to_nudges) |
           rv::join | ranges::to<std::vector>;
}

const auto sequence = [](auto s) {
    const auto level1_positions =
        rv::concat(rv::single('A'), s) | rv::transform(number_keypad);
    const auto level1_moves = positions_to_moves(level1_positions, true);
    fmt::println("1pos: {}", level1_positions);
    fmt::println("1mov: {}",
                 fmt::join(level1_moves | rv::transform(btn_to_char), ""));

    const auto level2_positions = rv::concat(rv::single('A'), level1_moves) |
                                  rv::transform(direction_keypad);
    const auto level2_moves = positions_to_moves(level2_positions, false);
    fmt::println("2pos: {}", level2_positions);
    fmt::println("2mov: {}",
                 fmt::join(level2_moves | rv::transform(btn_to_char), ""));

    const auto level3_positions = rv::concat(rv::single('A'), level2_moves) |
                                  rv::transform(direction_keypad);
    const auto level3_moves = positions_to_moves(level3_positions, false);
    fmt::println("3pos: {}", level3_positions);
    fmt::println("3mov: {}",
                 fmt::join(level3_moves | rv::transform(btn_to_char), ""));

    return level3_moves;
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
    for (auto i : inputs) {
        f(i);
        auto m = ranges::max(
            xxx | rv::transform([](const auto& v) { return v.size(); }));
        for (auto& v : xxx)
            if (v.size() < m)
                v.resize(m, ' ');
    }
    write_presses();
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

TEST_CASE("179A match", "[day21]") {
    auto seq =
        "<v<A>>^A<vA<A>>^AAvAA<^A>A<v<A>>^AAvA^A<vA>^AA<A>A<v<A>A>^AAAvA<^A>A"sv;
    auto mine = sequence("179A"sv);
    auto mine_s = mine | rv::transform(btn_to_char) | ranges::to<std::string>;
    CHECK(seq == mine_s);
}

TEST_CASE("aoc 456A", "[day21]") {
    auto seq = rv::transform(
        "<v<A>>^AA<vA<A>>^AAvAA<^A>A<vA>^A<A>A<vA>^A<A>A<v<A>A>^AAvA<^A>A"sv,
        char_to_btn);
    CHECK(actual_scenario(seq) == "456A");
}

TEST_CASE("inverse actual", "[day21]") {
    CHECK(actual_scenario(sequence("279A"sv)) == "279A");
    CHECK(actual_scenario(sequence("286A"sv)) == "286A");
    CHECK(actual_scenario(sequence("508A"sv)) == "508A");
    CHECK(actual_scenario(sequence("463A"sv)) == "463A");
    CHECK(actual_scenario(sequence("246A"sv)) == "246A");
}

TEST_CASE("inverse 029A", "[day21]") {
    CHECK(actual_scenario(sequence("029A"sv)) == "029A");
}

TEST_CASE("inverse 980A", "[day21]") {
    CHECK(actual_scenario(sequence("980A"sv)) == "980A");
}

TEST_CASE("inverse 179A", "[day21]") {
    CHECK(actual_scenario(sequence("179A"sv)) == "179A");
}

TEST_CASE("inverse 456A", "[day21]") {
    CHECK(actual_scenario(sequence("456A"sv)) == "456A");
}

TEST_CASE("inverse 379A", "[day21]") {
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
    CHECK(sequence_length("179A"sv) == expected_seq.length());
    write_presses();
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
