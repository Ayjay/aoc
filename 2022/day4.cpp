#include "aoc.h"

#include <tuple>

#include <boost/fusion/adapted.hpp>
namespace fusion = boost::fusion;
#include <boost/spirit/home/x3.hpp>
#include <bitset>

using section_range = std::bitset<128>;

namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;

using x3::char_;
using x3::int_;
using ascii::space;
using boost::spirit::x3::phrase_parse;

auto test_data =
R"(2-4,6-8
2-3,4-5
5-7,7-9
2-8,3-7
6-6,4-6
2-6,4-8)";

template <class T> struct Debug;

const x3::rule<class elf_range, section_range> elf_range = "elf_range";
const auto make_range = [](auto& ctx) {
    auto first = fusion::at_c<0>(_attr(ctx));
    auto last = fusion::at_c<1>(_attr(ctx));
    for (int i = first; i <= last; ++i)
        _val(ctx).set(i);
};
const auto elf_range_def = (int_ >> '-' >> int_)[make_range];

BOOST_SPIRIT_DEFINE(elf_range);

auto run_a(std::string_view s) {
    std::vector<std::tuple<section_range, section_range>> elf_ranges;
    phrase_parse(s.begin(), s.end(), *(elf_range >> ',' >> elf_range), space, elf_ranges);
    return std::ranges::count_if(elf_ranges, [](auto& range) {
        const auto& [a, b] = range;
        return (a & b) == a ||
               (b & a) == b;
    });
}

auto run_b(std::string_view s) {
    std::vector<std::tuple<section_range, section_range>> elf_ranges;
    phrase_parse(s.begin(), s.end(), *(elf_range >> ',' >> elf_range), space, elf_ranges);
    return std::ranges::count_if(elf_ranges, [](auto& range) {
        const auto& [a, b] = range;
        return (a & b).any();
    });
}

int main() {
    run(run_a, run_b, 2, 4, test_data, get_input(4));
}