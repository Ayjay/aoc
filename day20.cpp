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
using ascii::space;
using ascii::lit;
using boost::spirit::x3::phrase_parse;

const auto test_data = std::vector{ std::tuple
{R"(1
2
-3
3
-2
0
4)", 3, -2
},
{R"()", -2, -2}
};

auto parse(std::string_view s) {
    std::vector<int> ret;
    phrase_parse(s.begin(), s.end(),
        *(int_),
        space, ret);
    return ret;
}

auto mod(auto a, auto b)
{
    return (a%b+b)%b;
}

auto run_a(std::string_view s) {
    const auto nums = parse(s);
    if (nums.size() == 0) return -1;
    auto ptrs = nums | rv::addressof | ranges::to<std::vector>();

    for (auto& n : nums) {
        const auto ptr = ranges::find(ptrs, &n);
        const int pos = ptr - ptrs.begin();
        int dest = (pos + n) >= 0 ? mod(pos + n, static_cast<int>(ptrs.size())) :
            mod(pos + n, static_cast<int>(ptrs.size())) - 1;
        if (dest == 0)
            dest = ptrs.size() - 1;
        assert(dest >= 0 && dest < ptrs.size());
        const auto dest_ptr = ptrs.begin() + dest;
        assert(dest_ptr < ptrs.end());
        assert(ptr < ptrs.end());
        if (ptr < dest_ptr)
            std::rotate(ptr, ptr+1, dest_ptr+1);
        else if (ptr > dest_ptr)
            std::rotate(dest_ptr, ptr, ptr+1);
    }

    return *ptrs[999%ptrs.size()] + *ptrs[1999%ptrs.size()] + *ptrs[2999%ptrs.size()];
}

auto run_b(std::string_view s) {
    return -1;
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
