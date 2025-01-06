#include "aoc2024.h"

#include <string_view>
#include <tuple>
#include <utility>
#include <vector>
#include <array>
#include <variant>
#include <iterator>

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#include <range/v3/view/join.hpp>

#include <catch2/catch_test_macros.hpp>

#include "grid.hpp"
#include "simple_timer.hpp"

namespace day21 {
using i64 = long long;
using namespace grid;

i64 manhattan(vector2 a, vector2 b) {
    const auto [a_r, a_c] = a;
    const auto [b_r, b_c] = b;
    return std::abs(b_r - a_r) + std::abs(b_c - a_c);
}

const auto number_keypad = [](char c) {
    static const auto positions = boost::unordered_map<char, vector2>{
        std::pair{'0', vector2{3,1}},
        std::pair{'1', vector2{2,0}},
        std::pair{'2', vector2{2,1}},
        std::pair{'3', vector2{2,2}},
        std::pair{'4', vector2{1,0}},
        std::pair{'5', vector2{1,1}},
        std::pair{'6', vector2{1,2}},
        std::pair{'7', vector2{0,0}},
        std::pair{'8', vector2{0,1}},
        std::pair{'9', vector2{0,2}},
        std::pair{'A', vector2{3,2}}
    };
    return positions.at(c);
};

const auto direction_keypad = [](std::variant<char,vector2> c) {
    static const auto positions = boost::unordered_map<std::variant<char,vector2>, vector2>{
        std::pair{up,    vector2{0,1}},
        std::pair{left,  vector2{1,0}},
        std::pair{down,  vector2{1,1}},
        std::pair{right, vector2{1,2}},
        std::pair{'A',   vector2{0,2}}
    };
    return positions.at(c);
};


auto foo(auto first, auto last, auto result, auto end_result, auto bop, auto proj) {
    if(first != last && result != end_result)
    {
        auto t1(ranges::invoke(proj, *first));
        *result = t1;
        for(++first, ++result; first != last && result != end_result;
            ++first, ++result)
        {
            auto t2(ranges::invoke(proj, *first));
            *result = ranges::invoke(bop, t2, t1);
            t1 = std::move(t2);
        }
    }
}

template <ranges::forward_range R>
auto adjacent_diff(R&& rng) {
    auto wrapped_range = rv::all(std::forward<R>(rng));
    //static_assert(ranges::viewable_range<decltype(wrapped_range)>);
    //static_assert(ranges::forward_range<decltype(wrapped_range)>);

    //auto r1 = rv::sliding(wrapped_range, 2);
    //static_assert(ranges::viewable_range<decltype(r1)>);
    //auto r =  rv::transform(r1, [](auto subrng) {
    //    auto it = subrng.begin();
    //    const auto a = *it++;
    //    const auto b = *it++;
    //    return b - a;
    //});
    //return r;
    return rng;
}

TEST_CASE("my_adjacent_diff", "[day21]") {
    auto v = std::vector{1,2,3};
    REQUIRE(ranges::equal(adjacent_diff(v), std::vector{1,1}));
}

TEST_CASE("my_adjacent_vector2_diff", "[day21]") {
    auto x = std::vector<vector2>{{0,0}, {1,1}, {2,2}};
    auto y = std::vector<vector2>{{1,1}, {1,1}};
    REQUIRE(ranges::equal(adjacent_diff(x), y));
}

const auto make_robot_moves = [](vector2 move) {
    const auto [rows,cols] = move;
    auto a = direction_keypad('A');
    return rv::concat(
        rv::single(a),
        rv::repeat(rows < 0 ? down : up) | rv::take(std::abs(rows)),
        rv::repeat(cols < 0 ? left : right) | rv::take(std::abs(cols)),
        rv::single(a)
    );
};

i64 get_sequence_length(std::string_view s) {
    auto pos = number_keypad('A');
    auto positions = rv::concat(rv::single('A'), s);
    auto moves = adjacent_diff(positions | rv::transform(number_keypad));
    fmt::println("moves: {}", moves);
    //auto robot_moves = adjacent_diff(moves | rv::transform(make_robot_moves) | rv::cache1 | rv::join);
    //fmt::println("robot_moves: {}", robot_moves);
    return -1;
}

i64 run_a(std::string_view s) {
    auto v = std::vector{1,2,3};
    auto foo = [](int i) { return std::vector{1,2,3}; };
    //auto bar = [](int i, int j) { return i + j; };
    //namespace sv = std::ranges::views;

    auto a1 = v | sv::transform(foo) | rv::cache1 | sv::join;

    auto i = a1.begin();
    auto b = i == i;

    //static_assert(ranges::forward_range<decltype(a1)>);
    //auto a2 = a1 | sv::join;
    //static_assert(sr::forward_range<decltype(a2)>);
    //auto a3 = a2 | sv::adjacent_transform<2>(bar);
    //static_assert(sr::forward_range<decltype(a3)>);
    //auto a = v | rv::transform(foo) | rv::sliding(2);
    return -1;
}

i64 run_b(std::string_view s) {
    return -1;
}

TEST_CASE("day21a", "[day21]") {
    using namespace std::string_view_literals;
    const auto test_data = std::array{
        std::tuple{"029A"sv, "<vA<AA>>^AvAA<^A>A<v<A>>^AvA^A<vA>^A<v<A>^A>AAvA^A<v<A>A>^AAAvA<^A>A"sv},
        std::tuple{"980A"sv, "<v<A>>^AAAvA^A<vA<AA>>^AvAA<^A>A<v<A>A>^AAAvA<^A>A<vA>^A<A>A"sv},
        std::tuple{"179A"sv, "<v<A>>^A<vA<A>>^AAvAA<^A>A<v<A>>^AAvA^A<vA>^AA<A>A<v<A>A>^AAAvA<^A>A"sv},
        std::tuple{"456A"sv, "<v<A>>^AA<vA<A>>^AAvAA<^A>A<vA>^A<A>A<vA>^A<A>A<v<A>A>^AAvA<^A>A"sv},
        std::tuple{"379A"sv, "<v<A>>^AvA^A<vA<AA>>^AAvA<^A>AAvA^A<vA>^AA<A>A<v<A>A>^AAAvA<^A>A"sv}};

    for (const auto [button,test_seq] : test_data) {
        REQUIRE(get_sequence_length(button) == test_seq.length());
    }
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
