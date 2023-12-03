//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <vector>
#include <tuple>
#include <string_view>

const auto test_data = std::vector{ std::tuple
{R"(467..114..
...*......
..35..633.
......#...
617*......
.....+.58.
..592.....
......755.
...$.*....
.664.598..)", 4361, -2}
};

struct number_t {
    int row;
    int col;
    int length;
    long long num;
};

auto parse(const auto& lines) {
    auto numbers = std::vector<number_t>{};
    for (const auto& [r, row] : rv::enumerate(lines)) {
        for (const auto& [c, col] : rv::enumerate(row)) {
            if (std::isdigit(col)) {
                if (c == 0 || !std::isdigit(row[c - 1])) {
                    const auto num_end = std::find_if(row.begin() + c, row.end(), [](char a) { return !std::isdigit(a); });
                    const auto length = num_end - (row.begin() + c);
                    long long num;
                    std::from_chars(&col, &col + length, num);
                    numbers.push_back({ static_cast<int>(r), static_cast<int>(c), static_cast<int>(length), num });
                }
            }
        }
    }
    return numbers;
}

//ranges::any_view<std::tuple<int,int>> get_neighbours(number_t n) {

auto run_a(std::string_view s) {
    const auto lines = get_lines(s);
    const auto width = lines.front().size();
    const auto numbers = parse(lines);

    auto get_neighbours = [=](number_t n){
        auto above = rv::iota(n.col - 1, n.col + n.length + 1) | rv::transform([=](int c) { return std::tuple{ n.row - 1,c }; });
        auto below = rv::iota(n.col - 1, n.col + n.length + 1) | rv::transform([=](int c) { return std::tuple{ n.row + 1,c }; });
        auto left = ranges::single_view(std::tuple{ n.row, n.col - 1 });
        auto right = ranges::single_view(std::tuple{ n.row, n.col + n.length });
        return rv::concat(above, below, left, right);
    };

    const auto is_symbol = [&](auto pos) {
        auto [row, col] = pos;
        return row >= 0 && row < lines.size() &&
            col >= 0 && col < width &&
            lines[row][col] != '.' &&
            !std::isdigit(lines[row][col]);
    };

    const auto is_part = [&](number_t num) {
        return ranges::any_of(get_neighbours(num), is_symbol);
    };

    return reduce(
          numbers
        | rv::filter(is_part)
        | rv::transform(&number_t::num));
}

auto run_b(std::string_view s) {
    return -1;
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
