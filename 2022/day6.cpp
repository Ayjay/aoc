#include "aoc.h"

#include <tuple>
#include <unordered_set>

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
    {R"(mjqjpqmgbljsphdztnvjfqwrcgsmlb)", 7, 19},
    {R"(bvwbjplbgvbhsrlpgdmjqwftvncz)", 5, 23},
    {R"(nppdvjthqldpwncqszvftbrmjlhg)", 6, 23},
    {R"(nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg)", 10, 29},
    {R"(zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw)", 11, 26}
};

auto to_set = [](auto a) {
    return a | ranges::to<std::unordered_set>();
};

auto run_a(std::string_view s) {
    auto char_sets =
        s
      | rv::sliding(4)
      | rv::transform(to_set)
      | ranges::to<std::vector>();
    auto enumerated = char_sets | rv::enumerate;
    auto it = std::ranges::find_if(enumerated,
        [](auto a) { auto [_, chars] = a; return chars.size() == 4; });
    return 4 + std::get<0>(*it);
}

auto run_b(std::string_view s) {
    auto char_sets =
        s
      | rv::sliding(14)
      | rv::transform(to_set)
      | ranges::to<std::vector>();
    auto enumerated = char_sets | rv::enumerate;
    auto it = std::ranges::find_if(enumerated,
        [](auto a) { auto [_, chars] = a; return chars.size() == 14; });
    return 14 + std::get<0>(*it);
}

int main() {
    run(run_a, run_b, test_data, get_input(6));
}