//#define BOOST_SPIRIT_X3_DEBUG
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
#include <boost/spirit/home/x3/support/ast/variant.hpp>

namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;

using x3::char_;
using x3::int_;
using x3::long_long;
using ascii::space;
using ascii::lit;
using boost::spirit::x3::phrase_parse;

const auto test_data = std::vector{ std::tuple
{R"([1,1,3,1,1]
[1,1,5,1,1]

[[1],[2,3,4]]
[[1],4]

[9]
[[8,7,6]]

[[4,4],4,4]
[[4,4],4,4,4]

[7,7,7,7]
[7,7,7]

[]
[3]

[[[]]]
[[]]

[1,[2,[3,[4,[5,6,7]]]],8,9]
[1,[2,[3,[4,[5,6,0]]]],8,9])", 13, -1}
};

struct value : std::variant<int, std::vector<value>> {
    using std::variant<int, std::vector<value>>::variant;

    friend std::ostream& operator<<(std::ostream& os, const value& v) {
        if (auto i = std::get_if<int>(&v)) {
            os << *i;
        }
        else {
            const auto& vec = std::get<std::vector<value>>(v);
            os << '[';
            for (const auto& child : vec)
                os << child << ',';
            os << ']';
        }
        return os;
    }
};

using list = std::vector<value>;

const x3::rule<class list_, list> list_ = "list";
const x3::rule<class value_, value> value_ = "value";

const auto list__def = lit('[') >> -(value_ % ',') >> ']';
BOOST_SPIRIT_DEFINE(list_)

const auto value__def = list_ | int_;
BOOST_SPIRIT_DEFINE(value_)
using packet_t = std::pair<list, list>;

struct in_correct_order {
    std::strong_ordering operator()(int left, int right) const {
        return left <=> right;
    }

    std::strong_ordering operator()(int left, const list& right) const {
        return (*this)(list{ left }, right);
    }

    std::strong_ordering operator()(const list& left, int right) const {
        return (*this)(left, list{ right });
    }

    std::strong_ordering operator()(const list& left, const list& right) const {
        return std::lexicographical_compare_three_way(
            left.begin(), left.end(),
            right.begin(), right.end(),
            *this);
    }

    std::strong_ordering operator()(const value& left, const value& right) const {
        return std::visit(*this, left, right);
    }
};

auto run_a(std::string_view s) {
    std::vector<packet_t> packets;
    phrase_parse(s.begin(), s.end(),
        *(list_ >> list_),
        space, packets);
    return reduce(
        rv::enumerate(packets)
      | rv::filter([](const auto& x) {
            const auto& [index, packet] = x;
            const auto& [left, right] = packet;
            return in_correct_order{}(left, right) <= 0;
        })
      | rv::transform([](const auto& x) {
            const auto& [index, packet] = x;
            return index+1;
        })
    );
}

auto run_b(std::string_view s) {
    return -2;
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
