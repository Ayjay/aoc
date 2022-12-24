#include "aoc.h"

#include <tuple>
#include <unordered_set>
#include <cmath>
#include <array>

#include <boost/container_hash/hash.hpp>

#include <boost/fusion/adapted.hpp>
namespace fusion = boost::fusion;

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>

namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;

using x3::char_;
using x3::int_;
using ascii::space;
using boost::spirit::x3::phrase_parse;

const auto test_data = std::vector{ std::tuple
{R"(addx 15
addx -11
addx 6
addx -3
addx 5
addx -1
addx -8
addx 13
addx 4
noop
addx -1
addx 5
addx -1
addx 5
addx -1
addx 5
addx -1
addx 5
addx -1
addx -35
addx 1
addx 24
addx -19
addx 1
addx 16
addx -11
noop
noop
addx 21
addx -15
noop
noop
addx -3
addx 9
addx 1
addx -3
addx 8
addx 1
addx 5
noop
noop
noop
noop
noop
addx -36
noop
addx 1
addx 7
noop
noop
noop
addx 2
addx 6
noop
noop
noop
noop
noop
addx 1
noop
noop
addx 7
addx 1
noop
addx -13
addx 13
addx 7
noop
addx 1
addx -33
noop
noop
noop
addx 2
noop
noop
noop
addx 8
noop
addx -1
addx 2
addx 1
noop
addx 17
addx -9
addx 1
addx 1
addx -3
addx 11
noop
noop
addx 1
noop
addx 1
noop
noop
addx -13
addx -19
addx 1
addx 3
addx 26
addx -30
addx 12
addx -1
addx 3
addx 1
noop
noop
noop
addx -9
addx 18
addx 1
addx 2
noop
noop
addx 9
noop
noop
noop
addx -1
addx 2
addx -37
addx 1
addx 3
noop
addx 15
addx -21
addx 22
addx -6
addx 1
noop
addx 2
addx 1
noop
addx -10
noop
noop
addx 20
addx 1
addx 2
addx 2
addx -6
addx -11
noop
noop
noop)", 13140ll, -2ll}
};

auto run_a(std::string_view s) {
    auto strength_sum = 0ll;
    auto reg = 1ll;
    auto lines = get_lines(s);
    auto cycle = 1;
    auto check_cycle = [&] {
        if ((cycle + 20) % 40 == 0)
            strength_sum += reg * cycle;
    };
    for (auto command : lines) {
        if (command == "noop") {
            check_cycle(); ++cycle; 
        }
        else {
            int i;
            std::from_chars(&command[5], command.data() + command.size(), i);
            check_cycle(); ++cycle; 
            check_cycle(); ++cycle; 
            reg += i;
        }
    }
    return strength_sum;
}

auto run_b(std::string_view s) {
    return 0;
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
