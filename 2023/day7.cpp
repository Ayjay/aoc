//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <vector>
#include <tuple>
#include <string_view>

#include <boost/fusion/adapted/std_array.hpp>

// namespace boost { namespace spirit { namespace traits {
//     template <typename T, size_t N>
//         struct is_container<std::array<T, N>, void> : mpl::false_ { };
// } } }

const auto test_data = std::vector{ std::tuple
{R"(32T3K 765
T55J5 684
KK677 28
KTJJT 220
QQQJA 483)", 6440ll, -2ll}
};
using hand_t = std::array<char, 5>;

using ascii::alnum;
const x3::rule<class hand_, hand_t> hand_ = "hand";
const auto hand__def = alnum >> alnum >> alnum >> alnum >> alnum;
BOOST_SPIRIT_DEFINE(hand_);

auto parse(std::string_view s) {
    auto hands = std::vector<std::tuple<hand_t, long long>>{};
     phrase_parse(s.begin(), s.end(),
         *(hand_ >> long_long),
         space, hands);
    return hands;
}

enum class hand_rank {
    high_card,
    one_pair,
    two_pair,
    three_of_a_kind,
    full_house,
    four_of_a_kind,
    five_of_a_kind
};

hand_rank rank(hand_t hand) {
    auto cards = std::map<char, int>{};
    for (auto c : hand)
        ++cards[c];
    auto counts = cards | rv::values;
    if (ranges::contains(counts, 5))
        return hand_rank::five_of_a_kind;
    if (ranges::contains(counts, 4))
        return hand_rank::four_of_a_kind;
    if (ranges::contains(counts, 3) && ranges::contains(counts, 2))
        return hand_rank::full_house;
    if (ranges::contains(counts, 3))
        return hand_rank::three_of_a_kind;
    if (ranges::contains(counts, 3) && ranges::contains(counts, 2))
        return hand_rank::full_house;
    if (ranges::count(counts, 2) == 2)
        return hand_rank::two_pair;
    if (ranges::contains(counts, 2))
        return hand_rank::one_pair;
    return hand_rank::high_card;
}

int card_score(char c) {
    switch (c) {
        case 'A': return 12;
        case 'K': return 11;
        case 'Q': return 10;
        case 'J': return 9;
        case 'T': return 8;
        default: return c - '0' - 2;
    }
}

bool compare(hand_t left, hand_t right) {
    auto left_rank = rank(left);
    auto right_rank = rank(right);
    if (left_rank != right_rank)
        return left_rank < right_rank;
    return ranges::lexicographical_compare(
        left | rv::transform(card_score), right | rv::transform(card_score));
}

auto run_a(std::string_view s) {
    auto hands = parse(s);
    ranges::sort(hands, compare, [](auto hand_bid) { const auto [hand,bid] = hand_bid; return hand; });
    return reduce(hands
        | rv::enumerate
        | rv::transform([](auto rank_hand) {
             const auto [rank,hand_bid] = rank_hand;
             const auto [hand,bid] = hand_bid;
             return (rank+1) * bid;
        }));
}

auto run_b(std::string_view s) {
    return -1;
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}