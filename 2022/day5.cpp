#include "aoc.h"

#include <tuple>

#include <sstream>
#include <map>
#include <iterator>

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

auto test_data =
R"(    [D]    
[N] [C]    
[Z] [M] [P]
 1   2   3 

move 1 from 2 to 1
move 3 from 1 to 3
move 2 from 2 to 1
move 1 from 1 to 2)";

using towers_t = std::map<int, std::vector<char>>;
using moves_t = std::tuple<int, int, int>;

std::tuple<towers_t, std::vector<moves_t>> parse(std::string_view s) {
    auto towers = towers_t{};
    auto iss = std::istringstream{ std::string{s} };
    std::string line;
    while (std::getline(iss, line)) {
        if (line[1] == '1') break;
        for (int i = 1; i < line.size(); i += 4) {
            if (line[i] != ' ')
                towers[(i + 3) / 4].push_back(line[i]);
        }
    }
    for (auto& [_, v] : towers)
        std::ranges::reverse(v);
    std::getline(iss, line);
    auto moves = std::vector<moves_t>{};
    phrase_parse(boost::spirit::istream_iterator{iss}, boost::spirit::istream_iterator{},
        *("move" >> int_ >> "from" >> int_ >> "to" >> int_),
        space, moves);
    return { towers,moves };
}

auto run_a(std::string_view s) {
    auto [towers, moves] = parse(s);
    for (auto [amount,from,to] : moves) {
        for (int i = 0; i < amount; ++i) {
            towers[to].push_back(towers[from].back());
            towers[from].pop_back();
        }
    }
    std::string ret;
    for (auto [_, v] : towers)
        ret.push_back(v.back());
    return ret;
}

auto run_b(std::string_view s) {
    auto [towers, moves] = parse(s);
    for (auto [amount,from_idx,to_idx] : moves) {
        auto& from = towers[from_idx];
        auto& to = towers[to_idx];
        std::copy(from.end() - amount, from.end(), std::back_inserter(to));
        from.erase(from.end() - amount, from.end());
    }
    std::string ret;
    for (auto [_, v] : towers)
        ret.push_back(v.back());
    return ret;
}

int main() {
    run(run_a, run_b, "CMZ", "MCD", test_data, get_input(5));
}