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
35390)", 21, 0},
};

using tree_grid = boost::multi_array<int, 2>;
auto parse(std::string_view s) {
    auto lines = get_lines(s);

    auto ret = tree_grid{boost::extents[lines.size()][lines.front().size()]};
    for (int row = 0; row < lines.size(); ++row) {
        auto& line = lines[row];
        for (int col = 0; col < line.size(); ++col) {
            ret[row][col] = line[col] - '0';
        }
    }
    return ret;
}

auto from_left(const tree_grid& grid) {
}

auto run_a(std::string_view s) {
    auto grid = parse(s);
    return -1;
}

auto run_b(std::string_view s) {
    return -1;
}

int main() {
    run(run_a, run_b, test_data, get_input(8));
}
