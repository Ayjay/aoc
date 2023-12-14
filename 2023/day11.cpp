//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <vector>
#include <tuple>
#include <string_view>

#include <unordered_set>
#include <set>
#include <boost/container_hash/hash.hpp>

#include <range/v3/action.hpp>

using result_type = long long;
const auto test_data = std::vector{ std::tuple<std::string_view, std::optional<result_type>, std::optional<result_type>>
{R"(...#......
.......#..
#.........
..........
......#...
.#........
.........#
..........
.......#..
#...#.....)", 374, {}}
};

using bounds = std::tuple<long long, long long>;
using point_t = std::tuple<long long, long long>;

const auto get_row = [](auto star) {
    const auto [row, col] = star;
    return row;
};

const auto get_col = [](auto star) {
    const auto [row, col] = star;
    return col;
};

auto parse_grid(std::string_view s) {
    auto stars = std::unordered_set<point_t, boost::hash<point_t>>{};
    const auto lines = get_lines(s);
    for (const auto [row,line] : rv::enumerate(lines)) {
        for (const auto [col,c] : rv::enumerate(line)) {
            if (c == '#')
                stars.insert({ row,col });
        }
    }
    return std::tuple{ bounds { lines.size(), lines.front().size() }, stars };
}

auto calculate_star_distance(std::string_view s, auto empty_space) {
    const auto [grid_size, stars] = parse_grid(s);
    const auto [rows, cols] = grid_size;
    const auto empty_rows = rv::iota(0, rows)
        | rv::filter([&](auto row) {
            return ranges::find(stars, row, get_row) == stars.end();
            })
        | ranges::to<std::set>;

    const auto empty_cols = rv::iota(0, cols)
        | rv::filter([&](auto col) {
            return ranges::find(stars, col, get_col) == stars.end();
            })
        | ranges::to<std::set>;

    const auto star_distance = [&](auto stars) {
        const auto [a,b] = stars;
        const auto [a_row, a_col] = a;
        const auto [b_row, b_col] = b;
        const auto interposing_rows = std::distance(
            empty_rows.upper_bound(std::min(a_row,b_row)), empty_rows.lower_bound(std::max(a_row,b_row))
        );
        const auto interposing_cols = std::distance(
            empty_cols.upper_bound(std::min(a_col,b_col)), empty_cols.lower_bound(std::max(a_col,b_col))
        );
        const auto row_diff = std::abs(a_row - b_row);
        const auto col_diff = std::abs(a_col - b_col);
        return (row_diff - interposing_rows) + (interposing_rows * empty_space) +
               (col_diff - interposing_cols) + (interposing_cols * empty_space);
    };

    const auto not_same = [](auto stars) {
        const auto [a, b] = stars;
        return a != b;
    };

    return reduce(
        rv::cartesian_product(stars, stars)
      | rv::filter(not_same)
      | rv::transform(star_distance)) / 2;
}

auto run_a(std::string_view s) {
    return calculate_star_distance(s, 2);
}

auto run_b(std::string_view s) {
    return calculate_star_distance(s, 1000000);
}

int main() {
    const auto first_test = std::get<0>(test_data.front());
    fmt::println("Distance 10: {}", calculate_star_distance(first_test, 10));
    fmt::println("Distance 100: {}", calculate_star_distance(first_test, 100));
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
