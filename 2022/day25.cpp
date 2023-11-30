//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <tuple>
#include <cmath>
#include <array>
#include <iostream>
#include <deque>
#include <unordered_set>
#include <map>
#include <utility>

#include <fmt/std.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>

#include <boost/spirit/home/x3.hpp>
namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;

using x3::char_;
using x3::int_;
using x3::long_long;
using ascii::space;
using ascii::lit;
using boost::spirit::x3::phrase_parse;

const auto test_data = std::vector{ std::tuple
{R"(1=-0-2
12111
2=0=
21
2=01
111
20012
112
1=-1=
1-12
12
1=
122)", "2=-1=0", ""
}
};

constexpr auto snafu_mapping = std::array {'=','-','0','1','2'};
const auto to_snafu(long long i) {
    auto s = std::string{};
    if (i > 0) {
        while (i > 0) {
            i += 2;
            s.push_back(snafu_mapping[i % 5]);
            i /= 5;
        }
        ranges::reverse(s);
    }
    return s;
}

const auto from_snafu = [](std::string_view s) {
    auto ret = 0ll;
    for (auto c : s) {
        ret *= 5;
        auto c_value = static_cast<long long>(ranges::find(snafu_mapping, c) - snafu_mapping.begin()) - 2;
        ret += c_value;
    }
    return ret;
};

auto run_a(std::string_view s) {
    auto lines = get_lines(s);
    return to_snafu(reduce(lines | rv::transform(from_snafu)));
}

auto run_b(std::string_view s) {
    return "";
}

int main() {
    assert(to_snafu(1) == "1");                     assert(1 == from_snafu("1"));
    assert(to_snafu(2) == "2");                     assert(2 == from_snafu("2"));
    assert(to_snafu(3) == "1=");                    assert(3 == from_snafu("1="));
    assert(to_snafu(4) == "1-");                    assert(4 == from_snafu("1-"));
    assert(to_snafu(5) == "10");                    assert(5 == from_snafu("10"));
    assert(to_snafu(6) == "11");                    assert(6 == from_snafu("11"));
    assert(to_snafu(7) == "12");                    assert(7 == from_snafu("12"));
    assert(to_snafu(8) == "2=");                    assert(8 == from_snafu("2="));
    assert(to_snafu(9) == "2-");                    assert(9 == from_snafu("2-"));
    assert(to_snafu(10) == "20");                   assert(10 == from_snafu("20"));
    assert(to_snafu(15) == "1=0");                  assert(15 == from_snafu("1=0"));
    assert(to_snafu(20) == "1-0");                  assert(20 == from_snafu("1-0"));
    assert(to_snafu(2022) == "1=11-2");             assert(2022 == from_snafu("1=11-2"));
    assert(to_snafu(12345) == "1-0---0");           assert(12345 == from_snafu("1-0---0"));
    assert(to_snafu(314159265) == "1121-1110-1=0"); assert(314159265 == from_snafu("1121-1110-1=0"));
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
