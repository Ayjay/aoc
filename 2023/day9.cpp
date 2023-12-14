//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <vector>
#include <tuple>
#include <string_view>

using result_type = long long;
const auto test_data = std::vector{ std::tuple<std::string_view, std::optional<result_type>, std::optional<result_type>>
{R"(0 3 6 9 12 15
1 3 6 10 15 21
10 13 16 21 30 45)", 114, 2}
};

auto parse(std::string_view s) {
    const auto lines = get_lines(s);
    const auto to_nums = [](auto line) { 
        auto ret = std::vector<long long>{};
        phrase_parse(line.begin(), line.end(),
            *long_long,
            space, ret);
        return ret;
    };
    return lines | rv::transform(to_nums) | ranges::to<std::vector>;
}

auto adjacent_difference(const auto& rng) {
    const auto diff = [](auto v) {
        return v[1] - v[0];
    };
    return rng | rv::sliding(2) | rv::transform(diff);
}

auto build_layers(const auto& history) {
    auto seq = &history;
    auto seqs = std::vector<std::vector<long long>>{};
    do {
        seqs.push_back(adjacent_difference(*seq) | ranges::to<std::vector>);
        seq = &seqs.back();
    } while (ranges::any_of(*seq, [](auto i) { return i != 0; }));
    return seqs;
}

const auto next_value = [](const auto& history) {
    auto seqs = build_layers(history);
    auto extrapolated = 0ll;
    for (auto s : seqs | rv::reverse | rv::drop(1)) {
        extrapolated += s.back();
    }
    return extrapolated + history.back();
};

auto run_a(std::string_view s) {
    auto histories = parse(s);
    return reduce(histories | rv::transform(next_value));
}

const auto prev_value = [](const auto& history) {
    auto seqs = build_layers(history);
    auto extrapolated = 0ll;
    for (auto s : seqs | rv::reverse | rv::drop(1)) {
        extrapolated = s.front() - extrapolated;
    }
    return history.front() - extrapolated;
};

auto run_b(std::string_view s) {
    auto histories = parse(s);
    return reduce(histories | rv::transform(prev_value));
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
