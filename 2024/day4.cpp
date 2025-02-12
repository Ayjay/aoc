#include "aoc2024.h"

#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <fmt/core.h>

#include <boost/hana/tuple.hpp>
#include <boost/unordered_map.hpp>

#include <boost/parser/search.hpp>

#include <boost/multi_array.hpp>

#include <catch2/catch_test_macros.hpp>

namespace day4 {
using result_type = long long;
const auto test_data =
    std::vector{std::tuple<std::string_view,
                           std::optional<result_type>,
                           std::optional<result_type>>{R"(MMMSXXMASM
MSAMXMSMSA
AMXSXMAAMM
MSAMASMSMX
XMASAMXAMM
XXAMMXXAMA
SMSMSASXSS
SAXAMASAAA
MAMMMXMMMM
MXMXAXMASX)",
                                                       18, 9}};

struct point {
    int row;
    int col;
};

using word = std::array<point, 4>;

auto shoot_word(point from, point direction) {
    return word{from,
                {from.row + 1 * direction.row, from.col + 1 * direction.col},
                {from.row + 2 * direction.row, from.col + 2 * direction.col},
                {from.row + 3 * direction.row, from.col + 3 * direction.col}};
}

const auto get_words = [](point from) {
    return std::array{
        shoot_word(from, {1, 0}),    // right
        shoot_word(from, {1, 1}),    // down-right
        shoot_word(from, {0, 1}),    // down
        shoot_word(from, {-1, 1}),   // down-left
        shoot_word(from, {-1, 0}),   // left
        shoot_word(from, {-1, -1}),  // up-left
        shoot_word(from, {0, -1}),   // up
        shoot_word(from, {1, -1})    // up-right
    };
};

auto run_a(std::string_view s) {
    const auto lines = get_lines(s);
    const auto rows = lines.size();
    const auto cols = lines.front().size();
    const auto matches_xmas = [&](word w) {
        using namespace std::string_view_literals;
        constexpr auto XMAS = "XMAS"sv;
        auto p = std::begin(XMAS);
        for (const auto [row, col] : w) {
            if (col < 0 or col >= cols or row < 0 or row >= rows ||
                lines[row][col] != *p++)
                return false;
        }
        return true;
    };
    auto count = result_type{};
    for (auto row = 0; row < rows; ++row) {
        for (auto col = 0; col < cols; ++col) {
            if (lines[row][col] == 'X') {
                count += ranges::count_if(get_words({row, col}), matches_xmas);
            }
        }
    }
    return count;
}

static auto run_b(std::string_view s) {
    const auto lines = get_lines(s);
    const auto rows = lines.size();
    const auto cols = lines.front().size();
    const auto matches_xmas = [&](point p) {
        auto [row, col] = p;
        auto top_left = lines[row - 1][col - 1];
        auto top_right = lines[row - 1][col + 1];
        auto bottom_right = lines[row + 1][col + 1];
        auto bottom_left = lines[row + 1][col - 1];
        auto all = std::array{top_left, top_right, bottom_right, bottom_left};
        if (ranges::any_of(all, [](char c) { return c != 'M' and c != 'S'; }))
            return false;
        return ((top_left == 'M' and bottom_right == 'S') or
                (top_left == 'S' and bottom_right == 'M')) and
               ((top_right == 'M' and bottom_left == 'S') or
                (top_right == 'S' and bottom_left == 'M'));
    };
    auto count = result_type{};
    for (auto row = 1; row < rows - 1; ++row) {
        for (auto col = 1; col < cols - 1; ++col) {
            if (lines[row][col] == 'A') {
                if (matches_xmas({row, col}))
                    ++count;
            }
        }
    }
    return count;
}

TEST_CASE("day4a", "[day4]") {
    const auto [s, expected, _] = test_data[0];
    if (expected) {
        REQUIRE(run_a(s) == *expected);
    }
}

TEST_CASE("day4b", "[day4]") {
    const auto [s, _, expected] = test_data[0];
    if (expected) {
        REQUIRE(run_b(s) == *expected);
    }
}

}  // namespace day4

WEAK void entry() {
    using namespace day4;
    const auto input = get_input(AOC_DAY);
    fmt::println("A: {}", run_a(input));
    fmt::println("B: {}", run_b(input));
}