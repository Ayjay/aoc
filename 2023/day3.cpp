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
.664.598..)", 4361, 467835}
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

const auto get_neighbours = [](number_t n) {
    auto above = rv::iota(n.col - 1, n.col + n.length + 1) | rv::transform([=](int c) { return std::tuple{ n.row - 1,c }; });
    auto below = rv::iota(n.col - 1, n.col + n.length + 1) | rv::transform([=](int c) { return std::tuple{ n.row + 1,c }; });
    auto left = ranges::single_view(std::tuple{ n.row, n.col - 1 });
    auto right = ranges::single_view(std::tuple{ n.row, n.col + n.length });
    return rv::concat(above, below, left, right);
};

auto make_symbol_pred(const auto& lines, int width) {
    return [&lines,width](auto pos) {
        auto [row, col] = pos;
        return row >= 0 && row < lines.size() &&
            col >= 0 && col < width &&
            lines[row][col] != '.' &&
            !std::isdigit(lines[row][col]);
    };
}

auto run_a(std::string_view s) {
    const auto lines = get_lines(s);
    const auto width = lines.front().size();
    const auto numbers = parse(lines);
    const auto is_symbol = make_symbol_pred(lines, width);
    const auto is_part = [&](number_t num) {
        return ranges::any_of(get_neighbours(num), is_symbol);
    };

    return reduce(
          numbers
        | rv::filter(is_part)
        | rv::transform(&number_t::num));
}

auto run_b(std::string_view s) {
    const auto lines = get_lines(s);
    const auto height = lines.size();
    const auto width = lines.front().size();
    const auto numbers = parse(lines);
    const auto find_adjacent_numbers = [&numbers](auto pos) {
        const auto [row, col] = pos;
        const auto is_adjacent = [row,col](number_t n) {
            return n.row >= row-1 && n.row <= row+1 &&
                n.col <= col + 1 && n.col + n.length >= col;
        };

        return numbers | rv::filter(is_adjacent) | ranges::to<std::vector>;
    };

    auto cells = rv::enumerate(lines)
        | rv::transform([](auto r) {
            const auto& [row, line] = r;
            return rv::enumerate(line)
                | rv::transform([row](auto c) {
                const auto [col, ch] = c;
                return std::tuple{ row, col, ch };
            });
        })
        | rv::join;

    const auto is_star = [](auto c) {
        const auto [row, col, ch] = c;
        return ch == '*';
    };

    const auto to_position = [](auto c) {
        const auto [row, col, ch] = c;
        return std::tuple{ row,col };
    };

    return reduce(
        cells
        | rv::filter(is_star)
        | rv::transform(to_position)
        | rv::transform(find_adjacent_numbers)
        | rv::cache1
        | rv::filter([](const auto& adjacents) { return adjacents.size() == 2; })
        | rv::transform([](const auto& adjacents) { return adjacents[0].num * adjacents[1].num; })
    );
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
