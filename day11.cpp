#include "aoc.h"

#include <tuple>
#include <cmath>
#include <array>
#include <iostream>
#include <deque>
#include <map>

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
    If false: throw to monkey 1)", 10605ll, 2713310158ll}
};

struct old {
    friend std::ostream& operator<<(std::ostream& os, const old&) {
        return os << "old";
    }
};
template <> struct fmt::formatter<old> : ostream_formatter {};

using multiplicand_t = boost::variant<old, int>;
template <> struct fmt::formatter<multiplicand_t> : ostream_formatter {};

std::ostream& operator<<(std::ostream& os, const std::map<int, long long>& m) {
    for (auto [k, v] : m) {
        os << k << ':' << v << '\n';
    }
    return os;
}
template <> struct fmt::formatter<std::map<int, long long>> : ostream_formatter {};

struct monkey {
    int id;
    std::deque<long long> items;
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

auto parse(std::string_view s) {
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
    return monkeys;
}

auto run_a(std::string_view s) {
    auto monkeys = parse(s);

    std::map<int, int> monkey_activity;

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

using expected_t = std::map<int, std::map<int, long long>>;

std::optional<std::map<int, long long>> run(std::vector<monkey> monkeys, int divisor, const expected_t& expected) {
    std::map<int, long long> monkey_activity;

    auto round = [&] {
        constexpr auto log = false;
        for (auto& m : monkeys) {
            if (log) fmt::print("Monkey {}:\n", m.id);
            while (!m.items.empty()) {
                ++monkey_activity[m.id];
                auto item = m.items.front();
                m.items.pop_front();
                if (log) fmt::print("  Monkey inspects an item with a worry level of {}.\n", item);
                auto value = boost::apply_visitor(overload(
                    [&](old) { return item; },
                    [&](int i) { return i; }), m.multiplicand);
                if (log) fmt::print("    Worry level is multiplied by {} to {}.\n", value, item*value);
                switch (m.operation) {
                case '*': item *= value; break;
                case '+': item += value; break;
                }
                if (log) fmt::print("    Monkey gets bored with item. Worry level is divided by {} to {}.\n", divisor, item / divisor);
                //item /= divisor;
                item = sqrt(item);
                int target;
                if (item % m.test == 0) {
                    if (log) fmt::print("    Current worry level is divisible by {}.\n", m.test);
                    target = m.true_target;
                }
                else {
                    if (log) fmt::print("    Current worry level is not divisible by {}.\n", m.test);
                    target = m.false_target;
                }
                if (log) fmt::print("    Item with worry level {} is thrown to monkey {}.\n", item, target);
                monkeys[target].items.push_back(item);
            }
        }
    };

    for (auto r : rv::iota(1, 10001)) {
        round();
        if (auto it = expected.find(r); it != expected.end()) {
            if (monkey_activity != it->second)
                return std::nullopt;
            else
                int x = 0;
        }
    }
    return monkey_activity;
}

auto run_b(std::string_view s) {
    auto monkeys = parse(s);
    auto expected = std::map<int, std::map<int, long long>>{};
    expected[1]     = std::map{ std::pair{0,2ll}, {1,4ll}, {2,3ll}, {3,6ll} };
    expected[20]    = std::map{ std::pair{0,99ll}, {1,97ll}, {2,8ll}, {3,103ll} };
    expected[1000]  = std::map{ std::pair{0,5204ll}, {1,4792ll}, {2,199ll}, {3,5192ll} };
    expected[2000]  = std::map{ std::pair{0,10419ll}, {1,9577ll}, {2,392ll}, {3,10391ll} };
    expected[3000]  = std::map{ std::pair{0,15638ll}, {1,14358ll}, {2,587ll}, {3,15593ll} };
    expected[4000]  = std::map{ std::pair{0,20858ll}, {1,19138ll}, {2,780ll}, {3,20797ll} };
    expected[5000]  = std::map{ std::pair{0,26075ll}, {1,23921ll}, {2,974ll}, {3,26000ll} };
    expected[6000]  = std::map{ std::pair{0,31294ll}, {1,28702ll}, {2,1165ll}, {3,31204ll} };
    expected[7000]  = std::map{ std::pair{0,36508ll}, {1,33488ll}, {2,1360ll}, {3,36400ll} };
    expected[8000]  = std::map{ std::pair{0,41728ll}, {1,38268ll}, {2,1553ll}, {3,41606ll} };
    expected[9000]  = std::map{ std::pair{0,46945ll}, {1,43051ll}, {2,1746ll}, {3,46807ll} };
    expected[10000] = std::map{ std::pair{0,52166ll}, {1,47830ll}, {2,1938ll}, {3,52013ll} };

    for (auto divisor : rv::iota(0)) {
        auto monkey_activity = run(monkeys, divisor, expected);
        if (!monkey_activity) {
            fmt::print("Failed with divisor {}\n", divisor);
        } else {
            fmt::print("Success at divisor {}\n", divisor);
            auto ordered_activity = *monkey_activity | rv::values | ranges::to<std::vector>();
            ranges::sort(ordered_activity, std::greater<>{});

            return ordered_activity[0] * ordered_activity[1];
        }
    }

    return 0ll;
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
