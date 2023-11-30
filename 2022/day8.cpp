#include "aoc.h"

#include <tuple>
#include <unordered_set>
#include <iostream>

#include <boost/multi_array.hpp>

#include <boost/fusion/adapted.hpp>
namespace fusion = boost::fusion;

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>

namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;

using x3::char_;
using x3::int_;
using ascii::space;
using boost::spirit::x3::phrase_parse;

const auto test_data = std::vector{ std::tuple
    {R"(30373
25512
65332
33549
35390)", 21, 8},
};

struct cell {
    int height;
    bool visible = false;
};
using tree_grid = boost::multi_array<cell, 2>;
using index_range = tree_grid::index_range;
auto parse(std::string_view s) {
    auto lines = get_lines(s);

    auto ret = tree_grid{boost::extents[lines.size()][lines.front().size()]};
    for (int row = 0; row < lines.size(); ++row) {
        auto& line = lines[row];
        for (int col = 0; col < line.size(); ++col) {
            ret[row][col] = { line[col] - '0' };
        }
    }
    return ret;
}

void print(auto&& rng) {
    for (auto x : rng) {
        for (cell c : x) {
            fmt::print("{}{} ", c.height, c.visible ? 'o' : '_');
        }
        std::cout << '\n';
    }
}

auto run_a(std::string_view s) {
    auto grid = parse(s);
    const auto rows = grid.shape()[0];
    const auto cols = grid.shape()[1];

    auto from_left =
        rv::iota(0u, rows)
      | rv::transform([&](auto row) { return grid[boost::indices[row][index_range(0, cols)]]; });

    auto from_right = 
        rv::iota(0u, rows)
      | rv::transform([&](auto row) { return grid[boost::indices[row][index_range(cols-1, -1, -1)]]; });

    auto from_top = 
        rv::iota(0u, cols)
      | rv::transform([&](auto col) { return grid[boost::indices[index_range(0, rows)][col]]; });

    auto from_bottom =
        rv::iota(0u, cols)
      | rv::transform([&](auto col) { return grid[boost::indices[index_range(rows-1, -1, -1)][col]]; });

    auto mark_visible = [](auto r) {
        using std::begin;
        using std::end;
        auto it = begin(r);
        while (it != end(r)) {
            cell& c = *it;
            c.visible = true;
            it = std::find_if(it, end(r), [&](auto x) { return x.height > c.height; });
        }
    };

    for (auto row : from_left)   { mark_visible(row); }
    for (auto row : from_right)  { mark_visible(row); }
    for (auto col : from_top)    { mark_visible(col); }
    for (auto col : from_bottom) { mark_visible(col); }
    //print(from_left);
    return std::count_if(grid.origin(), grid.origin() + grid.num_elements(), [](const cell& c) { return c.visible; });
}

auto run_b(std::string_view s) {
    auto grid = parse(s);
    const auto rows = grid.shape()[0];
    const auto cols = grid.shape()[1];

    auto cells = rv::cartesian_product(rv::iota(0u, rows), rv::iota(0u, cols));

    auto calc_scenic_score = [&](auto coord) {
        auto [r, c] = coord;
        auto height = grid[r][c].height;

        auto count_visibility = [&](auto view) {
            auto end = std::ranges::find_if(view, [&](auto& c) { return c.height >= height; });
            if (end != view.end()) ++end;
            return std::distance(view.begin(), end);
        };
        auto row_to_left = grid[boost::indices[r][index_range(0, c)]];
        auto left_visible   = count_visibility(ranges::subrange(row_to_left.rbegin(), row_to_left.rend()));
        auto right_visible  = count_visibility(grid[boost::indices[r][index_range(c + 1, cols)]]);

        auto col_to_top = grid[boost::indices[index_range(0, r)][c]];
        auto top_visible    = count_visibility(ranges::subrange(col_to_top.rbegin(), col_to_top.rend()));
        auto bottom_visible = count_visibility(grid[boost::indices[index_range(r+1, rows)][c]]);

        return left_visible * right_visible * top_visible * bottom_visible;
    };

    auto scenic_scores = cells | rv::transform(calc_scenic_score);
    return *ranges::max_element(scenic_scores);
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
