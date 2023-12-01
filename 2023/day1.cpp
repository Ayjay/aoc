#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include "flux.hpp"

#include <vector>
#include <tuple>
#include <string_view>

const auto test_data = std::vector{ std::tuple
{R"(1abc2
pqr3stu8vwx
a1b2c3d4e5f
treb7uchet)", 142, -2},
{R"(two1nine
eightwothree
abcone2threexyz
xtwone3four
4nineeightseven2
zoneight234
7pqrstsixteen)", -2, 281},

};

auto run_a(std::string_view s) {
    auto get_calibration_value = [](auto line) {
        auto digits = flux::from(line)
            .filter([](char c) { return std::isdigit(c); });

        auto nums = digits
            .map([](char c) -> int { return c - '0'; });
        if (digits.size() < 2) return 0;
        return digits.front() * 10 + digits.back();
    };
    auto lines = get_lines(s);
    return reduce(lines | rv::transform(get_calibration_value));
}

struct digit_words : x3::symbols<int> {
    digit_words() {
        add
            ("zero", 0)
            ("one", 1)
            ("two", 2)
            ("three", 3)
            ("four", 4)
            ("five", 5)
            ("six", 6)
            ("seven", 7)
            ("eight", 8)
            ("nine", 9)
            ;
    }
};

auto run_b(std::string_view s) {
    auto get_calibration_value = [](auto line) {
        std::vector<int> digits;
        phrase_parse(line.begin(), line.end(), *(*char_ >> digit_words{}), space, digits);
        if (digits.size() < 2) return 0;
        return digits.front() * 10 + digits.back();
    };
    auto lines = get_lines(s);
    return reduce(lines | rv::transform(get_calibration_value));
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
