//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <vector>
#include <tuple>
#include <string_view>

using result_type = long long;
const auto test_data = std::vector{ std::tuple<std::string_view, std::optional<result_type>, std::optional<result_type>>
{R"(rn=1,cm-,qp=3,cm=2,qp-,pc=4,ot=9,ab=5,pc-,pc=6,ot=7)", 1320ll, 145ll}
};

const auto run_hash = [](auto s) {
    return ranges::accumulate(s, 0ll, [](auto acc, auto x) {
        acc += x;
        acc *= 17;
        return acc % 256;
    });
};

auto run_a(std::string_view s) {
    return reduce(sv::split(s, ","sv) | sv::transform(run_hash));
}

auto run_b(std::string_view s) {
    using lens_t = std::tuple<std::string_view, int>;
    const auto run_instruction = [](auto acc, auto instruction_range) {
        const auto get_label = [](auto lens) { return std::get<0>(lens); };
        auto instruction = std::string_view{ instruction_range };
        if (instruction.back() == '-') {
            instruction.remove_suffix(1);
            const auto hash = run_hash(instruction);
            auto& box = acc[hash];
            auto it = ranges::find(box, instruction, get_label);
            if (it != box.end())
                box.erase(it);
        }
        else {
            const auto focal_length = instruction.back() - '0';
            instruction.remove_suffix(2);
            const auto hash = run_hash(instruction);
            auto& box = acc[hash];
            auto it = ranges::find(box, instruction, get_label);
            if (it != box.end())
                std::get<1>(*it) = focal_length;
            else
                box.push_back({ instruction, focal_length });
        }
        return acc;
    };
    const auto boxes = ranges::accumulate(
        sv::split(s, ","sv),
        std::unordered_map<int, std::vector<lens_t>>{},
        run_instruction);

    const auto box_focusing_power = [](const auto& kv) {
        const auto& [box_number, box] = kv;
        const auto lens_focusing_power = [=](auto enumerated_lens) {
            const auto [index, lens] = enumerated_lens;
            const auto [label, focal_length] = lens;
            return (1 + box_number) * (index + 1) * focal_length;
        };
        return ranges::accumulate(rv::enumerate(box) | rv::transform(lens_focusing_power), 0ll);
    };
    return reduce(boxes | rv::transform(box_focusing_power));
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
