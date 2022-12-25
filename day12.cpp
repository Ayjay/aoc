#include "aoc.h"

#include <tuple>
#include <cmath>
#include <array>
#include <iostream>
#include <deque>
#include <map>

#include <fmt/std.h>
#include <fmt/ostream.h>

#include <boost/container_hash/hash.hpp>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>

#include <boost/fusion/adapted.hpp>
namespace fusion = boost::fusion;

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>

namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;

using x3::char_;
using x3::int_;
using x3::long_long;
using ascii::space;
using ascii::lit;
using boost::spirit::x3::phrase_parse;

const auto test_data = std::vector{ std::tuple
{R"(Sabqponm
abcryxxl
accszExk
acctuvwj
abdefghi)", 31, -2}
};

auto run_a(std::string_view s) {
    auto lines = get_lines(s);
    const int rows = lines.size();
    const int cols = lines.front().size();
    using point = std::pair<int, int>;
    using valid_point = point;
    auto point_to_id = [=](valid_point p) { return p.first * cols + p.second; };
    auto id_to_point = [=](int id) { return point{ id / cols, id % cols }; };
    valid_point start, end;
    auto get_height = [&](valid_point p) {
        char height = lines[p.first][p.second];
        switch (height) {
        case 'S': start = p; return 'a';
        case 'E': end = p; return 'z';
        default: return height;
        }
    };

    auto validate_point = [&](point p) -> std::optional<valid_point> {
        if (p.first >= 0 && p.first < rows &&
            p.second >= 0 && p.second < cols)
            return p;
        else
            return std::nullopt;
    };

    using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS>;
    auto g = Graph(rows * cols);

    auto check_add_edge = [&](valid_point from, valid_point to) {
        if (get_height(to) <= get_height(from) + 1)
            add_edge(point_to_id(from), point_to_id(to), g);
    };

    for (auto [row_num_unsigned,row] : rv::enumerate(lines)) {
        for (auto [col_num_unsigned, col]: rv::enumerate(row)) {
            auto row_num = static_cast<int>(row_num_unsigned);
            auto col_num = static_cast<int>(col_num_unsigned);
            auto to = valid_point{ row_num, col_num };
            if (auto top_from = validate_point({ row_num - 1, col_num })) {
                check_add_edge(*top_from, to);
                check_add_edge(to, *top_from);
            }
            if (auto left_from = validate_point({ row_num, col_num - 1 })) {
                check_add_edge(*left_from, to);
                check_add_edge(to, *left_from);
            }
        }
    }

    // vector for storing distance property
    std::vector<int> d(num_vertices(g));

    boost::breadth_first_search(g, point_to_id(start), boost::visitor(boost::make_bfs_visitor(boost::record_distances(&d[0], boost::on_tree_edge()))));

    return d[point_to_id(end)];
}

auto run_b(std::string_view s) {
    return -1;
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
