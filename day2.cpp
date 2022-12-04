#include "aoc.h"

#include <variant>
#include <tuple>

#include <boost/fusion/adapted.hpp>
#include <boost/spirit/home/x3.hpp>

auto test_data =
R"(A Y
B X
C Z)";

struct rock {}; struct paper {}; struct scissors {};
using move = std::variant<rock, paper, scissors>;

struct win {}; struct loss {}; struct draw {};
using outcome = std::variant<win, loss, draw>;

namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;

using x3::char_;
using ascii::space;

struct elf_move : x3::symbols<move> {
    elf_move() {
        add
            ("A", rock{})
            ("B", paper{})
            ("C", scissors{})
            ;
    }
};

struct my_move : x3::symbols<move> {
    my_move() {
        add
            ("X", rock{})
            ("Y", paper{})
            ("Z", scissors{})
            ;
    }
};

struct my_outcome : x3::symbols<move> {
    my_outcome() {
        add
            ("X", loss{})
            ("Y", draw{})
            ("Z", win{})
            ;
    }
};

using boost::spirit::x3::phrase_parse;

auto run_a(std::string_view s) {
    std::vector<std::tuple<move, move>> moves;
    phrase_parse(s.begin(), s.end(), *(elf_move{} >> my_move{}), space, moves);
    return 0;
}

auto run_b(std::string_view s) {
    return 0;
}

int main() {
    run(run_a, run_b, 15, 12, test_data, get_input(2));
}