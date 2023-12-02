//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <vector>
#include <tuple>
#include <string_view>

#include <boost/fusion/include/std_tuple.hpp>

struct move_t {
    int red = 0;
    int green = 0;
    int blue = 0;
};
using game_t = std::tuple<int, std::vector<move_t>>;
std::ostream& operator<<(std::ostream& os, move_t m) {
    os << fmt::format("red:{} green:{} blue:{}", m.red, m.green, m.blue);
    return os;
}

const auto test_data = std::vector{ std::tuple
{R"(Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue
Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red
Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red
Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green)", 8, -2}
};

const x3::rule<class move_, move_t> move_ = "move";
const auto set_red = [](auto& ctx) { _val(ctx).red = _attr(ctx); };
const auto set_green = [](auto& ctx) { _val(ctx).green = _attr(ctx); };
const auto set_blue = [](auto& ctx) { _val(ctx).blue = _attr(ctx); };
const auto move__def = 
    ((int_ >> "red")[set_red]
  | (int_ >> "blue")[set_blue]
  | (int_ >> "green")[set_green]) % ',';
BOOST_SPIRIT_DEFINE(move_)

auto parse(std::string_view s) {
    auto games = std::vector<game_t>{};
    phrase_parse(
        s.begin(), s.end(),
        *(lit("Game") >> int_ >> ":" >> (move_ % ';')),
        space, games);
    return games;
}

auto run_a(std::string_view s) {
    const auto games = parse(s);
    const auto possible_game = [](const game_t& game) {
        const auto& [id, moves] = game;
        return std::ranges::all_of(moves, [](move_t move) {
            return move.red <= 12 && move.green <= 13 && move.blue <= 14;
        });
    };
    return reduce(games | rv::filter(possible_game) | rv::transform([](const auto& game) { return std::get<0>(game); }));
}

auto run_b(std::string_view s) {
    return -1;
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
