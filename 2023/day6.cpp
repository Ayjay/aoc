//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <vector>
#include <tuple>
#include <string_view>

const auto test_data = std::vector{ std::tuple
{R"(Time:      7  15   30
Distance:  9  40  200)", 288ll, 71503ll}
};

auto parse_a(std::string_view s) {
    auto times_distances = std::tuple<std::vector<int>, std::vector<int>>{};
    phrase_parse(s.begin(), s.end(),
        "Time: " >> *int_ >> "Distance:" >> *int_,
        space, times_distances);

    const auto& [times,distances] = times_distances;

    return rv::zip(times, distances) | ranges::to<std::vector>;
}

const auto winning_count = [](auto race) {
    const auto [time, distance] = race;
    const auto time_d = static_cast<double>(time);
    const auto distance_d = static_cast<double>(distance);
    const auto sol1_pre = (time_d + std::sqrt(std::pow(time_d,2) - 4 * distance_d)) / 2.0d;
    const auto sol1 = static_cast<long long>(std::floor(std::nextafter(sol1_pre, 0.0d)));
    const auto sol2_pre = (time_d - std::sqrt(std::pow(time_d,2) - 4 * distance_d)) / 2.0d;
    const auto sol2 = static_cast<long long>(std::ceil(std::nextafter(sol2_pre, std::numeric_limits<double>::infinity())));
    return sol1 - sol2 + 1;
};

auto run_a(std::string_view s) {
    auto races = parse_a(s);
    return reduce(races | rv::transform(winning_count), std::multiplies{});
}

auto parse_b(std::string_view s) {
    auto no_space = s | rv::remove(' ');
    auto time_distance = std::tuple<long long, long long>{};
    phrase_parse(no_space.begin(), no_space.end(),
        "Time:" >> long_long >> "Distance:" >> long_long,
        space, time_distance);
    return time_distance;
}

auto run_b(std::string_view s) {
    return winning_count(parse_b(s));
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
