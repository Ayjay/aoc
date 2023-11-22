//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"
#include "zip_longest.hpp"

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

using snafu = std::string;

const auto from_snafu_digit = [](char c) {
    switch (c) {
        case '=': return -2;
        case '-': return -1;
        case '0': return 0;
        case '1': return 1;
        case '2': return 2;
    }
    std::unreachable();
};

const auto to_snafu_digit = [](int i) {
    switch (i) {
    case -2: return '=';
    case -1: return '-';
    case  0: return '0';
    case  1: return '1';
    case  2: return '2';
    }
    std::unreachable();
};

snafu add(snafu a, const auto& b) {
    if (a.size() < b.size())
        a.insert(a.begin(), b.size() - a.size(), '0');

    int carry = 0;
    auto b_rng = rv::concat(b | rv::reverse, rv::repeat('0'));
    auto rng = rv::zip(a | rv::reverse, b_rng);
    for ( auto it = rng.begin(); it != rng.end(); ++it) {
        auto& i = std::get<0>(*it);
        auto j = std::get<1>(*it);
        auto result = from_snafu_digit(i) + from_snafu_digit(j) + carry;
        carry = 0;
        if (result < -2) {
            auto it_r = it;
            while (std::get<0>(*--it_r) == '=')
                ;
            auto& n = std::get<0>(*it_r) ;
            n = to_snafu_digit(from_snafu_digit(n)-1);
            result += 5;
        } else if (result > 2) {
            carry = 1;
            result -= 5;
        }
        i = to_snafu_digit(result);
    }
    if (carry)
        a.insert(a.begin(), '1');
    return std::move(a);
}

constexpr auto snafu_mapping = std::array {'=','-','0','1','2'};
const auto to_snafu = [](long long i) {
    auto s = std::string(16, ' ');
    if (auto [ptr,ec] = std::to_chars(s.data(), s.data()+s.size(), i+2, 5); ec != std::errc{})
        throw std::format_error(fmt::format("{}", i));
    else
        s.resize(ptr - s.data());
    ranges::transform(s, s.begin(), [](char c) { return snafu_mapping[c - '0']; });
    return s;
};

const auto from_snafu = [](std::string_view s) {
    auto digits = s | rv::transform([](char c) { 
        return '0' + (ranges::find(snafu_mapping, c) - snafu_mapping.begin());
    });
    auto str = std::string(digits.begin(), digits.end());
    long long i;
    std::from_chars(str.data(), str.data() + str.size(), i, 5);
    return i - 2;
};

auto to_snafu_2(long long i) {
    auto s = std::string{};
    while (i > 0) {
        s.push_back(snafu_mapping[i % 5]);
        i /= 5;
    }
    ranges::reverse(s);
    return s;
}

auto run_a(std::string_view s) {
    auto lines = get_lines(s);
    return to_snafu(reduce(lines | rv::transform(from_snafu)));
}

auto run_b(std::string_view s) {
    return "";
}

int main() {
    std::cout << fmt::format("[{}]\n", to_snafu(1)) << std::flush;
    assert(to_snafu(1) == "1"); assert(1 == from_snafu("1"));
    assert(to_snafu(2) == "2"); assert(2 == from_snafu("2"));
    assert(to_snafu(3) == "1="); assert(3 == from_snafu("1="));
    assert(to_snafu(4) == "1-"); assert(4 == from_snafu("1-"));
    assert(to_snafu(5) == "10"); assert(5 == from_snafu("10"));
    assert(to_snafu(6) == "11"); assert(6 == from_snafu("11"));
    assert(to_snafu(7) == "12"); assert(7 == from_snafu("12"));
    assert(to_snafu(8) == "2="); assert(8 == from_snafu("2="));
    assert(to_snafu(9) == "2-"); assert(9 == from_snafu("2-"));
    assert(to_snafu(10) == "20"); assert(10 == from_snafu("20"));
    assert(to_snafu(15) == "1=0"); assert(15 == from_snafu("1=0"));
    assert(to_snafu(20) == "1-0"); assert(20 == from_snafu("1-0"));
    assert(to_snafu(2022) == "1=11-2"); assert(2022 == from_snafu("1=11-2"));
    assert(to_snafu(12345) == "1-0---0"); assert(12345 == from_snafu("1-0---0"));
    assert(to_snafu(314159265) == "1121-1110-1=0"); assert(314159265 == from_snafu("1121-1110-1=0"));
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
