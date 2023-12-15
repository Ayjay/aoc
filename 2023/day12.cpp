//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <vector>
#include <tuple>
#include <string_view>

using result_type = long long;
const auto test_data = std::vector{ std::tuple<std::string_view, std::optional<result_type>, std::optional<result_type>>
{R"(???.### 1,1,3
.??..??...?##. 1,1,3
?#?#?#?#?#?#?#? 1,3,1,6
????.#...#... 4,1,1
????.######..#####. 1,6,5
?###???????? 3,2,1)", 21, {}}
};

auto get_combinations_impl(std::string_view spring, auto report) {
    if (report.size() == 1) {
        const auto group_size = report.front();
        const auto can_be_spring = [](char c) { return c == '?' || c == '#'; };
        const auto it = ranges::find(spring, '#');
        if (it != spring.end()) {
            // have at least 1 confirmed spot, must place there
            const auto is_not_confirmed = [](char c) { return c != '#';};
            const auto confirmed_count = std::find_if(it, spring.end(), is_not_confirmed) - it;
            const auto to_place = confirmed_count - group_size;
            if (to_place == 0)
                return 1;
            const auto reversed_start = std::reverse_iterator{it};
            const auto possible_start = std::find(reversed_start, std::min(reversed_start+to_place, spring.rend()), '.');
        }
    }
    return 0;
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
