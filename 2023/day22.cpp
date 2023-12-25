//#define BOOST_SPIRIT_X3_DEBUG
#include "aoc.h"

#include <vector>
#include <tuple>
#include <string_view>

#include <boost/container_hash/hash.hpp>
#include <boost/bimap.hpp>
#include <boost/unordered_set.hpp>
#include <boost/bimap/unordered_multiset_of.hpp>

#include <fmt/std.h>
#include <fmt/ranges.h>

using result_type = long long;
const auto test_data = std::vector{ std::tuple<std::string_view, std::optional<result_type>, std::optional<result_type>>
{R"(1,0,1~1,2,1
0,0,2~2,0,2
0,2,3~2,2,3
0,0,4~0,2,4
2,0,5~2,2,5
0,1,6~2,1,6
1,1,8~1,1,9)", 5, 7}
};

using point_t = std::tuple<int, int, int>;
using brick_t = std::tuple<point_t, point_t>;
auto& get_z(point_t& p) { return std::get<2>(p); }
auto get_lowest_z(brick_t brick) {
    const auto [p, q] = brick;
    const auto [p_x, p_y, p_z] = p;
    const auto [q_x, q_y, q_z] = q;
    return p_z;
}

const x3::rule<class point_, point_t> point_ = "point";
const auto point__def = int_ >> ',' >> int_ >> ',' >> int_;
BOOST_SPIRIT_DEFINE(point_);

auto parse(std::string_view s) {
    auto ret = std::vector<brick_t>{};
    phrase_parse(s.begin(), s.end(),
        *(point_ >> '~' >> point_),
        space, ret);
    return ret;
}

auto tile_view(brick_t brick) {
    const auto [p, q] = brick;
    const auto [p_x, p_y, p_z] = p;
    const auto [q_x, q_y, q_z] = q;
    return rv::cartesian_product(
        rv::iota(p_x, q_x+1),
        rv::iota(p_y, q_y+1),
        rv::iota(p_z, q_z+1))
        | rv::transform([](auto a) { return point_t{ a }; });
}

auto fall(brick_t brick) {
    auto& [p, q] = brick;
    auto& [p_x, p_y, p_z] = p;
    auto& [q_x, q_y, q_z] = q;
    --p_z;
    --q_z;
    return brick;
}
auto simulate(auto& bricks) {
    auto tiles = std::unordered_map<point_t, brick_t, boost::hash<point_t>>{};
    for (const auto brick : bricks) {
        for (auto tile : tile_view(brick)) {
            tiles[tile] = brick;
        }
    }
    using boost::bimaps::unordered_multiset_of;
    auto supporting = boost::bimap<unordered_multiset_of<brick_t>, unordered_multiset_of<brick_t>>{};

    auto falling = bricks;
    ranges::sort(falling, std::less{}, [](const brick_t& b) {
        const auto& [p, q] = b;
        const auto [p_x, p_y, p_z] = p;
        const auto [q_x, q_y, q_z] = q;
        return std::min(p_z, q_z);
        });
    auto final_bricks = std::vector<brick_t>{};

    while (!falling.empty()) {
        for (auto it = falling.begin(); it != falling.end(); ) {
            auto& brick = *it;
            if (get_lowest_z(brick) == 0) {
                final_bricks.push_back(brick);
                it = falling.erase(it);
            }
            else {
                auto fallen_brick = fall(brick);
                auto supporters = tile_view(fallen_brick)
                    | rv::transform([&](point_t p) { return tiles.find(p); })
                    | rv::filter([&](auto it) { return it != tiles.end(); })
                    | rv::transform([](auto it) { return it->second; })
                    | rv::remove(brick)
                    | rv::unique;
                if (std::ranges::empty(supporters)) {
                    for (auto tile : tile_view(brick))
                        tiles.erase(tiles.find(tile));
                    brick = fallen_brick;
                    for (auto tile : tile_view(brick))
                        tiles[tile] = brick;
                    ++it;
                }
                else {
                    for (auto supporter : supporters)
                        supporting.insert({ supporter, brick });
                    final_bricks.push_back(brick);
                    it = falling.erase(it);
                }
            }
        }
    }

    // for (auto [l, r] : supporting.left) {
    //     fmt::println("{} -> {}", l, r);
    // }
    return std::tuple{ supporting,final_bricks };
}

auto run_a(std::string_view s) {
    const auto bricks = parse(s);
    const auto [supporting, final_bricks] = simulate(bricks);

    const auto only_one_support = [&](brick_t brick) {
        return supporting.right.count(brick) == 1;
    };
    const auto can_be_disintegrated = [&](brick_t brick) {
        auto [begin,end] = supporting.left.equal_range(brick);
        return ranges::none_of(ranges::subrange{ begin,end }, only_one_support, [](auto kv) { return kv.second; });
    };

    return ranges::count_if(final_bricks, can_be_disintegrated);
}


auto run_b(std::string_view s) {
    const auto bricks = parse(s);
    const auto [supporting, final_bricks] = simulate(bricks);

    const auto only_one_support = [&](brick_t b) {
        return supporting.right.count(b) == 1;
    };

    const auto would_fall = [&](brick_t brick) {
        auto falling_range = boost::unordered_set{ brick };
        auto ret = 0;
        while (!falling_range.empty()) {

            const auto all_falling = [&](brick_t b) {
                const auto [begin, end] = supporting.right.equal_range(b);
                return std::all_of(begin, end, [&](auto kv) { return falling_range.contains(kv.second); });
            };

            const auto next_fall = falling_range
                | rv::for_each([&](brick_t b) {
                    const auto [begin, end] = supporting.left.equal_range(b);
                    return ranges::subrange{ begin,end };
                })
                | rv::transform([](auto kv) { return kv.second; })
                | rv::filter(all_falling)
                | ranges::to<boost::unordered_set>;

            ret += next_fall.size();
            falling_range = std::move(next_fall);
        }
        return ret;
    };

    return reduce(final_bricks | rv::transform(would_fall));
}

int main() {
    run(run_a, run_b, test_data, get_input(AOC_DAY));
}
