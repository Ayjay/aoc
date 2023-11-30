#include "aoc.h"

#include <tuple>

#include <boost/fusion/adapted.hpp>
#include <boost/spirit/home/x3.hpp>

#include <boost/bimap.hpp>

namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;

using x3::char_;
using ascii::space;
using boost::spirit::x3::phrase_parse;

auto test_data =
R"(A Y
B X
C Z)";

enum move {
    rock, paper, scissors
};

enum outcome {
    win, loss, draw
};

struct elf_move : x3::symbols<move> {
    elf_move() {
        add
            ("A", rock)
            ("B", paper)
            ("C", scissors)
            ;
    }
};

struct my_move : x3::symbols<move> {
    my_move() {
        add
            ("X", rock)
            ("Y", paper)
            ("Z", scissors)
            ;
    }
};

struct outcome_parser : x3::symbols<outcome> {
    outcome_parser() {
        add
            ("X", loss)
            ("Y", draw)
            ("Z", win)
            ;
    }
};

const auto beats = [] {
    boost::bimap<move, move> ret;
    ret.insert({ rock, scissors });
    ret.insert({ scissors, paper });
    ret.insert({ paper, rock });
    return ret;
}();

auto outcome_score(move them, move me) {
    if (them == me) return 3;
    if (beats.left.at(them) == me) return 0;
    else return 6;
}

auto move_score(move m) {
    switch (m) {
    case rock: return 1;
    case paper: return 2;
    case scissors: return 3;
    default: throw 0;
    }
}

auto get_score(move them, move me) {
    return move_score(me) + outcome_score(them, me);
}

auto run_a(std::string_view s) {
    std::vector<std::tuple<move, move>> moves;
    phrase_parse(s.begin(), s.end(), *(elf_move{} >> my_move{}), space, moves);
    return reduce(moves | sv::transform([](auto a) { return std::apply(get_score, a); }));
}

auto get_move_for_result(move them, outcome desired) {
    switch (desired) {
    case win: return beats.right.at(them);
    case draw: return them;
    case loss: return beats.left.at(them);
    default: throw 0;
    }
}

auto run_b(std::string_view s) {
    std::vector<std::tuple<move, outcome>> moves;
    phrase_parse(s.begin(), s.end(), *(elf_move{} >> outcome_parser{}), space, moves);
    return reduce(
        moves
      | sv::transform([](auto a) { auto [them, desired] = a; return std::tuple{ them, get_move_for_result(them, desired) }; })
      | sv::transform([](auto a) { return std::apply(get_score, a); }));
}

int main() {
    run(run_a, run_b, 15, 12, test_data, get_input(2));
}