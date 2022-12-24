#include "aoc.h"

#include <tuple>
#include <cmath>
#include <array>
#include <iostream>
#include <deque>

#include <fmt/std.h>
#include <fmt/ostream.h>

#include <boost/container_hash/hash.hpp>

#include <boost/fusion/adapted.hpp>
namespace fusion = boost::fusion;

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>

namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;

using x3::char_;
using x3::int_;
using x3::long_long;
using ascii::space;
using ascii::lit;
using boost::spirit::x3::phrase_parse;

const auto test_data = std::vector{ std::tuple
{R"(Monkey 0:
  Starting items: 79, 98
  Operation: new = old * 19
  Test: divisible by 23
    If true: throw to monkey 2
    If false: throw to monkey 3

Monkey 1:
  Starting items: 54, 65, 75, 74
  Operation: new = old + 6
  Test: divisible by 19
    If true: throw to monkey 2
    If false: throw to monkey 0

Monkey 2:
  Starting items: 79, 60, 97
  Operation: new = old * old
  Test: divisible by 13
    If true: throw to monkey 1
    If false: throw to monkey 3

Monkey 3:
  Starting items: 74
  Operation: new = old + 3
  Test: divisible by 17
    If true: throw to monkey 0
    If false: throw to monkey 1)", 10605, -2}
};

struct old {
    friend std::ostream& operator<<(std::ostream& os, const old&) {
        return os << "old";
    }
};
template <> struct fmt::formatter<old> : ostream_formatter {};

using multiplicand_t = boost::variant<old, int>;
template <> struct fmt::formatter<multiplicand_t> : ostream_formatter {};

struct monkey {
    int id;
    std::deque<int64_t> items;
    char operation;
    multiplicand_t multiplicand;
    int test;
    int true_target;
    int false_target;

};

BOOST_FUSION_ADAPT_STRUCT(
    monkey,
    id, items, operation, multiplicand, test, true_target, false_target
)

template<typename ... Ts>                                                 // (7) 
struct overload : Ts ... { 
    using Ts::operator() ...;
};
template<class... Ts> overload(Ts...) -> overload<Ts...>;

auto run_a(std::string_view s) {
    auto set_value = [](auto& ctx) { _val(ctx) = _attr(ctx); };
    auto set_old = [](auto& ctx) { _val(ctx) = old{}; };
    const auto multiplicand =
        int_[set_value] |
        lit("old")[set_old];

    auto monkeys = std::vector<monkey>{};
    phrase_parse(s.begin(), s.end(),
        *("Monkey" >> int_ >> ":" >>
          "Starting items: " >> (long_long % ',') >>
          "Operation: new = old " >> char_ >> multiplicand >>
          "Test: divisible by" >> int_ >>
          "If true: throw to monkey" >> int_ >>
          "If false: throw to monkey" >> int_),
        space, monkeys);

    for (auto m : monkeys) {
        fmt::print("Monkey {} has multiplicand {}\n", m.id, m.multiplicand);
    }

    std::unordered_map<int, int> monkey_activity;

    auto round = [&] {
        for (auto& m : monkeys) {
            while (!m.items.empty()) {
                ++monkey_activity[m.id];
                auto item = m.items.front();
                m.items.pop_front();
                auto value = boost::apply_visitor(overload(
                    [&](old) { return item; },
                    [&](int i) { return i; }), m.multiplicand);
                switch (m.operation) {
                case '*': item *= value; break;
                case '+': item += value; break;
                }
                item /= 3;
                monkeys[item % m.test == 0 ? m.true_target : m.false_target].items.push_back(item);
            }
        }
    };

    for (auto _ : rv::iota(0, 20))
        round();

    auto ordered_activity = monkey_activity | rv::values | ranges::to<std::vector>();
    ranges::sort(ordered_activity, std::greater<>{});

    return ordered_activity[0] * ordered_activity[1];
}

auto run_b(std::string_view s) {
    return 0;
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
