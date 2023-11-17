//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <tuple>
#include <cmath>
#include <array>
#include <iostream>
#include <deque>
#include <unordered_set>
#include <map>
#include <valarray>
#include <stack>

#include <fmt/std.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>

#include <boost/array.hpp>

#include <boost/unordered_set.hpp>
#include <boost/container_hash/hash.hpp>
#include <boost/icl/interval_set.hpp>

#include <boost/fusion/adapted.hpp>
namespace fusion = boost::fusion;

#include <boost/spirit/home/x3.hpp>

namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;

using x3::char_;
using x3::int_;
using x3::long_long;
using x3::lexeme;
using x3::alpha;
using ascii::space;
using ascii::lit;
using boost::spirit::x3::phrase_parse;

const auto test_data = std::vector{ std::tuple
{R"(root: pppw + sjmn
dbpl: 5
cczh: sllz + lgvd
zczc: 2
ptdq: humn - dvpt
dvpt: 3
lfqf: 4
humn: 5
ljgn: 2
sjmn: drzm * dbpl
sllz: 4
pppw: cczh / lfqf
lgvd: ljgn * ptdq
drzm: hmdt - zczc
hmdt: 32)"sv, 152, -2
},
{R"()"sv, -2, -2}
};

using monkey_name_t = std::string;
using monkey_combination_t = std::tuple<monkey_name_t, char, monkey_name_t>;
using monkey_operation_t = std::variant<long long, monkey_combination_t>;

const x3::rule<class monkey_combination, monkey_combination_t> monkey_combination = "monkey combination";
const auto monkey_combination_def = lexeme[ *alpha ] >> (char_('+') | char_('-') | char_('*') | char_('/')) >> lexeme[ *alpha ];
BOOST_SPIRIT_DEFINE(monkey_combination);

const x3::rule<class monkey_operation, monkey_operation_t> monkey_operation = "monkey operation";
const auto monkey_operation_def = long_long | monkey_combination;

BOOST_SPIRIT_DEFINE(monkey_operation);

auto parse(std::string_view s) {
    auto monkeys = std::unordered_map<monkey_name_t, monkey_operation_t>{};
    phrase_parse(s.begin(), s.end(),
        *(lexeme[ *alpha ] >> ':' >> monkey_operation),
        space, monkeys);
    //fmt::print("{}", monkeys);
    return monkeys;
}

auto run_a(std::string_view s) {
    const auto monkeys = parse(s);
    if (monkeys.empty())
        return -2ll;

    auto m = std::stack<std::string>{};
    auto values = std::unordered_map<monkey_name_t, long long>{};
    m.push("root");
    while (!m.empty()) {
        const auto next = m.top();
        if (const auto v = values.find(next); v != values.end()) {
            m.pop();
        } else {
            std::visit(overload(
                [&](long long i) { values[next] = i; m.pop(); },
                [&](const monkey_combination_t& comb) { 
                    const auto& [left_name, op, right_name] = comb;
                    const auto left = values.find(left_name);
                    const auto right = values.find(right_name);
                    if (left != values.end() && right != values.end()) {
                        switch (op) {
                            case '+': values[next] = left->second + right->second; break;
                            case '-': values[next] = left->second - right->second; break;
                            case '*': values[next] = left->second * right->second; break;
                            case '/': values[next] = left->second / right->second; break;
                        }
                        m.pop();
                    } else {
                        if (left == values.end())
                            m.push(left_name);
                        if (right == values.end())
                            m.push(right_name);
                    }
                }
            ), monkeys.at(next));
        }
    }
    return values.at("root");
}

auto run_b(std::string_view s) {
    return -1;
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
