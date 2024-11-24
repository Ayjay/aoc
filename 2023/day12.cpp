//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <vector>
#include <tuple>
#include <string_view>
#include <fmt/format.h>

using result_type = long long;
const auto test_data = std::vector{ std::tuple<std::string_view, std::optional<result_type>, std::optional<result_type>>
{R"(???.### 1,1,3
.??..??...?##. 1,1,3
?#?#?#?#?#?#?#? 1,3,1,6
????.#...#... 4,1,1
????.######..#####. 1,6,5
?###???????? 3,2,1)", 21, {}}
};

const auto can_be_spring = [](char c) { return c == '.'; };
const auto can_be_broken = [](char c) { return c == '#'; };

bool match_group(std::string_view spring, const auto& report) {
    auto gaps_required = report.size() - 1;
    if (spring.size() < reduce(report) + gaps_required)
        return false;

    auto it = spring.begin();
    for (const auto broken_count : report) {
        it = std::find_if(it, spring.end(), can_be_broken);
        auto remaining_elements = std::distance(it, spring.end());
        if (remaining_elements < broken_count)
            return false;
        const auto potential_broken_group = std::ranges::subrange(it, it + broken_count);
        if (!ranges::all_of(potential_broken_group, can_be_broken))
            return false;
        it = potential_broken_group.end();
        if (it != spring.end()) {
            if (!can_be_spring(*it))
                return false;
            else
                ++it;
        }
    }
    return std::find_if(it, spring.end(), can_be_broken) == spring.end();
}

std::vector<std::string> resolve_questions(std::string_view spring) {
    std::vector<std::string> input;
    auto question = ranges::find(spring, '?');
    if (question == spring.end()) {
        input.push_back(std::string{ spring });
    } else {
        for (auto suffix : resolve_questions({ question + 1, spring.end() })) {
            input.push_back(fmt::format("{}.{}", std::string(spring.begin(), question), suffix));
            input.push_back(fmt::format("{}#{}", std::string(spring.begin(), question), suffix));
        }
    }
    return input;
}

auto get_combinations_impl(std::string spring, const auto& report) {
    return ranges::count_if(resolve_questions(spring), [&](auto&& s) { return match_group(s, report); });
}

const auto get_combinations = [](std::string_view spring_text) {
    auto spring = std::string{};
    using boost::spirit::x3::parse;
    auto it = spring_text.begin();
    parse(it, spring_text.end(), +char_("?.#"), spring);
    while (spring.back() == '.')
        spring.pop_back();
    while (spring.front() == '.')
        spring.erase(spring.begin());

    auto report = std::vector<int>{};
    phrase_parse(it, spring_text.end(), int_ % ',', space, report);

    return get_combinations_impl(spring, report);
};

auto run_a(std::string_view s) {
    const auto lines = get_lines(s);
    return reduce(lines | rv::transform(get_combinations));
}

auto run_b(std::string_view s) {
    return -1;
}

int main() {
    auto test_springs = std::vector{ std::tuple
        { "? 1", 1 },
        { "?? 1", 2 },
        { "??? 1", 3 },
        { "???.### 1,1,3", 1 },
        { ".??..??...?##. 1,1,3", 4 },
        { "?#?#?#?#?#?#?#? 1,3,1,6", 1 },
        { "????.#...#... 4,1,1", 1 },
        { "????.######..#####. 1,6,5", 4 },
        { "?###???????? 3,2,1", 10 }
    };
    for (auto [spring,expected] : test_springs)
        fmt::println("[{}]: {} (expected {})", spring, get_combinations(spring), expected);
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
