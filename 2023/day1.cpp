//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <vector>
#include <tuple>
#include <string_view>

const auto test_data = std::vector{ std::tuple
{R"(1abc2
pqr3stu8vwx
a1b2c3d4e5f
treb7uchet)", 142, -2}
};

auto run_a(std::string_view s) {
    auto get_calibration_value = [](auto line) {
        auto digits = line
            | rv::filter([](char c) { return std::isdigit(c); })
            | rv::transform([](char c) -> int { return c - '0'; });
        return digits.front() * 10 + digits.back();
    };
    auto lines = get_lines(s);
    return reduce(lines | rv::transform(get_calibration_value));
}

auto run_b(std::string_view s) {
    return -1;
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
