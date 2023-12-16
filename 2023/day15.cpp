//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <vector>
#include <tuple>
#include <string_view>

using result_type = long long;
const auto test_data = std::vector{ std::tuple<std::string_view, std::optional<result_type>, std::optional<result_type>>
{R"(rn=1,cm-,qp=3,cm=2,qp-,pc=4,ot=9,ab=5,pc-,pc=6,ot=7)", 1320, {}}
};

const auto run_hash = [](auto s) {
    return ranges::accumulate(s, 0, [](auto acc, auto x) {
        acc += x;
        acc *= 17;
        return acc % 256;
    });
};

auto run_a(std::string_view s) {
    return reduce(sv::split(s, ","sv) | sv::transform(run_hash));
}

auto run_b(std::string_view s) {
    return -1;
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
