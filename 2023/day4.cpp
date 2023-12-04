//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <vector>
#include <tuple>
#include <string_view>
#include <unordered_set>
#include <chrono>
#include <iostream>

#include <boost/unordered_set.hpp>
#include <boost/container/small_vector.hpp>

const auto test_data = std::vector{ std::tuple
{R"(Card 1: 41 48 83 86 17 | 83 86  6 31 17  9 48 53
Card 2: 13 32 20 16 61 | 61 30 68 82 17 32 24 19
Card 3:  1 21 53 59 44 | 69 82 63 72 16 21 14  1
Card 4: 41 92 73 84 69 | 59 84 76 51 58  5 54 83
Card 5: 87 83 26 28 32 | 88 30 70 12 93 22 82 36
Card 6: 31 18 13 56 72 | 74 77 10 23 35 67 36 11)", 13, 30}
};

using card_t = std::tuple<int, boost::container::small_vector<int, 10>, boost::container::small_vector<int, 10>>;

auto parse(std::string_view s) {
    auto ret = std::vector<card_t>{};
    phrase_parse(s.begin(), s.end(),
        *("Card" >> int_ >> ':' >> *int_ >> '|' >> *int_),
        space, ret);
    return ret;
}

auto run_a(std::string_view s) {
    const auto cards = parse(s);

    const auto card_score = [](const card_t& card) -> long long {
        const auto& [id, have, winning] = card;
        const auto is_win = [&](int j) { return ranges::find(winning, j) != winning.end(); };
        const auto count = ranges::count_if(have, is_win);
        return count == 0 ? 0 : std::pow(2, count - 1);
    };

    return reduce(cards | rv::transform(card_score));
}

auto run_b(std::string_view s) {
    const auto cards = parse(s);

    auto cards_with_multipliers =
        cards
      | rv::transform([](auto card) { return std::tuple{ 1, std::move(card) }; })
      | ranges::to<std::vector>;

    const auto card_score = [](const card_t& card) -> long long {
        const auto& [id, have, winning] = card;
        const auto is_win = [&](int j) { return ranges::find(winning, j) != winning.end(); };
        return ranges::count_if(have, is_win);
    };

    auto total_score = 0ll;
    for (auto i = 0; i < cards_with_multipliers.size(); ++i) {
        const auto& [multiplier, card] = cards_with_multipliers[i];
        const auto score = card_score(card);
        for (auto j = i+1; j <= i + score; ++j)
            std::get<0>(cards_with_multipliers[j]) += multiplier;
        total_score += multiplier;
    }

    return total_score;
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
