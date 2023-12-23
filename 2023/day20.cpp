#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <vector>
#include <tuple>
#include <string_view>
#include <unordered_map>
#include <functional>
#include <variant>

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
    pulse_t operator()(std::string_view from, pulse_t pulse) const {
        return pulse;
    }
};
std::ostream& operator<<(std::ostream& os, broadcaster) { return os << "broadcaster"; }
template <> struct fmt::formatter<broadcaster> : ostream_formatter {};

struct flipflop {
    pulse_t state = low;
    pulse_t operator()(std::string_view from, pulse_t pulse) {
        if (pulse == low)
            state = !state;
        return state;
    }
};
std::ostream& operator<<(std::ostream& os, flipflop) { return os << "flipflop"; }
template <> struct fmt::formatter<flipflop> : ostream_formatter {};

struct conjunction {
    std::unordered_map<std::string_view, pulse_t> remembered;
    pulse_t operator()(std::string_view from, pulse_t pulse) {
        remembered[from] = pulse;
        return ranges::all_of(remembered | rv::values, std::identity{});
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
            auto& [dest_op, _] = modules.at(dest_name);
            if (auto c = std::get_if<conjunction>(&dest_op))
                c->remembered[name] = low;
        }
    }

    return modules;
}

auto run_a(std::string_view s) {
    const auto modules = parse(s);
    return -1;
}

auto run_b(std::string_view s) {
    return -1;
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
