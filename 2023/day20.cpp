//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <vector>
#include <tuple>
#include <string_view>
#include <unordered_map>
#include <functional>
#include <variant>
#include <queue>

#include <fmt/ostream.h>
#include <fmt/std.h>
#include <fmt/ranges.h>

using result_type = long long;
const auto test_data = std::vector{ std::tuple<std::string_view, std::optional<result_type>, std::optional<result_type>>
{R"(broadcaster -> a, b, c
%a -> b
%b -> c
%c -> inv
&inv -> a)", 32000000, {}},
{R"(broadcaster -> a
%a -> inv, con
&inv -> b
%b -> con
&con -> output)", 11687500, {}}
};

using pulse_t = bool;
constexpr pulse_t low = false, high = true;

struct broadcaster {
    std::optional<pulse_t> operator()(std::string_view from, pulse_t pulse) const {
        return pulse;
    }
};
std::ostream& operator<<(std::ostream& os, broadcaster) { return os << "broadcaster"; }
template <> struct fmt::formatter<broadcaster> : ostream_formatter {};

struct flipflop {
    pulse_t state = low;
    std::optional<pulse_t> operator()(std::string_view from, pulse_t pulse) {
        if (pulse == high)
            return {};

        state = !state;
        return state;
    }
};
std::ostream& operator<<(std::ostream& os, flipflop) { return os << "flipflop"; }
template <> struct fmt::formatter<flipflop> : ostream_formatter {};

struct conjunction {
    std::unordered_map<std::string_view, pulse_t> remembered;
    std::optional<pulse_t> operator()(std::string_view from, pulse_t pulse) {
        remembered[from] = pulse;
        return !ranges::all_of(remembered | rv::values, std::identity{});
    }
};
std::ostream& operator<<(std::ostream& os, conjunction) { return os << "conjunction"; }
template <> struct fmt::formatter<conjunction> : ostream_formatter {};

using op_t = std::variant<broadcaster, flipflop, conjunction>;
std::ostream& operator<<(std::ostream& os, op_t op) {
    return os << fmt::format("{}", op);
}
using module_t = std::tuple<op_t, std::vector<std::string>>;
std::ostream& operator<<(std::ostream& os, const module_t& module) {
    return os << fmt::format("{}", module);
}

using x3::alpha;
const x3::rule<class op_parser, op_t> op_parser = "op";
const auto op_parser_def =
    ('%' >> x3::attr(flipflop{})) |
    ('&' >> x3::attr(conjunction{})) |
    (x3::eps >> x3::attr(broadcaster{}));
BOOST_SPIRIT_DEFINE(op_parser);

auto parse(std::string_view s) {
    using x3::eol;
    using x3::eoi;
    auto raw_modules = std::vector<std::tuple<op_t, std::string, std::vector<std::string>>>{};
    phrase_parse(s.begin(), s.end(),
        *(op_parser >> +alpha >> "->" >> +alpha % ',' >> (eol|eoi)),
        space - eol, raw_modules
    );

    auto modules = raw_modules
        | rv::transform([](const auto& raw) {
            const auto& [op, name, destinations] = raw;
            return std::pair{ name, module_t{op, destinations} };
        })
        | ranges::to<std::unordered_map>;

    for (auto& named_module : modules) {
        auto& [name, module] = named_module;
        auto& [op, destinations] = module;
        for (auto& dest_name : destinations) {
            if (auto it = modules.find(dest_name); it != modules.end()) {
                auto& [dest_op, _] = it->second;
                if (auto c = std::get_if<conjunction>(&dest_op))
                    c->remembered[name] = low;
            }
        }
    }

    return modules;
}


auto run_a(std::string_view s) {
    auto modules = parse(s);
    auto low_count = 0ll, high_count = 0ll;

    for (auto _ : rv::iota(0, 1000)) {
        auto q = std::queue<std::tuple<std::string, pulse_t, std::string>>{};
        q.push({ "button", low, "broadcaster" });

        while (!q.empty()) {
            const auto [from, pulse, to] = std::move(q.front());
            q.pop();

            ++(pulse ? high_count : low_count);
            if (auto it = modules.find(to); it != modules.end()) {
                auto& [op, destinations] = it->second;
                const auto next_pulse = std::visit([&](auto& o) { return o(from, pulse); }, op);
                if (next_pulse) {
                    for (auto& d : destinations) {
                        q.push({ to, *next_pulse, d });
                    }
                }
            }
        }
    }
    return low_count * high_count;
}

auto run_b(std::string_view s) {
    auto modules = parse(s);

    auto q = std::queue<std::tuple<std::string, pulse_t, std::string>>{};
    for (auto presses : rv::ints(0ll)) {
        q.push({ "button", low, "broadcaster" });

        while (!q.empty()) {
            const auto [from, pulse, to] = std::move(q.front());
            if (pulse == low && to == "rx")
                return presses;

            q.pop();

            if (auto it = modules.find(to); it != modules.end()) {
                auto& [op, destinations] = it->second;
                const auto next_pulse = std::visit([&](auto& o) { return o(from, pulse); }, op);
                if (next_pulse) {
                    for (auto& d : destinations) {
                        q.push({ to, *next_pulse, d });
                    }
                }
            }
        }
    }
    std::unreachable();
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
