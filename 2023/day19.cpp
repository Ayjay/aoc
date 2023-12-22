//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <vector>
#include <tuple>
#include <string_view>
#include <unordered_map>
#include <array>
#include <fmt/ostream.h>
#include <fmt/std.h>
#include <boost/fusion/include/std_array.hpp>

using result_type = long long;
const auto test_data = std::vector{ std::tuple<std::string_view, std::optional<result_type>, std::optional<result_type>>
{R"(px{a<2006:qkq,m>2090:A,rfg}
pv{a>1716:R,A}
lnx{m>1548:A,A}
rfg{s<537:gd,x>2440:R,A}
qs{s>3448:A,lnx}
qkq{x<1416:A,crn}
crn{x>2662:A,R}
in{s<1351:px,qqz}
qqz{s>2770:qs,m<1801:hdj,R}
gd{a>3333:R,R}
hdj{m>838:A,pv}

{x=787,m=2655,a=1222,s=2876}
{x=1679,m=44,a=2067,s=496}
{x=2036,m=264,a=79,s=2244}
{x=2461,m=1339,a=466,s=291}
{x=2127,m=1623,a=2188,s=1013})", 19114, 167409079868000}
};

enum class category {
    x,
    m,
    a,
    s,
    count
};

auto format_as(category cat) {
    switch (cat) {
        case category::x: return 'x';
        case category::m: return 'm';
        case category::a: return 'a';
        case category::s: return 's';
    }
    std::unreachable();
}

using part_t = std::array<long long, static_cast<size_t>(category::count)>;

struct comparison_t {
    category cat;
    bool greater;
    long long value;
    bool operator()(const part_t& part) const {
        return (greater && part[static_cast<size_t>(cat)] > value) ||
            (!greater && part[static_cast<size_t>(cat)] < value);
    }
};

std::ostream& operator<<(std::ostream& os, comparison_t comp) {
    return os << fmt::format("comparison [{}{}{}]", comp.cat, comp.greater?'>':'<', comp.value);
}
template <> struct fmt::formatter<comparison_t> : ostream_formatter {};

struct always {
    bool operator()(const part_t&) const { return true; }
};
std::ostream& operator<<(std::ostream& os, always) {
    return os << "always";
}
template <> struct fmt::formatter<always> : ostream_formatter {};

struct rule_t {
    std::variant<comparison_t, always> op;
    std::string destination;
    std::optional<std::string_view> operator()(const part_t& part) const {
        if (std::visit([&](const auto& a) { return a(part); }, op))
            return destination;
        else
            return {};
    }
};
std::ostream& operator<<(std::ostream& os, rule_t r) {
    return os << fmt::format("{}->{}", r.op, r.destination);
}
template <> struct fmt::formatter<rule_t> : ostream_formatter {};
//BOOST_FUSION_ADAPT_STRUCT(rule_t, op, destination);
using workflows_t = std::unordered_map<std::string, std::vector<rule_t>>;

struct category_ : x3::symbols<category> {
    category_() {
        add
            ("x", category::x)
            ("m", category::m)
            ("a", category::a)
            ("s", category::s)
        ;
    }
} category_parser;
const auto is_greater = [](auto& ctx) { _val(ctx) = _attr(ctx) == '>'; };
const x3::rule<class comparator, bool> comparator = "comparator";
const auto comparator_def = char_("<>")[is_greater];

using ascii::alpha;
const x3::rule<class comparison_op, rule_t> comparison_op = "comparison op";
const auto make_comparison_op = [](auto& ctx) {
    _val(ctx) = rule_t {
        comparison_t { at_c<0>(_attr(ctx)), at_c<1>(_attr(ctx)), at_c<2>(_attr(ctx)) },
        at_c<3>(_attr(ctx))
    };
};
const auto comparison_op_def = (category_parser >> comparator >> long_long >> ':' >> +alpha)[make_comparison_op];

const x3::rule<class always_op, rule_t> always_op = "always op";
const auto make_always_op = [](auto& ctx) {
    _val(ctx) = rule_t {
        always{},
        _attr(ctx)
    };
};
const auto always_op_def = (+alpha)[make_always_op];

const x3::rule<class rule_parser, rule_t> rule_parser = "rule";
const auto rule_parser_def = comparison_op | always_op;

BOOST_SPIRIT_DEFINE(comparator, comparison_op, always_op, rule_parser);

auto parse(std::string_view s) {
    auto workflows = workflows_t{};
    auto it = s.begin();
    phrase_parse(it, s.end(),
        *(*alpha >> '{' >> (rule_parser % ',') >> '}'),
        space, workflows);

    auto parts = std::vector<part_t>{};
    phrase_parse(it, s.end(),
        *("{x=" >> long_long >> ",m=" >> long_long >> ",a=" >> long_long >> ",s=" >> long_long >> '}'),
        space, parts);

    return std::tuple{workflows,parts};
}


auto run_a(std::string_view s) {
    const auto [workflows,parts] = parse(s);
    const auto accept = [&](part_t part) {
        auto bin = std::string{"in"};
        while (bin != "A" && bin != "R") {
            for (const auto& rule : workflows.at(bin)) {
                auto destination = rule(part);
                if (destination) {
                    bin = *destination;
                    break;
                }
            }
        }
        return bin == "A";
    };

    return reduce(parts | rv::filter(accept) | rv::transform(reduce));
}

struct half_open_range {
    long long lo;
    long long hi;
};
using possible_part = std::array<half_open_range, static_cast<size_t>(category::count)>;
void get_possibles(const auto& workflows, possible_part part, auto rule_it, auto&& out_it) {
    const auto& [op,dest] = *rule_it;
    const auto add_part = [&](possible_part p) {
        if (dest == "A") {
            *out_it++ = p;
        } else if (dest != "R") {
            get_possibles(workflows, p, workflows.at(dest).begin(), out_it);
        }
    };
    std::visit(overload(
        [&](comparison_t comparison) {
            const auto cat_index = static_cast<size_t>(comparison.cat);
            if (comparison.greater) {
                auto hi_part = part;
                hi_part[cat_index].lo = comparison.value+1;
                add_part(hi_part);

                auto lo_part = part;
                lo_part[cat_index].hi = comparison.value;
                get_possibles(workflows, lo_part, rule_it+1, out_it);
            } else {
                auto hi_part = part;
                hi_part[cat_index].lo = comparison.value;
                get_possibles(workflows, hi_part, rule_it+1, out_it);

                auto lo_part = part;
                lo_part[cat_index].hi = comparison.value-1;
                add_part(lo_part);
            }
        },
        [&](always) {
            add_part(part);
        }), op);
}

auto run_b(std::string_view s) {
    const auto [workflows,_] = parse(s);
    auto possibles = std::vector<possible_part>{};
    get_possibles(workflows,
        possible_part{half_open_range{1,4000},half_open_range{1,4000},half_open_range{1,4000},half_open_range{1,4000}},
        workflows.at("in").begin(),
        std::back_inserter(possibles));
    
    const auto get_combinations = [](possible_part part) {
        const auto get_range = [](auto r) { return r.hi - r.lo + 1; };
        return reduce(part | rv::transform(get_range), std::multiplies{});
    };
    return reduce(possibles | rv::transform(get_combinations));
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
